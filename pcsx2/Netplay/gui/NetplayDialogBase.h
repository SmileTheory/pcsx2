///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov  6 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __NETPLAYDIALOGBASE_H__
#define __NETPLAYDIALOGBASE_H__

#include <wx/artprov.h>
//#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/radiobut.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/listbox.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NetplayDialogBase
///////////////////////////////////////////////////////////////////////////////
class NetplayDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_statusText;
		wxStaticLine* m_staticline1;
		wxBoxSizer* m_contentSizer;
		wxButton* m_quitButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnQuit( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		NetplayDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Netplay - PCSX2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 250,-1 ), long style = wxCAPTION|wxCLOSE_BOX|wxSTAY_ON_TOP|wxSYSTEM_MENU ); 
		~NetplayDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class NetplaySettingsPanelBase
///////////////////////////////////////////////////////////////////////////////
class NetplaySettingsPanelBase : public wxPanel 
{
	private:
	
	protected:
		wxRadioButton* m_connectRadioButton;
		wxRadioButton* m_hostRadioButton;
		wxStaticText* m_usernameLabel;
		wxTextCtrl* m_usernameTextCtrl;
		wxStaticText* m_hostAddressLabel;
		wxTextCtrl* m_hostAddressTextCtrl;
		wxStaticText* m_hostPortLabel;
		wxSpinCtrl* m_hostPortSpinCtrl;
		wxStaticText* m_hostNumPlayersLabel;
		wxSpinCtrl* m_hostNumPlayersSpinCtrl;
		wxCheckBox* m_saveReplayCheckBox;
		wxCheckBox* m_readOnlyMCDCheckBox;
		
		// Virtual event handlers, overide them in your derived class
		virtual void UpdateUI( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		NetplaySettingsPanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 287,246 ), long style = wxTAB_TRAVERSAL ); 
		~NetplaySettingsPanelBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class NetplayLobbyPanelBase
///////////////////////////////////////////////////////////////////////////////
class NetplayLobbyPanelBase : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* m_NetplayChatTextCtrl;
		wxTextCtrl* m_NetplayConsoleEntryTextCtrl;
		wxButton* m_sendButton;
		wxListBox* m_playersBox;
		wxButton* m_moveUpButton;
		wxButton* m_moveDownButton;
		wxButton* m_startButton;
		wxStaticText* m_inputDelayLabel;
		wxSpinCtrl* m_inputDelaySpinner;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStart( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		NetplayLobbyPanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 530,336 ), long style = wxTAB_TRAVERSAL ); 
		~NetplayLobbyPanelBase();
	
};

#endif //__NETPLAYDIALOGBASE_H__
