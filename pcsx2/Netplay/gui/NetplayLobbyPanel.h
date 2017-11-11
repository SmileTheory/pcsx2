#include "NetplayDialogBase.h"

class NetplayLobbyPanel : public NetplayLobbyPanelBase
{
public:
	NetplayLobbyPanel( wxWindow* parent );
	void SetInputDelay(int value);
	int GetInputDelay();
	void SetReadOnly(bool readonly = true);
	void GetReadOnly();
};