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
	NetplayMode Mode;
	uint HostPort;
	wxString HostAddress;
	uint ListenPort;
	bool SaveReplay;
	bool ReadonlyMemcard;
	uint NumPlayers;
	
	NetplaySettings();
	void LoadSave( IniInterface& conf );
	void SanityCheck();
};