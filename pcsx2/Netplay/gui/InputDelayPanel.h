#include "NetplayDialogBase.h"

class InputDelayPanel : public InputDelayPanelBase
{
public:
	InputDelayPanel( wxWindow* parent );
	void SetInputDelay(int value);
	int GetInputDelay();
	void SetReadOnly(bool readonly = true);
	void GetReadOnly();
};