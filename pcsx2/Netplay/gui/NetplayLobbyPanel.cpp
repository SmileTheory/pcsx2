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