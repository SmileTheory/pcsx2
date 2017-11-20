#pragma once
#include "App.h"
#include "IopCommon.h"

class IOPHook
{
public:
	virtual u8 HandleIO(int side, int index, u8 value) = 0;
	virtual void NextFrame() = 0;
	virtual void AcceptInput(int side) = 0;
	virtual int RemapVibrate(int pad) = 0;
};

void HookIOP(IOPHook* hook);
void UnhookIOP();