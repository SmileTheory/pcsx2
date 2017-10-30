#pragma once
#include <boost/array.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/unordered_map.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>    
#include <boost/thread/locks.hpp>

namespace boost
{
#if 0
	template <typename T, std::size_t N>
	inline std::size_t hash_value(const boost::array<T,N>& arr) 
	{ 
		return boost::hash_range(arr.begin(), arr.end()); 
	}
#endif

#if 0
	// Couldn't compile on boost 1.47 without this
	template <typename T, std::size_t N>
	inline std::size_t hash_value(const boost::asio::detail::array<T,N>& arr) 
	{ 
		return boost::hash_range(arr.begin(), arr.end()); 
	}
#endif

	inline std::size_t hash_value(const boost::asio::ip::udp::endpoint & ep)
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, ep.port());
		if(ep.address().is_v4()) {
			boost::hash_combine(seed, ep.address().to_v4().to_bytes());
		}
		else if(ep.address().is_v6()) {
			boost::hash_combine(seed, ep.address().to_v6().to_bytes());
		}
		return seed;
	}

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
