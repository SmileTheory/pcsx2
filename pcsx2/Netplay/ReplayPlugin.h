#pragma once
#include "IOPHook.h"

class IReplayPlugin : public IOPHook
{
protected:
	static IReplayPlugin* instance;
public:
	static IReplayPlugin& GetInstance();

	virtual void Open() = 0;
	virtual void Init() = 0;
	virtual bool IsInit() = 0;
	virtual void Close() = 0;
};
