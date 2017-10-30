#pragma once

#include "App.h"
#include <cstdint>
#include "shoryu\archive.h"
struct EmulatorSyncState
{
	EmulatorSyncState();
	char biosVersion[35];
	char discId[15];
	bool skipMpeg;
	void serialize(shoryu::oarchive& a) const;
	void deserialize(shoryu::iarchive& a);
};
