#pragma once
#include <unordered_map>
#include <mutex>

#include <boost/asio/ip/udp.hpp>

namespace std
{
	template <>
	struct hash<boost::asio::ip::udp::endpoint>
	{
		size_t operator()(const boost::asio::ip::udp::endpoint &ep) const
		{
			string s = ep.address().to_string();
			s += ":" + to_string(ep.port());
			return hash<string>{}(s);
		}
	};

	class semaphore : boost::noncopyable
	{
	public:
		semaphore() : count_(0) {}

		explicit semaphore(unsigned int initial_count) 
			: count_(initial_count), 
			mutex_(), 
			condition_()
		{
		}

		void post()
		{
			std::unique_lock<std::mutex> lock(mutex_);
			++count_;
			condition_.notify_one(); 
		}

		void wait()
		{
			std::unique_lock<std::mutex> lock(mutex_);
			condition_.wait(lock, [&]() { return count_ > 0; });
			--count_;
		}
		bool timed_wait(int ms)
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if(!condition_.wait_for(lock,std::chrono::milliseconds(ms), [&]() { return count_ > 0; }))
				return false;
			--count_;
			return true;
		}
		void clear()
		{
			std::unique_lock<std::mutex> lock(mutex_);
			count_ = 0;
		}
	protected:
		unsigned int count_;
		std::mutex mutex_;
		std::condition_variable condition_;
	};
}
