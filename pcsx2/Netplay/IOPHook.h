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

u8 CALLBACK NETPADstartPoll(int port);
u8 CALLBACK NETPADpoll(u8 value);
s32 CALLBACK NETPADsetSlot(u8 port, u8 slot);

void HookIOP(IOPHook* hook);
void UnhookIOP();