#include "PrecompiledHeader.h"

#include "AppConfig.h"
#include <wx/stdpaths.h>
#include <iostream>
#include <fstream>
#include "Netplay/NetplayPlugin.h"
#include "Netplay/INetplayDialog.h"

#include "shoryu/session.h"
#include "Message.h"
#include "Replay.h"
#include "NetplaySettings.h"
#include "Utilities.h"


//#define CONNECTION_TEST

class NetplayPlugin : public INetplayPlugin
{
	typedef shoryu::session<Message, EmulatorSyncState> session_type;
	boost::shared_ptr<session_type> _session;
	boost::shared_ptr<boost::thread> _thread;
public:
	NetplayPlugin()
		: _is_initialized(false), _is_stopped(false), _dialog(0)
	{
	}
	void Open()
	{
		_console = Console;
		Utilities::ExecuteOnMainThread([&] { Console_SetActiveHandler(ConsoleWriter_Null); });
		_dialog = INetplayDialog::GetInstance();
		_is_stopped = false;
		_ready_to_print_error_check = [&]() { return !_is_initialized; };
		NetplaySettings& settings = g_Conf->Net;
		if( settings.LocalPort <= 0 || settings.LocalPort > 65535 )
		{
			Stop();
			ConsoleErrorMT(wxString::Format(wxT("NETPLAY: Invalid port: %u."), settings.LocalPort), _ready_to_print_error_check);
			return;
		}
		if( settings.Mode == ConnectMode && (settings.HostPort <= 0 || settings.HostPort > 65535) )
		{
			Stop();
			ConsoleErrorMT(wxString::Format(wxT("NETPLAY: Invalid port: %u."), settings.HostPort), _ready_to_print_error_check);
			return;
		}
		if( settings.Mode == ConnectMode && settings.HostAddress.Len() == 0 )
		{
			Stop();
			ConsoleErrorMT(wxT("NETPLAY: Invalid hostname."), _ready_to_print_error_check);
			return;
		}
		recursive_lock lock(_mutex);
		if(!_dialog->IsShown())
		{
			lock.unlock();
			Stop();
			return;
		}

		shoryu::prepare_io_service();
		_session.reset(new session_type());
#ifdef CONNECTION_TEST
		_session->send_delay_min(40);
		_session->send_delay_max(80);
		_session->packet_loss(25);
#endif
		if(_session->bind(settings.LocalPort))
		{
			_state = SSNone;
			_session->username(std::string((const char*)settings.Username.mb_str(wxConvUTF8)));

			if(g_Conf->Net.SaveReplay)
			{
				_replay.reset(new Replay());
				_replay->Mode(Recording);
			}
			_game_name.clear();
			std::function<bool()> connection_func;
			if(settings.Mode == ConnectMode)
				connection_func = [this, settings]() { return Connect(settings.HostAddress,settings.HostPort, 0); };
			else
				connection_func = [this]() { return Host(0); };

			_thread.reset(new boost::thread([this, connection_func]() {
				_state = connection_func() ? SSReady : SSCancelled;
				if(_dialog)
					_dialog->Close();
			}));
		}
		else
		{
			lock.unlock();
			Stop();
			ConsoleErrorMT(wxString::Format(wxT("NETPLAY: Unable to bind port %u."), settings.LocalPort), _ready_to_print_error_check);
		}
	}
	bool IsInit()
	{
		return _is_initialized;
	}
	void Init()
	{
		_is_initialized = true;
		_is_stopped = false;
		Utilities::SaveSettings();
		Utilities::ResetSettingsToSafeDefaults();
	}
	void Close()
	{
		_is_initialized = false;
		EndSession();
		Utilities::RestoreSettings();
		if(_mcd_backup.size())
		{
			Utilities::WriteMCD(0,0,_mcd_backup);
			_mcd_backup.clear();
		}
		if(_replay)
		{
			if(_state == SSRunning)
			{
				try
				{
					wxDirName dir = (wxDirName)wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
					dir = dir.Combine(wxDirName("replays"));
					wxString replayName = _game_name + wxT(".rep");
					replayName.Replace(wxT("<"),wxT("-"));
					replayName.Replace(wxT(">"),wxT("-"));
					replayName.Replace(wxT(":"),wxT("-"));
					replayName.Replace(wxT("\""),wxT("-"));
					replayName.Replace(wxT("/"),wxT("-"));
					replayName.Replace(wxT("\\"),wxT("-"));
					replayName.Replace(wxT("|"),wxT("-"));
					replayName.Replace(wxT("?"),wxT("-"));
					replayName.Replace(wxT("*"),wxT("-"));
					wxString file = ( dir + replayName ).GetFullPath();
					ConsoleInfoMT(wxT("Saving replay to ") + file);
					_replay->SaveToFile(file);
				}
				catch(std::exception& e)
				{
					Stop();
					ConsoleErrorMT(wxT("REPLAY: ") + wxString(e.what(), wxConvLocal));
				}
			}
			_replay.reset();
		}
		Utilities::ExecuteOnMainThread([&]() {
			UI_EnableEverything();
		});
		Utilities::ExecuteOnMainThread([&] { Console_SetActiveHandler(_console); });
	}
	void ConsoleInfoMT(const wxString& message, std::function<bool()> check = std::function<bool()>())
	{
		if(!check)
		{
			Utilities::ExecuteOnMainThread([&]() {
				_console.WriteLn(Color_StrongGreen, message);
			});
		}
		else
			boost::thread t([=]() {
				while(!check()) shoryu::sleep(100);
				ConsoleInfoMT(message);
		});
	}
	void ConsoleErrorMT(const wxString& message, std::function<bool()> check = std::function<bool()>())
	{
		if(!check)
		{
			Utilities::ExecuteOnMainThread([&]() {
				_console.Error(message);
			});
		}
		else
			boost::thread t([=]() {
				while(!check()) shoryu::sleep(100);
				ConsoleErrorMT(message);
		});
	}
	void ConsoleWarningMT(const wxString& message, std::function<bool()> check = std::function<bool()>())
	{
		if(!check)
		{
			Utilities::ExecuteOnMainThread([&]() {
				_console.Warning(message);
			});
		}
		else
			boost::thread t([=]() {
				while(!check()) shoryu::sleep(100);
				ConsoleWarningMT(message);
		});
	}
	bool Connect(const wxString& ip, unsigned short port, int timeout)
	{
		boost::mutex::scoped_lock connection_lock(_connection_mutex);
		_ready_to_connect_cond.wait(connection_lock);
		shoryu::endpoint ep = shoryu::resolve_hostname(std::string(ip.ToAscii().data()));
		ep.port(port);
		auto state = Utilities::GetSyncState();
		if(state)
		{
			if(_replay)
				_replay->SyncState(*state);
			if(!_session || !_session->join(ep, *state, boost::bind(&NetplayPlugin::CheckSyncStates, this, _1, _2), timeout))
				return false;

			{
				recursive_lock lock(_mutex);
				if(!_session || _session->state() != shoryu::Ready)
					return false;
				_dialog->OnConnectionEstablished(_session->delay());
				auto ep = _session->endpoints()[0];
				std::string player_name = _session->username(ep);
				if(!player_name.length())
				{
					std::stringstream ss;
					ss << ep.address().to_string() << ":" << ep.port();
					player_name = ss.str();
				}
				wxString host_username = wxString(player_name.c_str(), wxConvUTF8);
				_dialog->SetStatus(wxT("Connected to ") + host_username);
				wxString my_name = wxString(_session->username().c_str(), wxConvUTF8);
				if(!my_name.Len())
					my_name = wxT("Me");
				_game_name = wxDateTime::Now().Format(wxT("[%Y.%m.%d %H-%M] "))  + wxT(" [") + Utilities::GetCurrentDiscName() + wxT("] ") + host_username + wxT(" vs ") + my_name;
			}

			int delay = _dialog->WaitForConfirmation();
			if(delay <= 0)
				return false;

#if 0
			_dialog->SetStatus(wxT("Memory card synchronization..."));

			_mcd_backup = Utilities::ReadMCD(0,0);

			shoryu::msec timeout_timestamp = shoryu::time_ms() + 30000;
			size_t mcd_size = Utilities::GetMCDSize(0,0);
			Utilities::block_type compressed_mcd(mcd_size);
			size_t pos = 0;
			while(true)
			{
				bool ready;
				shoryu::message_data data;
				{
					recursive_lock lock(_mutex);
					if(!_session || _session->end_session_request())
						return false;
					_session->send();
					ready = _session->get_data(1-_session->side(), data, 50);
				}
				if(!ready)
				{
					if(timeout_timestamp < shoryu::time_ms())
					{
						ConsoleErrorMT(wxT("NETPLAY: Timeout while synchonizing memory cards."), _ready_to_print_error_check);
						return false;
					}
				}
				else
				{
					if(data.data_length != 9 || memcmp(data.p.get(), "BLOCK_END", 9) != 0)
					{
						std::copy(data.p.get(), data.p.get() + data.data_length, compressed_mcd.data()+pos);
						pos += data.data_length;
					}
					else
					{
						compressed_mcd.resize(pos);
						Utilities::block_type uncompressed_mcd(mcd_size);

						if(!Utilities::Uncompress(compressed_mcd, uncompressed_mcd))
						{
							ConsoleErrorMT(wxT("NETPLAY: Unable to decompress MCD buffer."), _ready_to_print_error_check);
							return false;
						}
						if(uncompressed_mcd.size() != mcd_size)
						{
							ConsoleErrorMT(wxT("NETPLAY: Invalid MCD received from host."), _ready_to_print_error_check);
							return false;
						}
						Utilities::WriteMCD(0,0,uncompressed_mcd);
						if(_replay)
							_replay->Data(uncompressed_mcd);
						break;
					}
				}
			}
#endif
			return true;
		}
		return false;
	}
	bool Host(int timeout)
	{
		boost::mutex::scoped_lock connection_lock(_connection_mutex);
		_ready_to_connect_cond.wait(connection_lock);
		auto state = Utilities::GetSyncState();
		if(state)
		{
			if(_replay)
				_replay->SyncState(*state);
			if(!_session || !_session->create(2, *state, boost::bind(&NetplayPlugin::CheckSyncStates, this, _1, _2), timeout))
				return false;
			
			{
				recursive_lock lock(_mutex);
				if(!_session || _session->state() != shoryu::Ready)
					return false;
				_dialog->OnConnectionEstablished(_session->delay());
				auto ep = _session->endpoints()[0];
				std::string player_name = _session->username(ep);
				if(!player_name.length())
				{
					std::stringstream ss;
					ss << ep.address().to_string() << ":" << ep.port();
					player_name = ss.str();
				}
				wxString client_username = wxString(player_name.c_str(), wxConvUTF8);
				_dialog->SetStatus(wxT("Connection from ") + client_username);
				wxString my_name = wxString(_session->username().c_str(), wxConvUTF8);
				if(!my_name.Len())
					my_name = wxT("Me");
				_game_name = wxDateTime::Now().Format(wxT("[%Y.%m.%d %H-%M] "))  + wxT(" [") + Utilities::GetCurrentDiscName() + wxT("] ") + my_name + wxT(" vs ") + client_username;
			}

			int delay = _dialog->WaitForConfirmation();
			if(delay <= 0)
				return false;

			{
				recursive_lock lock(_mutex);
				if(!_session || _session->state() != shoryu::Ready)
					return false;
				if(delay != _session->delay())
				{
					_session->delay(delay);
					_session->reannounce_delay();
				}
			}

#if 0
			_dialog->SetStatus(wxT("Memory card synchronization..."));

			auto uncompressed_mcd = Utilities::ReadMCD(0,0);
			if(_replay)
				_replay->Data(uncompressed_mcd);
			size_t mcd_size = Utilities::GetMCDSize(0,0);
			Utilities::block_type compressed_mcd(mcd_size);
			if(g_Conf->Net.ReadonlyMemcard)
				_mcd_backup = uncompressed_mcd;
			if(!Utilities::Compress(uncompressed_mcd, compressed_mcd))
			{
				ConsoleErrorMT(wxT("NETPLAY: Unable to compress MCD buffer."), _ready_to_print_error_check);
				return false;
			}

			{
				recursive_lock lock(_mutex);
				if(!_session || _session->state() != shoryu::Ready)
					return false;
				size_t blockSize = 128;
				for(size_t i = 0; i < compressed_mcd.size(); i+=blockSize)
				{
					shoryu::message_data data;
					data.data_length = compressed_mcd.size() - i;
					if(data.data_length > blockSize)
						data.data_length = blockSize;
					data.p.reset(new char[data.data_length]);
					std::copy(compressed_mcd.data()+i, compressed_mcd.data()+i+data.data_length, data.p.get());
					_session->queue_data(data);
				}
				shoryu::message_data data;
				data.data_length = 9;
				data.p.reset(new char[data.data_length]);
				char* blockEndMsg = "BLOCK_END";
				std::copy(blockEndMsg, blockEndMsg + data.data_length, data.p.get());
				_session->queue_data(data);
			}
#endif
			shoryu::msec timeout_timestamp = shoryu::time_ms() + 30000;
			while(true)
			{
				{
					recursive_lock lock(_mutex);
					if(!_session || _session->state() != shoryu::Ready)
						return false;
					if(!_session->send_sync())
						break;
					if(_session->end_session_request())
						return false;
					if(_session->first_received_frame() != -1)
					{
						_session->clear_queue();
						break;
					}
				}
				if(timeout_timestamp < shoryu::time_ms())
				{
					ConsoleErrorMT(wxT("NETPLAY: Timeout while synchonizing memory cards."), _ready_to_print_error_check);
					return false;
				}
			}
			return true;
		}
		return false;
	}
	void EndSession()
	{
		recursive_lock lock(_mutex);
		INetplayDialog* dialog = INetplayDialog::GetInstance();
		if(dialog->IsShown())
		{
			dialog->Close();
			_dialog = 0;
		}
		{
			if(_session)
			{
				if(_session->state() == shoryu::Ready)
				{
					_session->send_end_session_request();
					int try_count = _session->delay() * 4;
					while(_session->send())
					{
						shoryu::sleep(17);
						if(try_count-- == 0)
							break;
					}
				}
				_session->shutdown();
				_session->unbind();
			}
		}
		if(_thread)
		{
			_ready_to_connect_cond.notify_all();
			_thread->join();
			_thread.reset();
		}
		_session.reset();
	}
	void Interrupt()
	{
		EndSession();
	}
	void Stop()
	{
		_is_stopped = true;
		EndSession();
		Utilities::ExecuteOnMainThread([&]() {
			CoreThread.Reset();
		});
	}
	void NextFrame()
	{
		if(_is_stopped || !_session) return;
		_my_frame = Message();
		_session->next_frame();
		/*if(_session->last_error().length())
		{
			ConsoleErrorMT(wxT("NETPLAY: ") + wxString(_session->last_error().c_str(), wxConvLocal), _ready_to_print_error_check);
			_session->last_error("");
		}*/
		if(_state == SSReady)
		{
			if(_thread) _thread.reset();
			_state = SSRunning;
		}
	}
	void AcceptInput(int side)
	{
		if(_is_stopped || !_session) return;
		try
		{
			_session->set(_my_frame);
		}
		catch(std::exception& e)
		{
			Stop();
			ConsoleErrorMT(wxT("NETPLAY: ") + wxString(e.what(), wxConvLocal) + wxT(". Interrupting session."), _ready_to_print_error_check);
		}
		if(_replay)
		{
			Message f;
			_session->get(side, f, 0);
			_replay->Write(side, f);
		}
	}
	u8 HandleIO(int side, int index, u8 value)
	{
		if(_is_stopped || !_session) return value;
		{
			int delay = _session->delay();
			if(_state == SSNone)
				_ready_to_connect_cond.notify_one();
			while(_state == SSNone)
			{
				{
					recursive_lock lock(_mutex);
					if(!_session)
					{
						lock.unlock();
						Stop();
						break;
					}
					if(_session->end_session_request())
					{
						lock.unlock();
						Stop();
						break;
					}
					if(delay != _session->delay())
					{
						delay = _session->delay();
						_dialog->SetInputDelay(delay);
					}
				}
				shoryu::sleep(150);
			}
		}
		if( _state == SSCancelled && !_is_stopped )
		{
			Stop();
		}
		if( _session && _session->end_session_request() && !_is_stopped )
		{
			auto frame = _session->frame();
			Stop();
			ConsoleWarningMT(wxString::Format(wxT("NETPLAY: Session ended on frame %d."), frame), _ready_to_print_error_check);
		}
		if(_is_stopped || !_session) return value;

		Message frame;
		if(side == 0)
			_my_frame.input[index] = value;
		auto timeout = shoryu::time_ms() + 10000;
		try
		{
			while(!_session->get(side, frame, _session->delay()*17))
			{
				_session->send();
				if(_session->end_session_request())
					break;
				if(timeout <= shoryu::time_ms())
				{
					auto frame = _session->frame();
					Stop();
					ConsoleErrorMT(wxString::Format(wxT("NETPLAY: Timeout on frame %d."), frame), _ready_to_print_error_check);
					break;
				}
#ifdef CONNECTION_TEST
				shoryu::sleep(500);
#endif
			}
		}
		catch(std::exception& e)
		{
			Stop();
			ConsoleErrorMT(wxT("NETPLAY: ") + wxString(e.what(), wxConvLocal), _ready_to_print_error_check /* 3000 */);
		}
		value = frame.input[index];
		return value;
	}
protected:
	bool CheckSyncStates(const EmulatorSyncState& s1, const EmulatorSyncState& s2)
	{
		if(memcmp(s1.biosVersion, s2.biosVersion, sizeof(s1.biosVersion)))
		{
			ConsoleErrorMT(wxT("NETPLAY: Bios version mismatch."), _ready_to_print_error_check);
			return false;
		}
		if(memcmp(s1.discId, s2.discId, sizeof(s1.discId)))
		{
			size_t s1discIdLen = sizeof(s1.discId);
			size_t s2discIdLen = sizeof(s2.discId);
			for(size_t i = 0; i < s1discIdLen; i++)
			{
				if(s1.discId[i] == 0)
				{
					s1discIdLen = i;
					break;
				}
			}
			for(size_t i = 0; i < s2discIdLen; i++)
			{
				if(s2.discId[i] == 0)
				{
					s2discIdLen = i;
					break;
				}
			}
			wxString s1discId(s1.discId, wxConvUTF8, s1discIdLen);
			wxString s2discId(s2.discId, wxConvUTF8, s2discIdLen);

			ConsoleErrorMT(wxT("NETPLAY: You are trying to boot different games: ") + 
				Utilities::GetDiscNameById(s1discId) + wxT(" and ") + 
				Utilities::GetDiscNameById(s2discId), _ready_to_print_error_check);
			return false;
		}
		if(s1.skipMpeg != s2.skipMpeg)
		{
			ConsoleErrorMT(wxT("NETPLAY: SkipMpegHack settings mismatch."), _ready_to_print_error_check);
			return false;
		}
		return true;
	}
	
	enum SessionState
	{
		SSNone,
		SSCancelled,
		SSReady,
		SSRunning
	} _state;
	std::function<bool()> _ready_to_print_error_check;
	bool _is_initialized;
	bool _is_stopped;
	boost::condition_variable _ready_to_connect_cond;
	boost::mutex _connection_mutex;
	wxString _game_name;
	Message _my_frame;
	Utilities::block_type _mcd_backup;
	boost::shared_ptr<Replay> _replay;
	INetplayDialog* _dialog;
	boost::recursive_mutex _mutex;
	IConsoleWriter _console;
	typedef boost::unique_lock<boost::recursive_mutex> recursive_lock;
};

INetplayPlugin* INetplayPlugin::instance = 0;

INetplayPlugin& INetplayPlugin::GetInstance()
{
	if(!instance)
		instance = new NetplayPlugin();
	return *instance;
}
