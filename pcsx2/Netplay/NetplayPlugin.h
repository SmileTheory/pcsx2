#pragma once 

#include "IopCommon.h"
#include "App.h"
#include "IOPHook.h"

class INetplayPlugin : public IOPHook
{
protected:
	static INetplayPlugin* instance;
public:
	static INetplayPlugin& GetInstance();

	virtual void Open() = 0;
	virtual void Init() = 0;
	virtual bool IsInit() = 0;
	virtual void EndSession() = 0;
	virtual void Close() = 0;
};
