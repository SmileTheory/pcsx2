#include "PrecompiledHeader.h"
#include "NetplaySettings.h"

NetplaySettings::NetplaySettings()
{
	IsEnabled = false;
	HostPort = 7500;
	Mode = ConnectMode;
	ReadonlyMemcard = false;
	SaveReplay = false;
	NumPlayers = 2;
}

void NetplaySettings::LoadSave( IniInterface& ini )
{
	NetplaySettings defaults;
	ScopedIniGroup path( ini, L"Net" );

	IniEntry( Username );
	IniEntry( HostPort );
	IniEntry( HostAddress );
	IniEntry( ReadonlyMemcard );
	IniEntry( SaveReplay );
	IniEntry( NumPlayers );

	int mode = Mode;
	ini.Entry(wxT("Mode"), mode, mode);
	Mode = (NetplayMode)mode;

	if( ini.IsLoading() ) SanityCheck();
}
void NetplaySettings::SanityCheck()
{
	if(HostPort > 65535 || HostPort < 1)
		HostPort = 7500;
	if(NumPlayers < 2)
		NumPlayers = 2;
	if(NumPlayers > 8)
		NumPlayers = 8;
}
