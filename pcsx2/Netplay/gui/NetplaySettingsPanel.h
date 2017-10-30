#pragma once

#include "PrecompiledHeader.h"
#include "NetplayDialogBase.h"
#include "AppConfig.h"
#include "Netplay\NetplaySettings.h"

class NetplaySettingsPanel : public NetplaySettingsPanelBase
{
public:
	NetplaySettingsPanel(wxWindow* parent);
	void SetSettings(const NetplaySettings& settings);
	const NetplaySettings& GetSettings();
protected:
	void FromSettings();
	void ToSettings();

	void UpdateUI( wxCommandEvent& event = wxCommandEvent() );

	NetplaySettings m_settings;
};