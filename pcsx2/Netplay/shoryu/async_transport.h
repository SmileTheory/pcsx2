#pragma once
#include <cstdint>
#include <algorithm>
#include <array>
#include <unordered_map>
#include <thread>
#include <system_error>

#include <fstream>

#include "boost_extensions.h"
#include "tools.h"
#include "archive.h"
#include "peer.h"
#include "zed_net.h"

/*IMPROVE:
 * exception handling
 * architecture
 */

namespace shoryu
{
	void prepare_io_service()
	{
		zed_net_init();
	}

	enum class OperationType
	{
		Send,
		Recv
	};

	template<OperationType Operation, int BufferSize>
		class transaction_data
		{
		public:
			zed_net_address_t ep;
			std::array<char, BufferSize> buffer;
			size_t buffer_length;
		};
		template<OperationType Operation, int BufferSize, int BufferQueueSize>
		class transaction_buffer
		{
		public:
			transaction_buffer() : m_next_buffer(0) {}
			std::array<transaction_data<Operation,BufferSize>, BufferQueueSize> buffer;
			transaction_data<Operation,BufferSize>& next()
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				uint32_t i = m_next_buffer;
				m_next_buffer = ++m_next_buffer % BufferQueueSize;
				return buffer[i];
			}
		protected:
			volatile uint32_t m_next_buffer;
			std::mutex m_mutex;
		};

	template<class DataType, int BufferQueueSize = 256, int BufferSize = 1024>
	class async_transport : std::noncopyable
	{
	public:
		typedef typename peer<DataType> peer_type;
		typedef typename peer_data<DataType> peer_data_type;
		typedef std::unordered_map<zed_net_address_t, std::shared_ptr<peer_type>> peer_map_type;
		typedef std::list<const peer_data_type > peer_list_type;
		typedef std::function<void(const std::error_code&)> error_handler_type;
		typedef std::function<void(const zed_net_address_t&, DataType&)> receive_handler_type;

#ifdef ATPORT_ENABLE_LOG
		std::fstream log;
#endif
		async_transport() : m_is_running(false)
		{
#ifdef ATPORT_ENABLE_LOG
			std::string filename = "atport.";
			filename += std::to_string(time_ms());
			filename += ".log";

			log.open(filename, std::ios_base::trunc | std::ios_base::out);
#endif
		}
		virtual ~async_transport()
		{
			stop();
		}
		//Not thread-safe. Avoid concurrent calls with other methods
		void start(unsigned short port, int thread_num = 3)
		{
			m_is_running = true;
			zed_net_udp_socket_open(&m_socket, port, false);
			recv_thread.reset(new std::thread(&async_transport::receive_loop, this));
		}
		//Not thread-safe. Avoid concurrent calls with other methods
		void stop()
		{
			m_is_running = false;
			zed_net_socket_close(&m_socket);
			if (recv_thread && recv_thread->joinable())
			{
				recv_thread->join();
				recv_thread.reset();
			}
			m_peers.clear();
		}

		error_handler_type& error_handler()
		{
			return m_err_handler;
		}
		void error_handler(const error_handler_type& f)
		{
			m_err_handler = f;
		}
		receive_handler_type& receive_handler()
		{
			return m_recv_handler;
		}
		void receive_handler(const receive_handler_type& f)
		{
			m_recv_handler = f;
		}
		void clear_queue(const zed_net_address_t& ep)
		{
			if(m_is_running)
				clear_queue_impl(ep);
		}
		void queue(const zed_net_address_t& ep, const DataType& data)
		{
			if(m_is_running)
				queue_impl(ep, data);
		}
		int send(const zed_net_address_t& ep)
		{
			if(m_is_running)
				return send_impl(ep);
			return -1;
		}
		inline const peer_list_type peers()
		{
			peer_list_type list;
			for (auto& kv : m_peers)
				list.push_back(kv.second->data);
			return list;
		}
		inline const peer_data_type& peer(const zed_net_address_t& ep)
		{
			return m_peers[ep]->data;
		}
		inline int port()
		{
			return m_socket.port;
		}
	protected:
		inline int send_impl(const zed_net_address_t& ep)
		{
			transaction_data<OperationType::Send,BufferSize>& t = m_send_buffer.next();
			t.ep = ep;
			oarchive oa(t.buffer.data(), t.buffer.data() + t.buffer.size());
			int send_n = find_peer(ep).serialize_datagram(oa);

			t.buffer_length = oa.pos();
			if (zed_net_udp_socket_send(&m_socket, ep, t.buffer.data(), t.buffer_length))
			{
				// FIXME: fix error handler
				if (m_err_handler)
					m_err_handler(std::error_code());
			}

#ifdef ATPORT_ENABLE_LOG
				log << "[" << time_ms() << "] send " << t.buffer_length << " (" << send_n << ") " << zed_net_host_to_str(t.ep.host) << ":" << t.ep.port << "\n";
#endif

			return send_n;
		}
		inline uint64_t queue_impl(const zed_net_address_t& ep, const DataType& data)
		{
			return find_peer(ep).queue_msg(data);
		}
		inline void clear_queue_impl(const zed_net_address_t& ep)
		{
			find_peer(ep).clear_queue();
		}
		
		inline peer_type& find_peer(const zed_net_address_t& ep)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			if(m_peers.find(ep) == m_peers.end())
			{
				m_peers[ep].reset(new peer_type());
				m_peers[ep]->data.ep = ep;
			}
			return *m_peers[ep];
		}

		void receive_loop()
		{
			while (m_is_running)
			{
				transaction_data<OperationType::Recv,BufferSize>& t = m_recv_buffer.next();
				int size;
				
				while(m_is_running && (size = zed_net_udp_socket_receive(&m_socket, &t.ep, t.buffer.data(), BufferSize)) <= 0)
				{
					if (size < 0)
						m_err_handler(std::error_code());

					//std::this_thread::yield();
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}

				if (size > 0)
				{
#ifdef ATPORT_ENABLE_LOG
				log << "[" << time_ms() << "] recv " << size << " " << zed_net_host_to_str(t.ep.host) << ":" << t.ep.port << "\n";
#endif
					t.buffer_length = size;
					finalize(t);
				}
			}
		}
		void finalize(const transaction_data<OperationType::Send,BufferSize>& transaction)
		{
			//Outgoing transaction finalization is omitted for better performance
		}
		void finalize(const transaction_data<OperationType::Recv,BufferSize>& transaction)
		{
			iarchive ia(transaction.buffer.data(), transaction.buffer.data() + transaction.buffer_length);
			peer_type& peer = find_peer(transaction.ep);
			try
			{
				peer.deserialize_datagram(ia, [&](DataType& data)
				{
					if(m_recv_handler)
						m_recv_handler(transaction.ep, data);
				});
			}
			//TODO: Exception handling code
			catch(std::exception&)
			{
				if(m_err_handler)
					m_err_handler(std::error_code());
			}
		}

		error_handler_type m_err_handler;
		receive_handler_type m_recv_handler;

		volatile bool m_is_running;

		zed_net_socket_t m_socket;
		std::unique_ptr<std::thread> recv_thread;

		peer_map_type m_peers;

		transaction_buffer<OperationType::Send,BufferSize, BufferQueueSize> m_send_buffer;
		transaction_buffer<OperationType::Recv,BufferSize, BufferQueueSize> m_recv_buffer;

		std::mutex m_mutex;
	};
}
