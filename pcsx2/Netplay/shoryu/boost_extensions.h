#pragma once
#include <unordered_map>
#include <mutex>
#include "zed_net.h"


inline bool operator==(const zed_net_address_t &ep1, const zed_net_address_t &ep2)
{
	return (ep1.host == ep2.host) && (ep1.port == ep2.port);
}

inline bool operator!=(const zed_net_address_t &ep1, const zed_net_address_t &ep2)
{
	return (ep1.host != ep2.host) || (ep1.port != ep2.port);
}

inline bool operator<(const zed_net_address_t &ep1, const zed_net_address_t &ep2)
{
	if (ep1.host == ep2.host)
		return ep1.port < ep2.port;
	return ep1.host < ep2.host;
}

namespace std
{
	template <>
	struct hash<zed_net_address_t>
	{
		size_t operator()(const zed_net_address_t &ep) const
		{
			string s(zed_net_host_to_str(ep.host));
			s += ":" + to_string(ep.port);
			return hash<string>{}(s);
		}
	};

	class noncopyable {
		public:
			noncopyable() = default;
			~noncopyable() = default;
		private:
			noncopyable(const noncopyable&) = delete;
			noncopyable& operator=(const noncopyable&) = delete;
	};
}
