#pragma once
#include "App.h"
#include "EmulatorState.h"
#include "Utilities.h"
#include "Message.h"

enum ReplayMode
{
	None,
	Recording,
	Playback
};

// TODO: use boost::serialization here
class Replay
{
public:
	typedef Utilities::block_type block_type;
	typedef std::vector<Message> input_type;
	typedef std::deque<input_type> input_container;

	Replay();

	bool LoadFromFile(const wxString& path, bool compressed = true);
	const Replay& SaveToFile(const wxString& path, bool compress = true) const;
	ReplayMode Mode() const;
	Replay& Mode(ReplayMode mode);
	const EmulatorSyncState& SyncState() const;
	Replay& SyncState(const EmulatorSyncState& state);
	const block_type& Data();
	Replay& Data(const block_type&);
	const Message& Read(int side) const;
	Replay& Write(u8 side, const Message&);
	u64 Length();
	u64 Pos() const;
	int Sides() const;
	Replay& Rewind();
	Replay& Seek(u64 position);
	Replay& NextFrame();
protected:
	ReplayMode _mode;
	EmulatorSyncState _state;
	block_type _data;
	input_container _input;
	u64 _playback_frame;
	u64 _length;
};