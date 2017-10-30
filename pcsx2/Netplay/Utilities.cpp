#include "PrecompiledHeader.h"
#include "Utilities.h"
#include "zlib\zlib.h"
#include "ps2/BiosTools.h"
#include "Elfheader.h"
#include "CDVD/CDVD.h"
#include "AppGameDatabase.h"

void Utilities::SaveSettings()
{
	_settingsBackup.reset(new AppConfig(*g_Conf));
}
void Utilities::ResetSettingsToSafeDefaults()
{
	g_Conf->EmuOptions.HostFs = false;
	g_Conf->EmuOptions.McdEnableEjection = false;
	g_Conf->EmuOptions.MultitapPort0_Enabled = false;
	g_Conf->EmuOptions.MultitapPort1_Enabled = false;
	g_Conf->EmuOptions.UseBOOT2Injection = true;
	g_Conf->EmuOptions.EnablePatches = false;
	g_Conf->EmuOptions.Speedhacks.bitset = 0;
	g_Conf->EmuOptions.Speedhacks.DisableAll();
	g_Conf->EmuOptions.Cpu = Pcsx2Config::CpuOptions();
	g_Conf->EmuOptions.GS = Pcsx2Config::GSOptions();
	bool mpegHackBit = g_Conf->EmuOptions.Gamefixes.SkipMPEGHack;
	g_Conf->EmuOptions.Gamefixes.bitset = 0;
	g_Conf->EmuOptions.Gamefixes.SkipMPEGHack = mpegHackBit;
	g_Conf->EmuOptions.EnableCheats = false;
	g_Conf->EmuOptions.CdvdDumpBlocks = false;
	g_Conf->EmuOptions.CdvdVerboseReads = false;
	g_Conf->EmuOptions.Profiler.bitset = 0;
	g_Conf->EmuOptions.Trace.Enabled = false;
	
	g_Conf->Mcd[0].Enabled = true;
	g_Conf->Mcd[1].Enabled = false;
	g_Conf->EnableGameFixes = true;
}
void Utilities::RestoreSettings()
{
	if(!_settingsBackup.get())
		return;
	g_Conf->EmuOptions = _settingsBackup->EmuOptions;
	g_Conf->Mcd[0].Enabled = _settingsBackup->Mcd[0].Enabled;
	g_Conf->Mcd[1].Enabled = _settingsBackup->Mcd[1].Enabled;
	g_Conf->EnableGameFixes = _settingsBackup->EnableGameFixes;
	_settingsBackup.reset();
}

size_t Utilities::GetMCDSize(uint port, uint slot)
{
	PS2E_McdSizeInfo info;
	SysPlugins.McdGetSizeInfo(port,slot,info);
	return info.McdSizeInSectors*(info.SectorSize + info.EraseBlockSizeInSectors);
}

Utilities::block_type Utilities::ReadMCD(uint port, uint slot)
{
	block_type mcd;
	size_t size = GetMCDSize(port, slot);
	mcd.resize(size);
	SysPlugins.McdRead(0,0, mcd.data(), 0, size);
	return mcd;
}
void Utilities::WriteMCD(uint port, uint slot, const Utilities::block_type& block)
{
	size_t size = GetMCDSize(port, slot);
	if(block.size() != size)
		throw std::exception("invalid block size");
	for(size_t p = 0; p < size; p+= 528*16)
		SysPlugins.McdEraseBlock(0,0,p);

	SysPlugins.McdSave(port,slot, block.data(), 0, size);
}
bool Utilities::Compress(const Utilities::block_type& uncompressed,
	Utilities::block_type& compressed)
{
	uLongf size = uncompressed.size();
	compressed.resize(size);
	int r = compress2(compressed.data(), &size, uncompressed.data(), size, Z_BEST_COMPRESSION);
	if(r != Z_OK)
		return false;
	if(size < compressed.size())
		compressed.resize(size);
	return true;
}
bool Utilities::Uncompress(const Utilities::block_type& compressed,
	Utilities::block_type& uncompressed)
{
	uLongf size = uncompressed.size();
	int r = uncompress((Bytef*)uncompressed.data(), &size, (Bytef*)compressed.data(), compressed.size());
	if(r != Z_OK)
		return false;
	if(size < uncompressed.size())
		uncompressed.resize(size);
	return true;
}

void Utilities::DispatchEvent()
{
	if(_dispatch_event)
		_dispatch_event();
}
void Utilities::ExecuteOnMainThread(const std::function<void()>& evt)
{
	if(!evt)
		return;
	if(!wxThread::IsMain())
	{
		boost::lock_guard<boost::recursive_mutex> lock(_mutex);
		_dispatch_event = evt;
		wxGetApp().Rpc_TryInvoke( DispatchEvent );
		_dispatch_event = std::function<void()>();
	}
	else
		evt();
}

wxString Utilities::GetCurrentDiscId()
{
	cdvdReloadElfInfo();
	auto diskId = SysGetDiscID();
	if(diskId.Len() == 0)
	{
		ExecuteOnMainThread([&]() {
			Console.Error("Unable to get disc id.");
		});
	}
	return diskId;
}

bool Utilities::IsSyncStateReady()
{
	cdvdReloadElfInfo();
	return !DiscSerial.IsEmpty();
}

boost::shared_ptr<EmulatorSyncState> Utilities::GetSyncState()
{
	boost::shared_ptr<EmulatorSyncState> syncState(new EmulatorSyncState());

	auto diskId = GetCurrentDiscId();
	if(diskId.Len() == 0)
	{
		syncState.reset();
		return syncState;
	}

	memset(syncState->discId, 0, sizeof(syncState->discId));
	memcpy(syncState->discId, diskId.ToAscii().data(), 
		diskId.length() > sizeof(syncState->discId) ? 
		sizeof(syncState->discId) : diskId.length());

	wxString biosDesc;
	if(!IsBIOS(g_Conf->EmuOptions.BiosFilename.GetFullPath(), biosDesc) || biosDesc.Len() == 0)
	{
		ExecuteOnMainThread([&]() {
			Console.Error("Unable to read BIOS version.");
		});
		syncState.reset();
		return syncState;
	}
	memset(syncState->biosVersion, 0, sizeof(syncState->biosVersion));
	memcpy(syncState->biosVersion, biosDesc.ToAscii().data(), 
		biosDesc.length() > sizeof(syncState->biosVersion) ? 
		sizeof(syncState->biosVersion) : biosDesc.length());

	syncState->skipMpeg = g_Conf->EmuOptions.Gamefixes.SkipMPEGHack;

	return syncState;
}

wxString Utilities::GetDiscNameById(const wxString& id)
{
	wxString discName;
	Game_Data game;
	IGameDatabase* GameDB = AppHost_GetGameDatabase();
	if(GameDB && GameDB->findGame(game, id))
		discName = game.getString("Name") + wxT(" (") + game.getString("Region") + wxT(")");
	else
		discName = id;
	return discName;
}
wxString Utilities::GetCurrentDiscName()
{
	return GetDiscNameById(GetCurrentDiscId());
}

std::function<void()> Utilities::_dispatch_event;
std::auto_ptr<AppConfig> Utilities::_settingsBackup;
boost::recursive_mutex Utilities::_mutex;