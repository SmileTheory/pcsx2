#include "PrecompiledHeader.h"
#include <boost/thread/condition_variable.hpp>
#include <boost/shared_ptr.hpp>
#include "INetplayDialog.h"
#include "gui/NetplayDialog.h"
#include "App.h"
#include "Utilities.h"

enum NetplayConfigurationPhase
{
	None,
	Settings,
	Confirmation,
	Ready
};

class ConcreteNetplayDialog: public INetplayDialog
{
public:
	void SetCloseEventHandler(const event_handler_type& handler)
	{
		Utilities::ExecuteOnMainThread([&]() {
			_close_handler = handler;
		});
	}
	event_handler_type& GetCancelEventHandler()
	{
		return _close_handler;
	}
	void SetSettings(const NetplaySettings& settings)
	{
		Utilities::ExecuteOnMainThread([&]() {
			m_dialog->SetSettings(settings);
		});
	}
	NetplaySettings GetSettings()
	{
		NetplaySettings settings;
		Utilities::ExecuteOnMainThread([&]() {
			settings = m_dialog->GetSettings();
		});
		return settings;
	}
	void Initialize()
	{
		auto ok_hdl = [&]() {
			m_dialog->GetContent()->Disable();
			m_dialog->EnableOK(false);
			_operation_success = true;
			if(_phase == Settings)
			{
				_phase = Confirmation;
				if(_settings_ready_handler)
					_settings_ready_handler();
			}
			else
				_cond->notify_one();
		};
		auto close_hdl = [&]() { 
			try
			{
				_operation_success = false;
				_cond->notify_one();
				if(_close_handler)
					_close_handler();
			}
			catch(...){}
		};
		Utilities::ExecuteOnMainThread([&]() {
			if(m_dialog)
				m_dialog.reset();
			m_dialog.reset(new NetplayDialog((wxWindow*)GetMainFramePtr()));
			_phase = Settings;
			m_dialog->SetOKHandler(ok_hdl);
			m_dialog->SetCloseEventHandler(close_hdl);
		});
	}
	void Show()
	{
		_is_closed = false;
		Utilities::ExecuteOnMainThread([&]() {
			_cond.reset(new boost::condition_variable());
			m_dialog->Show();
		});
	}
	bool IsShown()
	{
		return !!m_dialog;
	}
	void Close()
	{
		{
			boost::mutex::scoped_lock lock(_close_mutex);
			if(!_is_closed)
				_is_closed = true;
			else
				return;
		}
		_operation_success = false;
		_cond->notify_one();
		Utilities::ExecuteOnMainThread([&]() {
			if(m_dialog) m_dialog.reset();
		});
	}
	void SetConnectionSettingsHandler(const event_handler_type& handler)
	{
		Utilities::ExecuteOnMainThread([&]() {
			_settings_ready_handler = handler;
		});
	}
	int WaitForConfirmation()
	{
		if(!m_dialog)
			return -1;
		if(_phase != Confirmation)
			throw std::exception("invalid state");
		boost::mutex::scoped_lock lock(_cond_mutex);
		_cond->wait(lock);
		_phase = _operation_success ? Ready : None;
		if(m_dialog && _operation_success)
			return m_dialog->GetInputDelayPanel().GetInputDelay();
		else
			return -1;
	}
	void OnConnectionEstablished(int input_delay)
	{
		if(_phase != Confirmation)
			throw std::exception("invalid state");
		Utilities::ExecuteOnMainThread([&]() {
			if(!m_dialog)
				return;
			m_dialog->EnableOK(true);
			InputDelayPanel& p = m_dialog->GetInputDelayPanel();
			p.SetInputDelay(input_delay);
			p.SetReadOnly(GetSettings().Mode != HostMode);

			m_dialog->SetContent(&p);
		});
	}
	int GetInputDelay()
	{
		if(m_dialog)
			return m_dialog->GetInputDelayPanel().GetInputDelay();
		else
			return -1;
	}
	void SetInputDelay(int input_delay)
	{
		Utilities::ExecuteOnMainThread([&]() {
			if(m_dialog)
				m_dialog->GetInputDelayPanel().SetInputDelay(input_delay);
		});
	}
	void SetStatus(const wxString& status)
	{
		Utilities::ExecuteOnMainThread([&]() {
			if(m_dialog) m_dialog->SetStatus(status);
		});
	}
protected:
	bool _is_closed;
	boost::shared_ptr<NetplayDialog> m_dialog;
	boost::shared_ptr<boost::condition_variable> _cond;
	boost::mutex _cond_mutex;
	boost::mutex _close_mutex;;
	bool _operation_success;
	NetplayConfigurationPhase _phase;
	event_handler_type _close_handler;
	event_handler_type _settings_ready_handler;
};


INetplayDialog* INetplayDialog::GetInstance()
{
	if(!instance)
		instance = new ConcreteNetplayDialog();
	return instance;
}

INetplayDialog* INetplayDialog::instance = 0;
