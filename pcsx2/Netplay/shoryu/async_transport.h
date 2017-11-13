#pragma once
#include <cstdint>
#include <algorithm>
#include <array>
#include <unordered_map>
#include <thread>

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "boost_extensions.h"
#include "tools.h"
#include "archive.h"
#include "peer.h"

/*IMPROVE:
 * exception handling
 * architecture
 */

namespace shoryu
{
	std::unique_ptr<boost::asio::io_service> g_io_service;
	void prepare_io_service()
	{
		g_io_service.reset(new boost::asio::io_service());
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
			endpoint ep;
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
	class async_transport : boost::noncopyable
	{
	public:
		typedef typename peer<DataType> peer_type;
		typedef typename peer_data<DataType> peer_data_type;
		typedef std::unordered_map<endpoint, std::shared_ptr<peer_type>> peer_map_type;
		typedef std::list<const peer_data_type > peer_list_type;
		typedef std::function<void(const error_code&)> error_handler_type;
		typedef std::function<void(const endpoint&, DataType&)> receive_handler_type;
		
		async_transport() : m_socket(*g_io_service), m_is_running(false)
		{
		}
		virtual ~async_transport()
		{
			stop();
		}
		//Not thread-safe. Avoid concurrent calls with other methods
		void start(unsigned short port, int thread_num = 3)
		{
			m_is_running = true;
			m_socket.open(boost::asio::ip::udp::v4());
			m_socket.bind(endpoint(boost::asio::ip::address_v4::any(), port));
			receive_loop();
			for(int i=0; i < thread_num; i++)
				m_thread_group.emplace_back([&] {g_io_service->run(); });
		}
		//Not thread-safe. Avoid concurrent calls with other methods
		void stop()
		{
			m_is_running = false;
			m_socket.close();
			g_io_service->stop();
			for (auto& thread : m_thread_group)
				if (thread.joinable())
					thread.join();
			m_thread_group.clear();
			g_io_service->reset();
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
		void clear_queue(const endpoint& ep)
		{
			if(m_is_running)
				clear_queue_impl(ep);
		}
		void queue(const endpoint& ep, const DataType& data)
		{
			if(m_is_running)
				queue_impl(ep, data);
		}
		int send(const endpoint& ep, int delay_ms, int loss_percentage)
		{
			if(m_is_running)
				return send_impl(ep, delay_ms, loss_percentage);
			return -1;
		}
		int send(const endpoint& ep)
		{
			if(m_is_running)
				return send_impl(ep);
			return -1;
		}
		int send_sync(const endpoint& ep)
		{
			if(m_is_running)
				return send_impl(ep, true);
			return -1;
		}
		inline const peer_list_type peers()
		{
			peer_list_type list;
			for (auto& kv : m_peers)
				list.push_back(kv.second->data);
			return list;
		}
		inline const peer_data_type& peer(const endpoint& ep)
		{
			return m_peers[ep]->data;
		}
		inline int port()
		{
			return m_socket.local_endpoint().port();
		}
	protected:
		inline int send_impl(const endpoint& ep, bool sync = false)
		{
			transaction_data<OperationType::Send,BufferSize>& t = m_send_buffer.next();
			t.ep = ep;
			oarchive oa(t.buffer.data(), t.buffer.data() + t.buffer.size());
			int send_n = find_peer(ep).serialize_datagram(oa);

			t.buffer_length = oa.pos();
			if(sync)
			{
				try
				{
					m_socket.send_to(boost::asio::buffer(t.buffer, t.buffer_length), t.ep);
				}
				catch(const boost::system::system_error& e)
				{
					if(m_err_handler)
						m_err_handler(e.code());
				}
			}
			else
			{
				m_socket.async_send_to(boost::asio::buffer(t.buffer, t.buffer_length), t.ep,
					[&](const boost::system::error_code &error, std::size_t bytes_transferred)
					{
						send_handler(t, bytes_transferred, error);
					}
				);
			}
			
			return send_n;
		}

		inline int send_impl(const endpoint& ep, int delay_ms, int loss_percentage)
		{
			std::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(*g_io_service));
			timer->expires_from_now(boost::posix_time::milliseconds(delay_ms));

			std::shared_ptr<transaction_data<OperationType::Send,BufferSize>> t(new transaction_data<OperationType::Send,BufferSize>());
			t->ep = ep;
			oarchive oa(t->buffer.data(), t->buffer.data() + t->buffer.size());
			int send_n = find_peer(ep).serialize_datagram(oa);

			if((rand() % 100)+1 <= loss_percentage)
				return send_n;

			t->buffer_length = oa.pos();
			timer->async_wait(
				[=](const boost::system::error_code& err) mutable
				{
					if (err != boost::asio::error::operation_aborted)
					{
						m_socket.async_send_to(boost::asio::buffer(t->buffer, t->buffer_length), t->ep,
							[=](const boost::system::error_code& e, size_t bytes_sent) mutable
							{
								send_handler(*t, bytes_sent, e);
							}
						);
					}
					else
					{
						throw std::exception("timer_aborted");
					}
					timer.reset();
				}
			);
			return send_n;
		}

		inline uint64_t queue_impl(const endpoint& ep, const DataType& data)
		{
			return find_peer(ep).queue_msg(data);
		}
		inline void clear_queue_impl(const endpoint& ep)
		{
			find_peer(ep).clear_queue();
		}
		
		inline peer_type& find_peer(const endpoint& ep)
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
			transaction_data<OperationType::Recv,BufferSize>& t = m_recv_buffer.next();
			m_socket.async_receive_from(boost::asio::buffer(t.buffer, BufferSize), t.ep,
				[&](const boost::system::error_code &error, std::size_t bytes_transferred)
				{
					receive_handler(t, bytes_transferred, error);
				}
			);
		}
		void receive_handler( transaction_data<OperationType::Recv,BufferSize>& t, size_t bytes_recvd, const boost::system::error_code& e)
		{
			if(m_is_running)
			{
				receive_loop();
				if (!e)
				{
					t.buffer_length = bytes_recvd;
					finalize(t);
				}
				else
				{
					if(m_err_handler)
						m_err_handler(e);
				}
			}
		}
		void send_handler(const transaction_data<OperationType::Send,BufferSize>& t, size_t bytes_sent, const boost::system::error_code& e)
		{
			if(m_is_running)
			{
				if(e)
				{
					if(m_err_handler)
						m_err_handler(e);
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
					m_err_handler(error_code());
			}
		}

		error_handler_type m_err_handler;
		receive_handler_type m_recv_handler;

		volatile bool m_is_running;

		boost::asio::ip::udp::socket m_socket;
		std::vector<std::thread> m_thread_group;

		peer_map_type m_peers;

		transaction_buffer<OperationType::Send,BufferSize, BufferQueueSize> m_send_buffer;
		transaction_buffer<OperationType::Recv,BufferSize, BufferQueueSize> m_recv_buffer;

		std::mutex m_mutex;
	};
}
