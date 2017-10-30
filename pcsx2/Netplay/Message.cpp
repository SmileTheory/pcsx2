#include "PrecompiledHeader.h"
#include "Message.h"

const char defaultInput[] = {0xff, 0xff};
Message::Message()
{
	std::copy(defaultInput, defaultInput + sizeof(defaultInput), input);
}
void Message::serialize(shoryu::oarchive& a) const
{
	a.write((char*)input, sizeof(input));
}
void Message::deserialize(shoryu::iarchive& a)
{
	a.read(input, sizeof(input));
}
