#pragma once
#include <cstdint>
#include <bitset>



namespace shoryu
{
	//protocol id should be defined at session-level
	#define PROTOCOL_ID "PCS2OV4"
	const size_t PROTOCOL_ID_LEN = sizeof(PROTOCOL_ID)/sizeof(char);

	struct datagram_header
	{
		int64_t time;
		int64_t rtt;

		inline void set_defaults()
		{
			ack_first_id = 0;
			ack_mask.reset();
			rtt = 0;
			time = time_ms();
		}
		inline bool is_acknoledged(uint64_t id) const
		{
			if(id < ack_first_id)
				return false;
			uint64_t offset = id - ack_first_id;
			if(offset >= ack_mask.size())
				return false;
			return ack_mask[offset];
		}
		inline void acknoledge(int64_t id)
		{
			if(id <= 0)
				throw std::exception("invalid id");
			if(!ack_first_id)
			{
				ack_first_id = id;
				ack_mask.set(0);
			}
			else
			{
				int64_t offset = id - ack_first_id;
				if(offset >= 32)
					throw std::exception("offset cannot exceed 32");
				ack_mask.set(offset);
			}
		}
		inline void serialize(oarchive& a) const
		{
			a.write(PROTOCOL_ID, PROTOCOL_ID_LEN);
			a << time << rtt << ack_first_id << ack_mask.to_ullong();
		}
		inline void deserialize(iarchive& a)
		{
			char pid[PROTOCOL_ID_LEN];
			a.read(pid, PROTOCOL_ID_LEN);
			uint64_t ack_mask_ull;
			if(std::memcmp(PROTOCOL_ID, pid, PROTOCOL_ID_LEN) == 0)
			{
				a >> time >> rtt >> ack_first_id >> ack_mask_ull;
				ack_mask = std::bitset<64>(ack_mask_ull);
			}
			else
				throw std::exception("invalid protocol id");
		}
	protected:
		uint64_t ack_first_id;
		std::bitset<64> ack_mask;
	};
}
