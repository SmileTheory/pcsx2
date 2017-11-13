///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov  6 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __NETPLAYDIALOGBASE_H__
#define __NETPLAYDIALOGBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
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
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
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
		wxStdDialogButtonSizer* m_dialogButtonSizer;
		wxButton* m_dialogButtonSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnCancelButtonClick( wxCommandEvent& event ) { event.Skip(); }
		
	
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
		wxStaticText* m_usernameLabel;
		wxTextCtrl* m_usernameTextCtrl;
		wxCheckBox* m_saveReplayCheckBox;
		wxNotebook* m_netplayModeNotebook;
		wxPanel* m_connect;
		wxStaticText* m_hostAddressLabel;
		wxTextCtrl* m_hostAddressTextCtrl;
		wxStaticText* m_hostPortLabel;
		wxSpinCtrl* m_hostPortSpinCtrl;
		wxCheckBox* m_observeCheckBox;
		wxButton* m_connectButton;
		wxPanel* m_host;
		wxStaticText* m_hostPortLabel1;
		wxSpinCtrl* m_listenPortSpinCtrl;
		wxStaticText* m_hostNumPlayersLabel;
		wxSpinCtrl* m_hostNumPlayersSpinCtrl;
		wxCheckBox* m_readOnlyMCDCheckBox;
		wxButton* m_hostButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnConnect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHost( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		NetplaySettingsPanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 287,272 ), long style = wxTAB_TRAVERSAL ); 
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
		virtual void OnSendText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMoveUp( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMoveDown( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStart( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		NetplayLobbyPanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 530,336 ), long style = wxTAB_TRAVERSAL ); 
		~NetplayLobbyPanelBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MyPanel3
///////////////////////////////////////////////////////////////////////////////
class MyPanel3 : public wxPanel 
{
	private:
	
	protected:
		wxNotebook* m_notebook2;
	
	public:
		
		MyPanel3( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL ); 
		~MyPanel3();
	
};

#endif //__NETPLAYDIALOGBASE_H__
