#pragma once
#include <cstdint>
#include <string>
#include <boost/asio/ip/udp.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/system/system_error.hpp>


namespace shoryu
{

#define repeat(N) for(decltype (N) i = 0; i < N; i++)
#define MS_IN_SEC 1000;

	typedef boost::gregorian::date date;
	typedef boost::asio::ip::udp::endpoint endpoint;
	typedef int64_t msec;
	typedef boost::asio::ip::udp::endpoint endpoint;
	typedef boost::asio::ip::address address;
	typedef boost::system::error_code error_code;



	inline void sleep( uint32_t msec ) 
	{
		boost::xtime xt; 
		boost::xtime_get(&xt, boost::TIME_UTC_);
		msec += xt.nsec / 1000000;
		uint32_t sec = msec / MS_IN_SEC;
		msec %= MS_IN_SEC;
		xt.nsec = msec*1000000;
		xt.sec += sec;
		boost::thread::sleep(xt); 
	}

	inline msec time_ms(const date& origin = date(1970,1,1)) {
		using namespace boost::posix_time;
		return (microsec_clock::universal_time() - ptime(origin)).total_milliseconds();
	}
		
	//TODO
	inline int get_random_free_port()
	{
		return -1;
	}

	inline endpoint resolve_hostname(const std::string& str)
	{
		using namespace boost::asio::ip;
		boost::asio::io_service io_service;
		boost::system::error_code e;
		endpoint ep(address::from_string(str, e), 0);
		//error means hostname was passed, not IP address
		if(e)
		{
			udp::resolver resolver(io_service);
			udp::resolver::iterator iter = resolver.resolve(udp::resolver::query(str, ""), e);
			if(!e) 
			{
				udp::resolver::iterator end;
				//get first DNS record, it's ok
				if (iter != end)
					return *iter;
			}
			throw boost::system::system_error(e);
		}
		return ep;
	}
}
