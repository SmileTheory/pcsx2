#pragma once
#include <cstdint>
#include <string>
#include <chrono>
#include <thread>
#include <boost/asio/ip/udp.hpp>
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
		std::this_thread::sleep_for(std::chrono::milliseconds(msec));
	}

	inline msec time_ms() {
		//return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		return GetCPUTicks() / (GetTickFrequency() / 1000);
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
