#include "PrecompiledHeader.h"
#include "NetplaySettingsPanel.h"



NetplaySettingsPanel::NetplaySettingsPanel(wxWindow* parent)
	: NetplaySettingsPanelBase(parent)
{
	FromSettings();
}
void NetplaySettingsPanel::FromSettings()
{
	this->m_hostPortSpinCtrl->SetValue(m_settings.HostPort);
	this->m_hostAddressTextCtrl->SetValue(m_settings.HostAddress);
	this->m_hostNumPlayersSpinCtrl->SetValue(m_settings.NumPlayers);

	this->m_listenPortSpinCtrl->SetValue(m_settings.ListenPort);

	switch(m_settings.Mode)
	{
	case ObserveMode:
		this->m_netplayModeNotebook->SetSelection(0);
		this->m_observeCheckBox->SetValue(true);
	case ConnectMode:
		this->m_netplayModeNotebook->SetSelection(0);
		this->m_observeCheckBox->SetValue(false);
		break;
	case HostMode:
		this->m_netplayModeNotebook->SetSelection(1);
		break;
	
	}
	this->m_usernameTextCtrl->SetValue(m_settings.Username);
	this->m_saveReplayCheckBox->SetValue(m_settings.SaveReplay);

	this->m_readOnlyMCDCheckBox->SetValue(m_settings.ReadonlyMemcard);
}
void NetplaySettingsPanel::ToSettings()
{
	m_settings.SaveReplay = this->m_saveReplayCheckBox->GetValue();
	m_settings.Username = this->m_usernameTextCtrl->GetValue();
	m_settings.ListenPort = this->m_listenPortSpinCtrl->GetValue();
	m_settings.HostAddress = this->m_hostAddressTextCtrl->GetValue();
	m_settings.HostPort = this->m_hostPortSpinCtrl->GetValue();
	
	if (this->m_netplayModeNotebook->GetSelection() == 0)
	{
		if (this->m_observeCheckBox->GetValue())
			m_settings.Mode = ObserveMode;
		else
			m_settings.Mode = ConnectMode;
	}
	else
	{
		m_settings.Mode = HostMode;
	}

	m_settings.ReadonlyMemcard = this->m_readOnlyMCDCheckBox->GetValue();

	m_settings.NumPlayers = this->m_hostNumPlayersSpinCtrl->GetValue();

	m_settings.SanityCheck();
	FromSettings();
}

void NetplaySettingsPanel::SetSettings(const NetplaySettings& settings)
{
	m_settings = settings;
	FromSettings();
}
const NetplaySettings& NetplaySettingsPanel::GetSettings()
{
	ToSettings();
	return m_settings;
}
void NetplaySettingsPanel::SetOKHandler(const event_handler_type& handler)
{
	m_ok_handler = handler;	
}
void NetplaySettingsPanel::OnConnect(wxCommandEvent& event) 
{ 
	if (m_ok_handler)
		m_ok_handler();
}

void NetplaySettingsPanel::OnHost(wxCommandEvent& event)
{
	if (m_ok_handler)
		m_ok_handler();
}


