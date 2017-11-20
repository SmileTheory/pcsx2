#include "NetplayDialogBase.h"

class NetplayLobbyPanel : public NetplayLobbyPanelBase
{
public:
	NetplayLobbyPanel( wxWindow* parent );

	typedef std::function<void()> event_handler_type;
	void SetStartHandler(const event_handler_type& handler);

	void SetInputDelay(int value);
	int GetInputDelay();
	void SetReadOnly(bool readonly = true);
	void SetUserlist(const std::vector<std::string> &usernames);

protected:
	virtual void OnStart(wxCommandEvent& event);
	event_handler_type m_start_handler;

};