#include "PrecompiledHeader.h"
#include "Message.h"

#ifdef NETPLAY_ANALOG_STICKS
#define NETPLAY_SYNC_NUM_INPUTS 6
#else
#define NETPLAY_SYNC_NUM_INPUTS 2
#endif

const char defaultInput[] = {0xff, 0xff, 0x7f, 0x7f, 0x7f, 0x7f};
Message::Message()
{
	std::copy(defaultInput, defaultInput + sizeof(defaultInput), input);
}
void Message::serialize(shoryu::oarchive& a) const
{
	a.write((char*)input, NETPLAY_SYNC_NUM_INPUTS);
}
void Message::deserialize(shoryu::iarchive& a)
{
	a.read(input, NETPLAY_SYNC_NUM_INPUTS);
}
