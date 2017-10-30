#include "PrecompiledHeader.h"
#include "NetplaySettingsPanel.h"



NetplaySettingsPanel::NetplaySettingsPanel(wxWindow* parent)
	: NetplaySettingsPanelBase(parent)
{
	FromSettings();
}
void NetplaySettingsPanel::FromSettings()
{
	this->m_localPortSpinCtrl->SetValue(m_settings.LocalPort);
	this->m_hostPortSpinCtrl->SetValue(m_settings.HostPort);
	this->m_hostAddressTextCtrl->SetValue(m_settings.HostAddress);

	switch(m_settings.Mode)
	{
	case ConnectMode:
		this->m_connectRadioButton->SetValue(true);
		break;
	case HostMode:
		this->m_hostRadioButton->SetValue(true);
		break;
	}
	this->m_usernameTextCtrl->SetValue(m_settings.Username);
	this->m_saveReplayCheckBox->SetValue(m_settings.SaveReplay);

	this->m_readOnlyMCDCheckBox->SetValue(m_settings.ReadonlyMemcard);

	UpdateUI();
}
void NetplaySettingsPanel::ToSettings()
{
	m_settings.LocalPort = this->m_localPortSpinCtrl->GetValue();
	m_settings.HostPort = this->m_hostPortSpinCtrl->GetValue();

	m_settings.SaveReplay = this->m_saveReplayCheckBox->GetValue();
	m_settings.Username = this->m_usernameTextCtrl->GetValue();

	m_settings.HostAddress = this->m_hostAddressTextCtrl->GetValue();
	if(this->m_connectRadioButton->GetValue())
		m_settings.Mode = ConnectMode;
	if(this->m_hostRadioButton->GetValue())
		m_settings.Mode = HostMode;

	m_settings.ReadonlyMemcard = this->m_readOnlyMCDCheckBox->GetValue();

	m_settings.SanityCheck();
	FromSettings();
}
void NetplaySettingsPanel::UpdateUI(wxCommandEvent& event)
{
	bool host = this->m_hostRadioButton->GetValue();

	this->m_hostAddressLabel->Show(!host);
	this->m_hostAddressTextCtrl->Show(!host);
	this->m_hostPortLabel->Show(!host);
	this->m_hostPortSpinCtrl->Show(!host);
	this->m_readOnlyMCDCheckBox->Show(host);

	this->GetParent()->Fit();
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

