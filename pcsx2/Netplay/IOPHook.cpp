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
	int g_pollPort = -1;
	int g_pollSlot[2] = {0, 0};
	int g_pollIndex = -1;
	int g_hookFrameNum = -1;
	bool g_active = false;
#ifdef LOG_IOP
	std::fstream g_log;
#endif

	// port 0 slot 0   -> pad 0
	// port 1 slot 0-3 -> pad 1-4
	// port 0 slot 1-3 -> pad 5-7

	int NET_CurrentPad()
	{
		int slot = g_pollSlot[g_pollPort];

		if (slot)
			return slot + ((g_pollPort == 0) ? 4 : 1);

		return g_pollPort;
	}

	s32 CALLBACK NETPADopen(void *pDsp)
	{
		return PADopenBackup(pDsp);
	}
	u8 CALLBACK NETPADstartPoll(int port)
	{
		g_pollPort = port - 1;
		g_pollIndex = 0;

		if(g_pollPort == 0)
		{
			if(g_IOPHook && g_currentCommand == 0x42)
			{
				g_IOPHook->NextFrame();
				g_hookFrameNum++;
			}
		}
#ifdef LOG_IOP
		using namespace std;
		g_log << endl << setw(8) << (int)g_hookFrameNum << ": ";
		g_log << setw(2) << (int)port << '-' << setw(2) << (int)g_pollSlot[g_pollPort];
		g_log << " (" << setw(2) << NET_CurrentPad() << ") : ";
#endif
		return PADstartPollBackup(port);
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
			int pad = NET_CurrentPad();

			if (g_pollIndex < 2)
			{
				// nothing
			}
			// ignore pads > number of players
			else if (pad < g_Conf->Net.NumPlayers && g_pollIndex <= 1 + NETPLAY_SYNC_NUM_INPUTS)
			{
				if (g_IOPHook)
				{
					value = g_IOPHook->HandleIO(pad, g_pollIndex - 2, value);

					if (g_pollIndex == 1 + NETPLAY_SYNC_NUM_INPUTS)
						g_IOPHook->AcceptInput(pad);
				}
			}
			else if (g_pollIndex > 3 && g_pollIndex < 8)
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
		g_pollPort = port - 1;
		g_pollSlot[g_pollPort] = slot - 1;

		return PADsetSlotBackup(port, slot);
	}
}

void HookIOP(IOPHook* hook)
{
	g_IOPHook = hook;
	g_currentCommand = 0;
	g_pollPort = 0;
	g_pollIndex = 0;
	g_hookFrameNum = 0;

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