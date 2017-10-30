#pragma once
#include "Utilities\IniInterface.h"

enum NetplayMode : int
{
	ConnectMode,
	HostMode,
	ObserveMode
};

struct NetplaySettings
{
	bool IsEnabled;

	wxString Username;
	uint LocalPort;
	NetplayMode Mode;
	uint HostPort;
	wxString HostAddress;
	bool SaveReplay;
	bool ReadonlyMemcard;
	
	NetplaySettings();
	void LoadSave( IniInterface& conf );
	void SanityCheck();
};