#pragma once

#include <functional>

#include "PrecompiledHeader.h"
#include "NetplayDialogBase.h"
#include "AppConfig.h"
#include "Netplay\NetplaySettings.h"
#include "NetplaySettingsPanel.h"
#pragma once
#include "NetplayLobbyPanel.h"

class NetplayDialog : public NetplayDialogBase
{
public:
	typedef std::function<void()> event_handler_type;
	NetplayDialog(wxWindow* parent);
	void SetSettings(const NetplaySettings& settings);
	const NetplaySettings& GetSettings();
	void SetStatus(const wxString& status);
	void SetReadonly(bool readonly = true);
	void EnableCancel(bool enable = true);
	void SetOKHandler(const event_handler_type& handler);
	void SetSendChatMessageHandler(const std::function<void(std::string message)>& handler);
	void SetCloseEventHandler(const event_handler_type& handler);
	NetplaySettingsPanel& GetSettingsPanel();
	NetplayLobbyPanel& GetLobbyPanel();
	void SetContent(wxPanel* content);
	void OnClose( wxCloseEvent& event );
	wxPanel* GetContent();
protected:
	virtual void OnCancelButtonClick( wxCommandEvent& event );
	virtual void OnOKButtonClick( wxCommandEvent& event );

	NetplaySettingsPanel m_settingsPanel;
	NetplayLobbyPanel m_lobbyPanel;

	event_handler_type m_ok_handler;
	event_handler_type m_close_handler;
	wxPanel* m_content;
};
