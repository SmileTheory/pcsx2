#pragma once

#include <list>
#include <deque>
#include <memory>
#include <mutex>
#include <algorithm>

#include "datagram_header.h"
#include "zed_net.h"

namespace shoryu
{
	template<typename MsgType>
	struct peer_data
	{
		peer_data()
			: rtt_avg(-1),rtt_max(-1),rtt_min(-1),remote_time(0),recv_time(0)
		{
		}
		zed_net_address_t ep;
		uint64_t remote_time;
		uint64_t recv_time;

		int32_t rtt_avg;
		int32_t rtt_max;
		int32_t rtt_min;
	};

	template<typename MsgType>
	class peer : std::noncopyable
	{
	private:
		struct msg_wrapper
		{
			int64_t id;
			MsgType data;
			inline void serialize(oarchive& a) const
			{
				a << id;
				data.serialize(a);
			}
			inline void deserialize(iarchive& a)
			{
				a >> id;
				data.deserialize(a);
			}
		};
		typedef std::list<msg_wrapper> container_type;
	public:
		peer() : received_msgs(),next_id(1){}
		peer_data<MsgType> data;

		inline uint64_t queue_msg(const MsgType& msg)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			msg_wrapper m;
			m.id = next_id;
			++next_id;
			m.data = msg;
			msg_queue.push_back(m);
			return m.id;
		}
		inline void clear_queue()
		{
			std::unique_lock<std::mutex> lock(_mutex);
			msg_queue.clear();
		}

		template<typename Pred>
		inline void deserialize_datagram(iarchive& ia, Pred& yield)
		{
			std::list<MsgType> data_list;
			{
				std::unique_lock<std::mutex> lock(_mutex);
				data.recv_time = time_ms();
				datagram_header header;
				header.deserialize(ia);
				data.remote_time = header.time;
				msg_queue.remove_if([&](const msg_wrapper& msg) { return header.is_acknoledged(msg.id);});
				
				if(header.rtt != 0)
					estimate_rtt( (int32_t)(time_ms() - header.rtt));

				while(true)
				{
					msg_wrapper msg;
					try
					{
						msg.deserialize(ia);
					}
					catch(std::ios_base::failure&){ break; }

					// TODO: Optimize here
					if(std::find(received_msgs.begin(), received_msgs.end(), msg.id) == received_msgs.end())
					{
						if (received_msgs.size() == 32)
							received_msgs.pop_front();
						received_msgs.push_back(msg.id);
						data_list.push_back(msg.data);
					}
				}
			}
			for (auto& data : data_list)
				yield(data);
		}

		inline int serialize_datagram(oarchive& oa)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			datagram_header header;
			header.set_defaults();
			if(received_msgs.begin() != received_msgs.end())
			{
				std::sort(received_msgs.begin(), received_msgs.end());
				received_msgs_type::value_type min = *received_msgs.begin();
				for(auto it = received_msgs.begin(); it != received_msgs.end(); ++it)
				{
					if(*it - min> 31)
						break;
					header.acknoledge(*it);
				}
			}
			if(data.remote_time != 0)
				header.rtt = data.remote_time + (time_ms() - data.recv_time);

			header.serialize(oa);
			container_type::size_type size = msg_queue.size();
			container_type::size_type i = 0;

			std::vector<msg_wrapper*> msg_shuffle;
			msg_shuffle.reserve(size);
			for (auto& msg : msg_queue)
				msg_shuffle.push_back(&msg);
			
			std::random_shuffle(msg_shuffle.begin(), msg_shuffle.end());

			for (auto& msg : msg_shuffle)
			{
				try
				{
					msg->serialize(oa);
				}
				catch(std::ios_base::failure&) { break; }
				++i;
			}
			if(size && !i)
				throw std::runtime_error("Unable to serialize message: packet is too long.");

			return size;
		}
	private:
		container_type msg_queue;
		typedef std::deque<uint64_t> received_msgs_type;
		received_msgs_type received_msgs;
		uint64_t next_id;
		inline void estimate_rtt(int32_t rtt)
		{
			if(data.rtt_min > rtt || data.rtt_min < 0)
				data.rtt_min = rtt;
			if(data.rtt_max < rtt || data.rtt_max < 0)
				data.rtt_max = rtt;

			if(data.rtt_avg < 0)
				data.rtt_avg = rtt;
			else
				data.rtt_avg = (3*rtt + 7*data.rtt_avg)/10;
		}
		std::mutex _mutex;
	};
}
