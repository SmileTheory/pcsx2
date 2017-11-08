#include "PrecompiledHeader.h"
#include "Replay.h"
#include <fstream>
#include <sstream>
#include <streambuf>

Replay::Replay() : _playback_frame(0), _mode(None), _length(0) {}

static const char Header[] = "PCSX2REPV1";

bool Replay::LoadFromFile(const wxString& path, bool compressed)
{
	wxFile file(path);
	bool validFile = false;
	Mode(None);
	if(file.IsOpened())
	{
		size_t size;
		std::istringstream ss;
		if(compressed)
		{
			if(file.Read((char*)&size,sizeof(size)) != sizeof(size))
				return false;
			block_type uncompressed(size);
			size = file.Length() - sizeof(size);
			block_type compressed(size);
			if(file.Read(compressed.data(), size) != size)
				return false;

			if(!Utilities::Uncompress(compressed, uncompressed))
				return false;
			ss.str(std::string(uncompressed.begin(), uncompressed.end()));
		}
		else
		{
			if(file.Read((char*)&size,sizeof(size)) != sizeof(size))
				return false;
			block_type block(size);
			if(file.Read(block.data(), size) != size)
				return false;
			ss.str(std::string(block.begin(), block.end()));
		}
		
		char header_test[sizeof(Header)];
		if(!ss.read(header_test, sizeof(Header)))
			return false;
		if(memcmp(header_test, Header, sizeof(Header)))
			return false;
		if(!ss.read(_state.biosVersion, sizeof(_state.biosVersion)))
			return false;
		if(!ss.read(_state.discId, sizeof(_state.discId)))
			return false;
		if(!ss.read((char*)&size, sizeof(size)))
			return false;
		_data.clear();
		_data.resize(size);
		if(size)
		{
			if(!ss.read((char*)_data.data(), size))
				return false;
		}
		if(!ss.read((char*)&size, sizeof(size)))
			return false;
		_input.clear();
		_input.resize(size);
		if(size)
		{
			for(size_t i = 0; i < _input.size(); i++)
			{
				if(!ss.read((char*)&size, sizeof(size)))
					return false;
				if(size)
				{
					for(size_t j = 0; j < size; j++)
					{
						Message m;
						if(!ss.read(m.input, sizeof(m.input)))
							return false;
						_input[i].push_back(m);
					}
				}
			}
		}
		_length = Length();
	}
	else
		throw std::exception("Cannot open file");
	return true;
}

u64 Replay::Pos() const
{
	return _playback_frame;
}

const Replay& Replay::SaveToFile(const wxString& path, bool compress) const
{
	wxFile file(path, wxFile::write);
	if(file.IsOpened())
	{
		std::ostringstream ss;
		ss.write(Header, sizeof(Header));
		ss.write(_state.biosVersion, sizeof(_state.biosVersion));
		ss.write(_state.discId, sizeof(_state.discId));
		size_t size = _data.size();
		ss.write((const char*)&size, sizeof(size));
		ss.write((const char*)_data.data(), size);
		size = _input.size();
		ss.write((const char*)&size, sizeof(size));
		for(size_t i = 0; i < _input.size(); i++)
		{
			size = _input[i].size();
			ss.write((const char*)&size, sizeof(size));
			for(size_t j = 0; j < size; j++)
				ss.write(_input[i][j].input, sizeof(_input[i][j].input));
		}
		std::string data = ss.str();
		if(compress)
		{
			block_type uncompressed(data.begin(), data.end());
			block_type compressed;
			if(!Utilities::Compress(uncompressed, compressed))
				throw std::exception("Unable to compress data");
			size = uncompressed.size();
			file.Write((char*)&size,sizeof(size));
			file.Write(compressed.data(), compressed.size());
		}
		else
		{
			size = data.length();
			file.Write((char*)&size,sizeof(size));
			file.Write(data.data(), data.length());
		}
	}
	else
		throw std::exception("Cannot open file");
	return *this;
}
ReplayMode Replay::Mode() const
{
	return _mode;
}
Replay& Replay::Mode(ReplayMode mode)
{
	_mode = mode;
	return *this;
}
const EmulatorSyncState& Replay::SyncState() const
{
	return _state;
}
Replay& Replay::SyncState(const EmulatorSyncState& state)
{
	if(_mode != Recording)
		throw std::exception("Write operation while not in Recording mode");
	_state = state;
	return *this;
}
const Replay::block_type& Replay::Data()
{
	return _data;
}
Replay& Replay::Data(const block_type& data)
{
	if(_mode != Recording)
		throw std::exception("Write operation while not in Recording mode");
	_data = data;
	return *this;
}
const Message& Replay::Read(int side) const
{
	if(_mode != Playback)
		throw std::exception("Read operation while not in Playback mode");
	return _input[side][_playback_frame];
}
Replay& Replay::Write(u8 side, const Message& msg)
{
	if(_mode != Recording)
		throw std::exception("Write operation while not in Recording mode");
	if(_input.size() <= side)
		_input.resize(side+1);
	_input[side].push_back(msg);
	return *this;
}
u64 Replay::Length()
{
	if(_mode != Playback || !_length)
	{
		auto elem = std::min_element(_input.begin(), _input.end(), [&](const input_type& a, const input_type& b){
			return a.size() < b.size();
		});
		if(elem != _input.end())
		{
			_length = elem->size();
		}
		else
		{
			_length = 0;
		}
	}
	return _length;
}
int Replay::Sides() const
{
	return _input.size();
}
Replay& Replay::Rewind()
{
	if(_mode != Playback)
		throw std::exception("Rewind operation while not in Playback mode");
	_playback_frame = 0;
	return *this;
}
Replay& Replay::Seek(u64 position)
{
	if(_mode != Playback)
		throw std::exception("Seek operation while not in Playback mode");
	_playback_frame = position;
	return *this;
}
Replay& Replay::NextFrame()
{
	if(_mode != Playback)
		throw std::exception("NextFrame operation while not in Playback mode");
	++_playback_frame;
	return *this;
}