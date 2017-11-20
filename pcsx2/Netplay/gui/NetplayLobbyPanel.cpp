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

void NetplayLobbyPanel::AddChatMessage(const std::string &username, const std::string &message)
{
	//m_NetplayChatTextCtrl->SetDefaultStyle(wxTextAttr(*wxBLUE));
	m_NetplayChatTextCtrl->AppendText(username);
	//m_NetplayChatTextCtrl->SetDefaultStyle(wxTextAttr(*wxGREEN));
	m_NetplayChatTextCtrl->AppendText(": ");
	//m_NetplayChatTextCtrl->SetDefaultStyle(wxTextAttr(*wxBLACK));
	m_NetplayChatTextCtrl->AppendText(message);
	m_NetplayChatTextCtrl->AppendText("\r\n");
}

void NetplayLobbyPanel::OnStart(wxCommandEvent & event)
{
	if (m_start_handler)
		m_start_handler();
}

void NetplayLobbyPanel::SetSendTextHandler(const std::function<void(std::string message)>& handler)
{
	m_sendtext_handler = handler;
}

void NetplayLobbyPanel::OnSendText(wxCommandEvent& event)
{
	std::string msg = m_NetplayConsoleEntryTextCtrl->GetValue().mb_str();

	AddChatMessage("<me>", msg);

	if (m_sendtext_handler)
	{
		m_sendtext_handler(msg);
	}

	m_NetplayConsoleEntryTextCtrl->Clear();
}