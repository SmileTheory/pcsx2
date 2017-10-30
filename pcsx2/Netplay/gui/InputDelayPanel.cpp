#include "PrecompiledHeader.h"
#include "InputDelayPanel.h"

InputDelayPanel::InputDelayPanel( wxWindow* parent ) : InputDelayPanelBase(parent) {}


void InputDelayPanel::SetInputDelay(int value)
{
	this->m_inputDelaySpinner->SetValue(value);
}
int InputDelayPanel::GetInputDelay()
{
	return this->m_inputDelaySpinner->GetValue();
}

void InputDelayPanel::SetReadOnly(bool readonly)
{
	this->m_inputDelaySpinner->Enable(!readonly);
}