#pragma once

#include "PrecompiledHeader.h"
#include "NetplayDialogBase.h"
#include "AppConfig.h"
#include "Netplay\NetplaySettings.h"

class NetplaySettingsPanel : public NetplaySettingsPanelBase
{
public:
	typedef std::function<void()> event_handler_type;
	NetplaySettingsPanel(wxWindow* parent);
	void SetSettings(const NetplaySettings& settings);
	const NetplaySettings& GetSettings();
	void SetOKHandler(const event_handler_type& handler);
protected:
	void FromSettings();
	void ToSettings();

	virtual void OnConnect(wxCommandEvent& event);
	virtual void OnHost(wxCommandEvent& event);

	NetplaySettings m_settings;
	event_handler_type m_ok_handler;
};