#include "PrecompiledHeader.h"
#include "NetplayLobbyPanel.h"

NetplayLobbyPanel::NetplayLobbyPanel( wxWindow* parent ) : NetplayLobbyPanelBase(parent) {}

void NetplayLobbyPanel::SetInputDelay(int value)
{
	this->m_inputDelaySpinner->SetValue(value);
}
int NetplayLobbyPanel::GetInputDelay()
{
	return this->m_inputDelaySpinner->GetValue();
}

void NetplayLobbyPanel::SetReadOnly(bool readonly)
{
	this->m_inputDelaySpinner->Enable(!readonly);
}

void NetplayLobbyPanel::SetStartHandler(const event_handler_type & handler)
{
	m_start_handler = handler;
}

void NetplayLobbyPanel::SetUserlist(const std::vector<std::string> &usernames)
{
	m_playersBox->Clear();
	for(auto &str : usernames)
		m_playersBox->Append(wxString::wxString(str));
}

void NetplayLobbyPanel::OnStart(wxCommandEvent & event)
{
	if (m_start_handler)
		m_start_handler();
}
