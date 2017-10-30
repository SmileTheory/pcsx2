#include "PrecompiledHeader.h"
#include "NetplaySettings.h"

NetplaySettings::NetplaySettings()
{
	IsEnabled = false;
	LocalPort = HostPort = 7500;
	Mode = ConnectMode;
	ReadonlyMemcard = false;
	SaveReplay = false;
}

void NetplaySettings::LoadSave( IniInterface& ini )
{
	NetplaySettings defaults;
	ScopedIniGroup path( ini, L"Net" );

	IniEntry( Username );
	IniEntry( LocalPort );
	IniEntry( HostPort );
	IniEntry( HostAddress );
	IniEntry( ReadonlyMemcard );
	IniEntry( SaveReplay );

	int mode = Mode;
	ini.Entry(wxT("Mode"), mode, mode);
	Mode = (NetplayMode)mode;

	if( ini.IsLoading() ) SanityCheck();
}
void NetplaySettings::SanityCheck()
{
	if(LocalPort > 65535 || LocalPort < 1)
		LocalPort = 7500;
	if(HostPort > 65535 || HostPort < 1)
		HostPort = 7500;
}
