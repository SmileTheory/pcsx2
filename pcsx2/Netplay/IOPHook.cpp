#include "PrecompiledHeader.h"
#include "IOPHook.h"
#include <fstream>
#include <iostream>
#include <iomanip>


IOPHook* g_IOPHook = 0;
//#define LOG_IOP

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
	
	int g_cmd42Counter = -1;
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
			if(g_IOPHook && g_cmd42Counter > 2)
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
		if(g_pollIndex == 0 && g_pollSide == 0)
		{
			if( value == 0x42 )
			{
				if(g_cmd42Counter < 10) g_cmd42Counter++;
			}
			else
				g_cmd42Counter = 0;
		}

#ifdef LOG_IOP
		using namespace std;
		g_log << hex << setw(2) << (int)value << '=';
#endif
		value = PADpollBackup(value);
#ifdef LOG_IOP
		//g_log << hex << setw(2) << (int)value << ' ';
#endif

		if(g_cmd42Counter > 2 && g_pollIndex > 1)
		{
			if(g_pollIndex <= 3)
			{
				if(g_IOPHook)
					value = g_IOPHook->HandleIO(g_pollSide, g_pollIndex-2,value);

				if(g_IOPHook && g_pollIndex == 3)
					g_IOPHook->AcceptInput(g_pollSide);
			}
			else
				value = g_pollIndex <= 7 ? 0x7f : 0xff;
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
			if(g_IOPHook && g_cmd42Counter > 2)
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
	g_cmd42Counter = 0;
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