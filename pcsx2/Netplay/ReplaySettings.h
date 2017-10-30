#pragma once
#include "Utilities\IniInterface.h"

struct ReplaySettings
{
	bool IsEnabled;
	bool FastForward;

	wxString FilePath;
	ReplaySettings();
};