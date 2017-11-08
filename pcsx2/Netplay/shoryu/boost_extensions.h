#pragma once
#include <unordered_map>

#include <boost/asio/ip/udp.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>    
#include <boost/thread/locks.hpp>

namespace std
{
	template <>
	struct hash<boost::asio::ip::udp::endpoint>
	{
		std::size_t operator()(const boost::asio::ip::udp::endpoint &ep) const
		{
			std::string s;
			s = (int)ep.port();
			s += ":";
			if(ep.address().is_v4()) {
				s += ep.address().to_v4().to_string();
			}
			else if(ep.address().is_v6()) {
				s += ep.address().to_v6().to_string();
			}
			return std::hash<std::string>{}(s);
		}
	};
}


namespace boost
{
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
			boost::unique_lock<boost::mutex> lock(mutex_);
			++count_;
			condition_.notify_one(); 
		}

		void wait()
		{
			boost::unique_lock<boost::mutex> lock(mutex_);
			condition_.wait(lock, [&]() { return count_ > 0; });
			--count_;
		}
		bool timed_wait(int ms)
		{
			boost::unique_lock<boost::mutex> lock(mutex_);
			if(!condition_.timed_wait(lock,boost::posix_time::millisec(ms), [&]() { return count_ > 0; }))
				return false;
			--count_;
			return true;
		}
		void clear()
		{
			boost::unique_lock<boost::mutex> lock(mutex_);
			count_ = 0;
		}
	protected:
		unsigned int count_;
		boost::mutex mutex_;
		boost::condition_variable condition_;
	};
}
