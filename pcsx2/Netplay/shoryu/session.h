#pragma once
#include "async_transport.h"

namespace shoryu
{
	enum MessageType : uint8_t
	{
		None,
		Frame,
		Data,
		Ping, //for pinging
		Join,
		Deny,
		Info, //side, all endpoints, delay
		Wait,
		Delay, //set delay
		Ready, //send to eps, after all eps answered - start the game
		EndSession
	};

	struct message_data
	{
		boost::shared_ptr<char[]> p;
		uint32_t data_length;
	};

	template<typename T, typename StateType>
	struct message
	{
		typedef std::vector<endpoint> endpoint_container;
		typedef boost::asio::ip::address_v4 address_type;

		message() {}

		message(MessageType type) : cmd(type)
		{
		}
		MessageType cmd;
		StateType state;
		int64_t frame_id;
		std::vector<endpoint> eps;
		std::vector<std::string> usernames;
		endpoint host_ep;
		uint32_t rand_seed;
		uint8_t delay;
		uint8_t side;
		uint8_t peers_needed;
		uint8_t peers_count;
		T frame;
		message_data data;
		std::string username;
		
		inline void serialize(shoryu::oarchive& a) const
		{
			a << cmd << side;
			size_t length;
			switch(cmd)
			{
			case Join:
				a << state << host_ep.address().to_v4().to_ulong() << host_ep.port();
				length = username.length();
				a << length;
				if(length)
					a.write((char*)username.c_str(), username.length());
				break;
			case Data:
				a << frame_id << data.data_length;
				a.write(data.p.get(), data.data_length);
			case Deny:
				a << state;
				break;
			case Wait:
				a << peers_needed << peers_count;
				break;
			case Frame:
				a << frame_id;
				frame.serialize(a);
				break;
			case Info:
				a << rand_seed << side << eps.size();
				for(size_t i = 0; i < eps.size(); i++)
				{
					a << eps[i].address().to_v4().to_ulong() << eps[i].port();
					length = usernames[i].length();
					a << length;
					if(length)
						a.write((char*)usernames[i].c_str(), usernames[i].length());
				}
				a << state;
				break;
			case Delay:
				a << delay;
			default:
				break;
			}
		}
		inline void deserialize(shoryu::iarchive& a)
		{
			a >> cmd >> side;
			unsigned long addr;
			unsigned short port;
			switch(cmd)
			{
			case Join:
				a >> state >> addr >> port;
				host_ep = endpoint(address_type(addr), port);
				size_t length;
				a >> length;
				if(length > 0)
				{
					std::auto_ptr<char> str(new char[length]);
					a.read(str.get(), length);
					username.assign(str.get(), str.get()+length);
				}
				break;
			case Data:
				a >> frame_id >> data.data_length;
				data.p.reset(new char[data.data_length]);
				a.read(data.p.get(), data.data_length);
			case Deny:
				a >> state;
				break;
			case Wait:
				a >> peers_needed >> peers_count;
				break;
			case Frame:
				a >> frame_id;
				frame.deserialize(a);
				break;
			case Info:
				endpoint_container::size_type size;
				a >> rand_seed >> side >> size;
				repeat(size)
				{
					a >> addr >> port;
					eps.push_back(endpoint(address_type(addr), port));
					size_t length;
					a >> length;
					if(length > 0)
					{
						std::auto_ptr<char> str(new char[length]);
						a.read(str.get(), length);
						usernames.push_back(std::string(str.get(), str.get()+length));
					}
					else
						usernames.push_back(std::string());
				}
				a >> state;
				break;
			case Delay:
				a >> delay;
			default:
				break;
			}
		}
	};

	template<typename FrameType, typename StateType>
	class session : boost::noncopyable
	{
		typedef message<FrameType, StateType> message_type;
		typedef std::vector<endpoint> endpoint_container;
		typedef std::unordered_map<int64_t, FrameType> frame_map;
		typedef std::vector<frame_map> frame_table;
		typedef std::function<bool(const StateType&, const StateType&)> state_check_handler_type;
		typedef std::vector<std::unordered_map<int64_t, message_data>> data_table;
	public:
#ifdef SHORYU_ENABLE_LOG
		//std::stringstream log;
		std::fstream log;
#endif
		session() : 
			_send_delay_max(0), _send_delay_min(0), _packet_loss(0)
		{
#ifdef SHORYU_ENABLE_LOG
			std::string filename;

			filename = "shoryu.";
			filename += std::to_string(time_ms());
			filename += ".log";

			log.open(filename, std::ios_base::trunc | std::ios_base::out);
#endif
			clear();
		}

