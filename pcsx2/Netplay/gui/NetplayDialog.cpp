#include "PrecompiledHeader.h"
#include "NetplayDialog.h"
#include "NetplaySettingsPanel.h"

void NetplayDialog::OnClose( wxCloseEvent& event )
{
	if(m_close_handler)
		m_close_handler();
}
void NetplayDialog::OnCancelButtonClick( wxCommandEvent& event )
{
	OnClose(wxCloseEvent());
}
void NetplayDialog::OnOKButtonClick( wxCommandEvent& event )
{
	if(m_ok_handler)
		m_ok_handler();
}
void NetplayDialog::SetStatus(const wxString& status)
{
	this->m_statusText->SetLabel(status);
}
void NetplayDialog::SetContent( wxPanel* content )
{
	if(m_content)
		m_content->Hide();
	m_contentSizer->Clear();
	m_contentSizer->Add(content, 1, wxEXPAND, 5);
	m_content = content;
	m_content->Show();
	Fit();
}
wxPanel* NetplayDialog::GetContent()
{
	return m_content;
}
void NetplayDialog::SetReadonly(bool readonly)
{
	if(m_content)
		m_content->Enable(!readonly);
}

void NetplayDialog::SetOKHandler(const event_handler_type& handler)
{
	m_ok_handler = handler;
}
void NetplayDialog::SetCloseEventHandler(const event_handler_type& handler)
{
	m_close_handler = handler;
}
void NetplayDialog::SetSettings(const NetplaySettings& settings)
{
	m_settingsPanel.SetSettings(settings);
}
const NetplaySettings& NetplayDialog::GetSettings()
{
	return m_settingsPanel.GetSettings();
}
NetplayDialog::NetplayDialog(wxWindow* parent) 
	: NetplayDialogBase(parent), m_content(0), m_settingsPanel(this), m_lobbyPanel(this)
{
	m_lobbyPanel.Hide();
	SetContent(&m_settingsPanel);
}
NetplaySettingsPanel& NetplayDialog::GetSettingsPanel()
{
	return m_settingsPanel;
}
NetplayLobbyPanel& NetplayDialog::GetLobbyPanel()
{
	return m_lobbyPanel;
}
