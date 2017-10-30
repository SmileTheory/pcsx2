#include "PrecompiledHeader.h"
#include "EmulatorState.h"

EmulatorSyncState::EmulatorSyncState()
{
	memset(biosVersion, 0, sizeof(biosVersion));
	memset(discId, 0, sizeof(discId));
}

void EmulatorSyncState::serialize(shoryu::oarchive& a) const
{
	a.write((char*)discId, sizeof(discId));
	a.write((char*)biosVersion, sizeof(biosVersion));
	a.write((char*)&skipMpeg, sizeof(skipMpeg));
}
void EmulatorSyncState::deserialize(shoryu::iarchive& a)
{
	a.read((char*)discId, sizeof(discId));
	a.read((char*)biosVersion, sizeof(biosVersion));
	a.read((char*)&skipMpeg, sizeof(skipMpeg));
}