		bool bind(int port)
		{
			try
			{
				_async.start(port, 2);
			}
			catch(boost::system::system_error&)
			{
				return false;
			}
			return true;
		}
		void unbind()
		{
			_async.stop();
		}

		bool create(int players, const StateType& state, const state_check_handler_type& handler, int timeout = 0)
		{
			_shutdown = false;
			try_prepare();
			_state = state;
			_state_check_handler = handler;
			_async.receive_handler(boost::bind(&session<FrameType, StateType>::create_recv_handler, this, _1, _2));
			bool connected = true;
			if(create_handler(players, timeout) && _current_state != None)
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] Established! ";
#endif
				connection_established();
			}
			else
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] NotEstablished! ";
#endif
				connected = false;
				_current_state = None;
				_async.receive_handler(boost::bind(&session<FrameType, StateType>::recv_hdl, this, _1, _2));
			}
			return connected;
		}
		bool join(endpoint ep, const StateType& state, const state_check_handler_type& handler, int timeout = 0)
		{
			_shutdown = false;
			try_prepare();
			_state = state;
			_state_check_handler = handler;
			_async.receive_handler(boost::bind(&session<FrameType, StateType>::join_recv_handler, this, _1, _2));
			bool connected = true;
			if(join_handler(ep, timeout) && _current_state != None)
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] Established! ";
#endif
				connection_established();
			}
			else
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] NotEstablished! ";
#endif
				connected = false;
				_current_state = None;
				_async.receive_handler(boost::bind(&session<FrameType, StateType>::recv_hdl,this, _1, _2));
			}
			return connected;
		}

		inline void clear_queue()
		{
			if(_current_state == None)
				throw std::exception("invalid state");
			std::unique_lock<std::mutex> lock(_mutex);
			foreach(auto ep, _eps)
				_async.clear_queue(ep);
		}

		inline void send_end_session_request()
		{
			_end_session_request = true;
			std::unique_lock<std::mutex> lock(_mutex);
			message_type msg(EndSession);

			foreach(auto ep, _eps)
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] EndSn --> " << ep.address().to_string() << ":" << (int)ep.port() << "\n";
#endif
				_async.queue(ep, msg);
				_async.send(ep);
			}
		}
		inline bool end_session_request()
		{
			return _end_session_request;
		}

		inline void reannounce_delay()
		{
			if(_current_state == None)
				throw std::exception("invalid state");
			std::unique_lock<std::mutex> lock(_mutex);
			message_type msg(Delay);
			msg.delay = delay();

			foreach(auto ep, _eps)
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] Delay --> " << ep.address().to_string() << ":" << (int)ep.port() << "\n";
#endif
				_async.queue(ep, msg);
				_async.send(ep);
			}
		}

		inline void queue_data(message_data& data)
		{
			if(_current_state == None)
				throw std::exception("invalid state");
			std::unique_lock<std::mutex> lock(_mutex);
			message_type msg(Data);
			msg.data = data;
			msg.frame_id = _data_index++;

			foreach(auto ep, _eps)
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] Data  --^ " << ep.address().to_string() << ":" << (int)ep.port() << "\n";
#endif
				_async.queue(ep, msg);
			}
		}

		inline bool get_data(int side, message_data& data, int timeout = 0)
		{
			if(_current_state == None)
				throw std::exception("invalid state");

			std::unique_lock<std::mutex> lock(_mutex);
			auto pred = [&]() -> bool {
				if(_current_state != None)
					return _data_table[side].find(_data_index) != _data_table[side].end();
				else
					return true;
			};
			if(timeout > 0)
			{
				if(!_data_cond.timed_wait(lock, boost::posix_time::millisec(timeout), pred))
					return false;
			}
			else
				_data_cond.wait(lock, pred);

			if(_current_state == None)
				throw std::exception("invalid state");
			data = _data_table[side][_data_index];
			_data_table[side].erase(_data_index);
			++_data_index;
			return true;
		}
		
		inline void set(const FrameType& frame)
		{
			if(_current_state == None)
				throw std::exception("invalid state");
			std::unique_lock<std::mutex> lock(_mutex);
			_frame_table[_side][_frame+_delay] = frame;
			message_type msg(Frame);
			msg.frame_id = _frame+_delay;
			msg.frame = frame;
			msg.side = _side;
			for (int i = 0; i < _eps.size(); i++)
			{
				if (i == 1 && _side != 0)
					break;
				if (i == _side)
					continue;
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] Frame " << (int)msg.frame_id << " (" << _side << ") --^ (" << i << ") " << _eps[i].address().to_string() << ":" << (int)_eps[i].port() << "\n";
#endif
				_async.queue(_eps[i], msg);
			}
			send();
		}
		inline int send()
		{
			int n = 0;
			for (int i = 0; i < _eps.size(); i++)
			{
				if (i == 1 && _side != 0)
					break;
				if (i == _side)
					continue;
				n += send(_eps[i]);
			}
			return n;
		}
		inline int send_sync()
		{
			int n = 0;
			for (int i = 0; i < _eps.size(); i++)
			{
				if (i == 1 && _side != 0)
					break;
				if (i == _side)
					continue;
				n += send_sync(_eps[i]);
			}
			return n;
		}
		inline int send_sync(const endpoint& ep)
		{
			return _async.send_sync(ep);
		}

		inline int send(const endpoint& ep)
		{
			if(_packet_loss == 0 && _send_delay_max == 0)
				return _async.send(ep);
			else
			{
				int delay = _send_delay_min;
				int max_add = _send_delay_max - _send_delay_min;
				if(max_add > 0)
					delay += rand() % max_add;
				return _async.send(ep, delay, _packet_loss);
			}
		}
		inline bool get(int side, FrameType& f, int64_t frame, int timeout)
		{
			if(_current_state == None)
				throw std::exception("invalid state");
			if(frame < _delay)
				return true;
			std::unique_lock<std::mutex> lock(_mutex);
			auto pred = [&]() -> bool {
				if(_current_state != None)
					return _frame_table[side].find(frame) != _frame_table[side].end();
				else
					return true;
			};
#ifdef SHORYU_ENABLE_LOG
			log << "[" << time_ms() << "] Waiting for frame " << frame << " side " << side << "\n";
#endif
			if(timeout > 0)
			{
				if(!_frame_cond.wait_for(lock, std::chrono::milliseconds(timeout), pred))
					return false;
			}
			else
				_frame_cond.wait(lock, pred);

			if(_current_state == None)
				throw std::exception("invalid state");
			f = _frame_table[side][frame];
			return true;
		}

		inline bool get(int side, FrameType& f, int timeout)
		{
			return get(side, f, _frame, timeout);
		}

		FrameType get(int side)
		{
			FrameType f;
			get(side, f, 0);
			return f;
		}
		void delay(int d)
		{
			_delay = d;
		}
		int delay()
		{
			return _delay;
		}
		void next_frame()
		{
			_frame++;
		}
		int64_t frame()
		{
			return _frame;
		}
		void frame(int64_t f)
		{
			_frame = f;
		}
		int side()
		{
			return _side;
		}
		bool _shutdown;
		void shutdown()
		{
			_shutdown = true;
			clear();
			_frame_cond.notify_all();
			_connection_sem.post();
		}
		int port()
		{
			return _async.port();
		}
		MessageType state()
		{
			return _current_state;
		}
		endpoint_container endpoints()
		{
			return _eps;
		}
		int64_t first_received_frame()
		{
			return _first_received_frame;
		}
		int64_t last_received_frame()
		{
			return _last_received_frame;
		}
		int send_delay_min()
		{
			return _send_delay_min;
		}
		void send_delay_min(int ms)
		{
			_send_delay_min = ms;
		}
		int send_delay_max()
		{
			return _send_delay_max;
		}
		void send_delay_max(int ms)
		{
			_send_delay_max = ms;
		}
		int packet_loss()
		{
			return _packet_loss;
		}
		void packet_loss(int ms)
		{
			_packet_loss = ms;
		}
		const std::string& last_error()
		{
			std::unique_lock<std::mutex> lock(_error_mutex);
			return _last_error;
		}
		void last_error(const std::string& err)
		{
			std::unique_lock<std::mutex> lock(_error_mutex);
			_last_error = err;
		}
		const std::string& username(const endpoint& ep)
		{
			return _username_map[ep];
		}
		const std::string& username()
		{
			return _username;
		}
		void username(const std::string& name)
		{
			_username = name;
		}
	protected:
		void try_prepare()
		{
			clear();
		}
		void clear()
		{
			_username_map.clear();
			_connection_sem.clear();
			_last_received_frame = -1;
			_first_received_frame = -1;
			_delay = _side = /*_players =*/ 0;
			_frame = 0;
			_data_index = 0;
			_current_state = None;
			//_host = false;
			_eps.clear();
			_end_session_request = false;
			_frame_table.clear();
			_last_error = "";
			_data_table.clear();
			_async.error_handler(std::function<void(const error_code&)>());
			_async.receive_handler(std::function<void(const endpoint&, message_type&)>());
#ifdef SHORYU_ENABLE_LOG
			//log.str("");
#endif
		}
		void connection_established()
		{
#ifdef SHORYU_ENABLE_LOG
			foreach(endpoint& ep, _eps)
			{
				std::string s = ep.address().to_string();
				s += ":" + std::to_string(ep.port());
				log << "\nep " << s << "\n";
			}
#endif
			std::unique_lock<std::mutex> lock1(_connection_mutex);
			std::unique_lock<std::mutex> lock2(_mutex);
			_frame_table.resize(_eps.size() + 1);
			_data_table.resize(_eps.size() + 1);
			_async.error_handler(boost::bind(&session<FrameType, StateType>::err_hdl, this, _1));
			_async.receive_handler(boost::bind(&session<FrameType, StateType>::recv_hdl, this, _1, _2));
		}
		int calculate_delay(uint32_t rtt)
		{
			return (rtt / 32) + 1;
		}

		struct peer_info
		{
			MessageType state;
			uint64_t time;
			int delay;
		};

		typedef std::map<endpoint, peer_info> state_map;

		state_map _states;
		MessageType _current_state;	
		unsigned int _players_needed;
		endpoint _host_ep;
		std::semaphore _connection_sem;
		std::mutex _connection_mutex;
		static const int connection_timeout = 1000;
		StateType _state;
		state_check_handler_type _state_check_handler;
		int64_t _first_received_frame;
		int64_t _last_received_frame;

		bool check_peers_readiness()
		{
#ifdef SHORYU_ENABLE_LOG
			log << "[" << time_ms() << "] Out.Ready ";
#endif
			return send() == 0;
		}

		bool create_handler(int players, int timeout)
		{
			_players_needed = players;
			_current_state = Wait;
			msec start_time = time_ms();
			if(timeout)
			{
				if(!_connection_sem.timed_wait(timeout))
					return false;
			}
			else
				_connection_sem.wait();
			if(_current_state != Ready)
				return false;
			while(true)
			{
				if(timeout > 0 && (time_ms() - start_time > timeout))
					return false;
				if(check_peers_readiness())
					return true;
				sleep(50);
			}
		}
		void create_recv_handler(const endpoint& ep, message_type& msg)
		{
			std::unique_lock<std::mutex> lock(_connection_mutex);

			if(msg.cmd == Join)
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] In.Join from " << ep.address().to_string() << ":" << ep.port() << "\n";
#endif
				_username_map[ep] = msg.username;
				if(!_state_check_handler(_state, msg.state))
				{
					message_type msg(Deny);
					msg.state = _state;
					_async.queue(ep, msg);
					int t = 5;
					while(t --> 0)
					{
						send(ep);
						sleep(50);
					}
					_connection_sem.post();
#ifdef SHORYU_ENABLE_LOG
					log << "[" << time_ms() << "] Out.Deny ";
#endif
					return;
				}
				if(_current_state == Wait)
				{
					peer_info pi = { Join, time_ms(), 0 };
					_states[ep] = pi;
				}
				else
					_states[ep].time = time_ms();
				
				std::vector<endpoint> ready_list;
				ready_list.push_back(msg.host_ep);
				foreach(auto kv, _states)
				{
					if((time_ms() - kv.second.time < 1000) && kv.second.state == Join)
						ready_list.push_back(kv.first);
					if(ready_list.size() >= _players_needed )
						break;
				}

				if(ready_list.size() >= _players_needed)
				{
					if(_current_state == Wait)
					{
						message_type msg;
						msg.cmd = Info;
						msg.rand_seed = (uint32_t)time(0);
						msg.eps = ready_list;
						msg.state = _state;
						_eps = ready_list;
						for(size_t i = 0; i < _eps.size(); i++)
						{
							if(i != _side)
								msg.usernames.push_back(_username_map[_eps[i]]);
							else
								msg.usernames.push_back(_username);
						}

						//_eps.erase(std::find(_eps.begin(), _eps.end(), _eps[_side]));
						srand(msg.rand_seed);
						for(size_t i = 1; i < ready_list.size(); i++)
						{
							msg.side = i;
							_async.queue(ready_list[i], msg);
						}
						_current_state = Ping;
						_side = 0;
					}
					for(size_t i = 1; i < ready_list.size(); i++)
						send(ready_list[i]);
#ifdef SHORYU_ENABLE_LOG
					log << "[" << time_ms() << "] Out.Info ";
#endif
				}
			}
			if(msg.cmd == Ping)
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] In.Ping ";
#endif
				message_type msg;
				msg.cmd = None;
				_async.queue(ep, msg);
				send(ep);
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] Out.None ";
#endif
			}
			if(msg.cmd == Delay)
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] In.Delay ";
#endif
				peer_info pi = { Delay, 0, msg.delay };
				_states[ep] = pi;
				int ready = 0;
				int d = 0;
				foreach(auto kv, _states)
				{
					if(kv.second.state == Delay)
					{
						d += kv.second.delay;
						if( ++ready == (_players_needed - 1))
						{
							d /= _players_needed - 1;
							break;
						}
					}
				}
				if(ready == (_players_needed - 1))
				{
					if(_current_state != Ready)
					{
						message_type msg(Delay);
						msg.delay = d;
						delay(d);
						for(size_t i = 0; i < _eps.size(); i++)
							_async.queue(_eps[i], msg);
						_current_state = Ready;
						_connection_sem.post();
					}
				}
			}
		}

		bool join_handler(const endpoint& host_ep, int timeout)
		{
			_host_ep = host_ep;
			msec start_time = time_ms();
			do
			{
				if(_shutdown)
					return false;
				if(timeout > 0 && (time_ms() - start_time > timeout))
					return false;
				message_type msg(Join);
				msg.username = _username;
				msg.host_ep = host_ep;
				msg.state = _state;
				// Commenting this line out causes multiple connect messages to send every 500ms, which is what we want
				//if(!send(host_ep))
				{
					_async.queue(host_ep, msg);
					send(host_ep);
				}
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] Out.Join ";
#endif
			}
			while(!_connection_sem.timed_wait(500));

			if(_current_state == Deny)
				return false;

			int i = 150;
			while(i-->0)
			{
				if(_shutdown)
					return false;
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] Out.Ping ";
#endif
				foreach(endpoint& ep, _eps)
				{
					_async.queue(ep, message_type(Ping));
					send(ep);
				}
				shoryu::sleep(50);
			}

			int rtt = 0;
			foreach(auto ep, _eps)
			{
				auto peer = _async.peer(ep);
				if(rtt < peer.rtt_avg)
					rtt = peer.rtt_avg;
			}

			message_type msg(Delay);
			msg.delay = calculate_delay(rtt);
			_async.queue(host_ep, msg);

			bool packet_reached = false;
			while(true)
			{
				if(!packet_reached)
					packet_reached = (send(host_ep) == 0);

				if(_shutdown)
					return false;
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] Out.Delay ";
#endif
				if(timeout > 0 && (time_ms() - start_time > timeout))
					return false;
				if(_current_state == Ready)
				{
					if(packet_reached)
						break;
				}
				_connection_sem.timed_wait(50);
			}

			{
				message_type msg(Ready);
				_async.queue(host_ep, msg);
				for(int i = 0; i < delay(); i++)
				{
					if(!send(host_ep)) break;
					sleep(17);
				}
			}
			return true;
		}
		void join_recv_handler(const endpoint& ep, message_type& msg)
		{
			if(ep != _host_ep)
				return;
			std::unique_lock<std::mutex> lock(_connection_mutex);
			if(msg.cmd == Info)
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] In.Info ";
#endif
				_side = msg.side;
				_eps = msg.eps;
				for(size_t i = 0; i < _eps.size(); i++)
				{
					_username_map[_eps[i]] = msg.usernames[i];
				}
				//_eps.erase(std::find(_eps.begin(), _eps.end(), _eps[_side]));
				std::srand(msg.rand_seed);
				_current_state = Info;
				if(!_state_check_handler(_state, msg.state))
					_current_state = Deny;
				_connection_sem.post();
			}
			if(msg.cmd == Deny)
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] In.Deny ";
#endif
				_current_state = Deny;
				_state_check_handler(_state, msg.state);
				_connection_sem.post();
			}
			if(msg.cmd == Delay)
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] In.Delay ";
#endif
				delay(msg.delay);
				if(_current_state != Ready)
				{
					_current_state = Ready;
				}
				_async.queue(ep, message_type(Ready));
				send(ep);
				_connection_sem.post();
			}
			if(msg.cmd == Ping)
			{
#ifdef SHORYU_ENABLE_LOG
				log << "[" << time_ms() << "] In.Ping ";
#endif
				message_type msg;
				msg.cmd = None;
				_async.queue(ep, msg);
				send(ep);
			}
		}
		
		void recv_hdl(const endpoint& ep, message_type& msg)
		{
#ifdef SHORYU_ENABLE_LOG
			if (msg.cmd == Frame)
				log << "[" << time_ms() << "] Frame " << (int)msg.frame_id << " (" << _side << ") <-- (" << (int)msg.side << ") " << ep.address().to_string() << ":" << (int)ep.port() << "\n";
			else if (msg.cmd == Data)
				log << "[" << time_ms() << "] Data  <-- " << ep.address().to_string() << ":" << (int)ep.port() << "\n";
			else if (msg.cmd == Delay)
				log << "[" << time_ms() << "] Delay <-- " << ep.address().to_string() << ":" << (int)ep.port() << "\n";
			else if (msg.cmd == EndSession)
				log << "[" << time_ms() << "] EndSn <-- " << ep.address().to_string() << ":" << (int)ep.port() << "\n";
#endif

			// ignore messages from self
			// FIXME: this shouldn't happen, track down if it does.
			if (msg.side == _side)
				return;

			//if(_sides.find(ep) != _sides.end())
			{
				int side = msg.side; //_sides[ep];

				// if we're server, echo to everyone else
				if (_side == 0 && side != 0)
				{
					for (int i = 1; i < _eps.size(); i++)
					{
						if (i == side)
							continue;
						_async.queue(_eps[i], msg);
						send(_eps[i]);
					}
				}

				if(msg.cmd == Frame)
				{
					std::unique_lock<std::mutex> lock(_mutex);
					_frame_table[side][msg.frame_id] = msg.frame;
					if(_first_received_frame < 0)
						_first_received_frame = msg.frame_id;
					else if(msg.frame_id < _first_received_frame)
						_first_received_frame = msg.frame_id;

					if(_last_received_frame < 0)
						_last_received_frame = msg.frame_id;
					else if(msg.frame_id > _last_received_frame)
						_last_received_frame = msg.frame_id;
					_frame_cond.notify_all();
				}
				if(msg.cmd == Data)
				{
					std::unique_lock<std::mutex> lock(_mutex);
					_data_table[side][msg.frame_id] = msg.data;
					_data_cond.notify_all();
					if (_side == 0 || side == 0)
						send(ep);
				}
				if(msg.cmd == Delay)
				{
					std::unique_lock<std::mutex> lock(_mutex);
					delay(msg.delay);
					if (_side == 0 || side == 0)
						send(ep);
				}
				if(msg.cmd == EndSession)
				{
					std::unique_lock<std::mutex> lock(_mutex);
					_end_session_request = true;
					if (_side == 0 || side == 0)
						send(ep);
				}
			}
		}
		void err_hdl(const error_code& error)
		{
			std::unique_lock<std::mutex> lock(_error_mutex);
			_last_error = error.message();
		}
	private:
		volatile int _delay;
		int64_t _frame;
		int64_t _data_index;
		int _side;
		int _packet_loss;
		int _send_delay_max;
		int _send_delay_min;
		bool _end_session_request;
		
		std::string _username;
		typedef std::map<endpoint, std::string> username_map;

		username_map _username_map;
		std::string _last_error;

		async_transport<message_type> _async;
		endpoint_container _eps;
		frame_table _frame_table;
		std::mutex _mutex;
		std::mutex _error_mutex;
		std::condition_variable _frame_cond;
		std::condition_variable _data_cond;
		data_table _data_table;
	};
}
