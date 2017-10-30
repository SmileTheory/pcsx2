#pragma once
#include "App.h"
#include "EmulatorState.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include "PS2Edefs.h"

class Utilities
{
public:
	typedef std::vector<u8> block_type;

	static block_type ReadMCD(uint port, uint slot);
	static void WriteMCD(uint port, uint slot, const block_type& block);
	static bool Compress(const Utilities::block_type& uncompressed,
		Utilities::block_type& compressed);
	static bool Uncompress(const Utilities::block_type& compressed,
		Utilities::block_type& uncompressed);
	static size_t GetMCDSize(uint port, uint slot);
	static bool IsSyncStateReady();
	static boost::shared_ptr<EmulatorSyncState> GetSyncState();
	static wxString GetDiscNameById(const wxString& id);
	static wxString GetCurrentDiscId();
	static wxString GetCurrentDiscName();

	static void ExecuteOnMainThread(const std::function<void()>& evt);

	static void SaveSettings();
	static void ResetSettingsToSafeDefaults();
	static void RestoreSettings();
private:
	static boost::recursive_mutex _mutex;
	static std::function<void()> _dispatch_event;
	static void DispatchEvent();
	static std::auto_ptr<AppConfig> _settingsBackup;
};
