#include "PrecompiledHeader.h"
#include "IOPHook.h"
#include <fstream>
#include <iostream>
#include <iomanip>


IOPHook* g_IOPHook = 0;
//#define LOG_IOP

#ifdef NETPLAY_ANALOG_STICKS
#define NETPLAY_SYNC_NUM_INPUTS 6
#else
#define NETPLAY_SYNC_NUM_INPUTS 2
#endif

namespace
{
	_PADupdate		   PADupdateBackup;
	_PADopen           PADopenBackup;
	_PADstartPoll      PADstartPollBackup;
	_PADpoll           PADpollBackup;
	_PADquery          PADqueryBackup;
	_PADkeyEvent       PADkeyEventBackup;
	_PADsetSlot        PADsetSlotBackup;
	_PADqueryMtap      PADqueryMtapBackup;
	
	int g_currentCommand = -1;
	int g_pollSide = -1;
	int g_pollIndex = -1;
	bool g_active = false;
#ifdef LOG_IOP
	std::fstream g_log;
#endif

	s32 CALLBACK NETPADopen(void *pDsp)
	{
		return PADopenBackup(pDsp);
	}
	u8 CALLBACK NETPADstartPoll(int pad)
	{
		g_pollSide = pad - 1;
		g_pollIndex = 0;

		if(g_pollSide == 0)
		{
			if(g_IOPHook && g_currentCommand == 0x42)
				g_IOPHook->NextFrame();
		}
#ifdef LOG_IOP
		using namespace std;
		g_log << endl << setw(2) << (int)pad << '-' << setw(2) << "!" << ": ";
#endif
		return PADstartPollBackup(pad);
	}
	u32 CALLBACK NETPADquery(int pad)
	{
		return PADqueryBackup(pad);
	}
	keyEvent* CALLBACK NETPADkeyEvent()
	{
		return PADkeyEventBackup();
	}
	s32 CALLBACK NETPADqueryMtap(u8 port)
	{
		return PADqueryMtapBackup(port);
	}
	void CALLBACK NETPADupdate(int pad)
	{
		return PADupdateBackup(pad);
	}

	u8 CALLBACK NETPADpoll(u8 value)
	{
		if (g_pollIndex == 0)
			g_currentCommand = value;

#ifdef LOG_IOP
		using namespace std;
		g_log << hex << setw(2) << (int)value << '=';
#endif
		value = PADpollBackup(value);

		if (g_currentCommand == 0x42)
		{
			if (g_pollIndex < 2)
			{
				// nothing
			}
			else if (g_pollIndex <= 1 + NETPLAY_SYNC_NUM_INPUTS)
			{
				if (g_IOPHook)
				{
					value = g_IOPHook->HandleIO(g_pollSide, g_pollIndex - 2, value);

					if (g_pollIndex == 1 + NETPLAY_SYNC_NUM_INPUTS)
						g_IOPHook->AcceptInput(g_pollSide);
				}
			}
			else if (g_pollIndex < 8)
			{
				value = 0x7f;
			}
			else
			{
				value = 0xff;
			}
		}
#ifdef LOG_IOP
		g_log << hex << setw(2) << (int)value << ' ';
#endif
		g_pollIndex++;
		return value;
	}
	s32 CALLBACK NETPADsetSlot(u8 port, u8 slot)
	{
		g_pollSide = port - 1;
		g_pollIndex = 0;

		if(g_pollSide == 0)
		{
			if(g_IOPHook && g_currentCommand == 0x42)
				g_IOPHook->NextFrame();
		}
#ifdef LOG_IOP
		using namespace std;
		g_log << endl << setw(2) << (int)port << '-' << setw(2) << (int)slot << ": ";
#endif

		return PADsetSlotBackup(port, slot);
	}
}

void HookIOP(IOPHook* hook)
{
	g_IOPHook = hook;
	g_currentCommand = 0;
	g_pollSide = 0;
	g_pollIndex = 0;

	if(g_active)
		return;
	g_active = true;
#ifdef LOG_IOP
	g_log.open("iop.log", std::ios_base::trunc | std::ios_base::out);
	g_log.fill('0');
#endif


	PADopenBackup = PADopen;
	PADstartPollBackup = PADstartPoll;
	PADpollBackup = PADpoll;
	PADqueryBackup = PADquery;
	PADkeyEventBackup = PADkeyEvent;
	PADsetSlotBackup = PADsetSlot;
	PADqueryMtapBackup = PADqueryMtap;
	PADupdateBackup = PADupdate;
		
	PADopen = NETPADopen;
	PADstartPoll = NETPADstartPoll;
	PADpoll = NETPADpoll;
	PADquery = NETPADquery;
	PADkeyEvent = NETPADkeyEvent;
	PADsetSlot = NETPADsetSlot;
	PADqueryMtap = NETPADqueryMtap;
	PADupdate = NETPADupdate;
}

void UnhookIOP()
{
	g_IOPHook = 0;
#ifdef LOG_IOP
	g_log.close();
#endif
	PADopen = PADopenBackup;
	PADstartPoll = PADstartPollBackup;
	PADpoll = PADpollBackup;
	PADquery = PADqueryBackup;
	PADkeyEvent = PADkeyEventBackup;
	PADsetSlot = PADsetSlotBackup;
	PADqueryMtap = PADqueryMtapBackup;
	PADupdate = PADupdateBackup;
	g_active = false;
}