///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov  6 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "PrecompiledHeader.h"

#include "NetplayDialogBase.h"

///////////////////////////////////////////////////////////////////////////

NetplayDialogBase::NetplayDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 250,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_statusText = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_statusText->Wrap( -1 );
	m_statusText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	
	bSizer4->Add( m_statusText, 0, wxALL|wxEXPAND, 7 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer4->Add( m_staticline1, 0, wxEXPAND, 5 );
	
	m_contentSizer = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer4->Add( m_contentSizer, 1, wxEXPAND, 5 );
	
	m_quitButton = new wxButton( this, wxID_ANY, _("Quit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_quitButton, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	this->SetSizer( bSizer4 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( NetplayDialogBase::OnClose ) );
	m_quitButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NetplayDialogBase::OnQuit ), NULL, this );
}

NetplayDialogBase::~NetplayDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( NetplayDialogBase::OnClose ) );
	m_quitButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NetplayDialogBase::OnQuit ), NULL, this );
	
}

NetplaySettingsPanelBase::NetplaySettingsPanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_connectRadioButton = new wxRadioButton( this, wxID_ANY, _("Connect"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	bSizer1->Add( m_connectRadioButton, 0, wxALL, 5 );
	
	m_hostRadioButton = new wxRadioButton( this, wxID_ANY, _("Host a game"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_hostRadioButton, 0, wxALL, 5 );
	
	
	bSizer2->Add( bSizer1, 0, wxALIGN_CENTER, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_usernameLabel = new wxStaticText( this, wxID_ANY, _("Username:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_usernameLabel->Wrap( -1 );
	fgSizer3->Add( m_usernameLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_usernameTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_usernameTextCtrl, 0, wxALL|wxEXPAND, 5 );
	
	m_hostAddressLabel = new wxStaticText( this, wxID_ANY, _("Host Address:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_hostAddressLabel->Wrap( -1 );
	fgSizer3->Add( m_hostAddressLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_hostAddressTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_hostAddressTextCtrl, 0, wxALL|wxEXPAND, 5 );
	
	m_hostPortLabel = new wxStaticText( this, wxID_ANY, _("Host Port:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_hostPortLabel->Wrap( -1 );
	fgSizer3->Add( m_hostPortLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_hostPortSpinCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 65535, 7500 );
	fgSizer3->Add( m_hostPortSpinCtrl, 0, wxALL|wxEXPAND, 5 );
	
	m_hostNumPlayersLabel = new wxStaticText( this, wxID_ANY, _("# of players:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_hostNumPlayersLabel->Wrap( -1 );
	fgSizer3->Add( m_hostNumPlayersLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_hostNumPlayersSpinCtrl = new wxSpinCtrl( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 2, 8, 2 );
	fgSizer3->Add( m_hostNumPlayersSpinCtrl, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer2->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	m_saveReplayCheckBox = new wxCheckBox( this, wxID_ANY, _("Save Replay"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_saveReplayCheckBox, 0, wxALL, 5 );
	
	m_readOnlyMCDCheckBox = new wxCheckBox( this, wxID_ANY, _("Read-only Memory Card"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_readOnlyMCDCheckBox, 0, wxALL, 5 );
	
	
	this->SetSizer( bSizer2 );
	this->Layout();
	
	// Connect Events
	m_connectRadioButton->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( NetplaySettingsPanelBase::UpdateUI ), NULL, this );
	m_hostRadioButton->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( NetplaySettingsPanelBase::UpdateUI ), NULL, this );
}

NetplaySettingsPanelBase::~NetplaySettingsPanelBase()
{
	// Disconnect Events
	m_connectRadioButton->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( NetplaySettingsPanelBase::UpdateUI ), NULL, this );
	m_hostRadioButton->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( NetplaySettingsPanelBase::UpdateUI ), NULL, this );
	
}

NetplayLobbyPanelBase::NetplayLobbyPanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableRow( 0 );
	fgSizer2->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Chat") ), wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_NetplayChatTextCtrl = new wxTextCtrl( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
	bSizer8->Add( m_NetplayChatTextCtrl, 1, wxEXPAND, 5 );
	
	
	sbSizer1->Add( bSizer8, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_NetplayConsoleEntryTextCtrl = new wxTextCtrl( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizer7->Add( m_NetplayConsoleEntryTextCtrl, 1, wxBOTTOM|wxEXPAND|wxTOP, 5 );
	
	m_sendButton = new wxButton( sbSizer1->GetStaticBox(), wxID_ANY, _("Send"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_sendButton, 0, wxALIGN_RIGHT|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	
	sbSizer1->Add( bSizer7, 0, wxALIGN_BOTTOM|wxEXPAND, 5 );
	
	
	fgSizer2->Add( sbSizer1, 1, wxEXPAND|wxRIGHT, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Players") ), wxVERTICAL );
	
	m_playersBox = new wxListBox( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sbSizer2->Add( m_playersBox, 1, wxEXPAND, 5 );
	
	m_moveUpButton = new wxButton( sbSizer2->GetStaticBox(), wxID_ANY, _("Move up"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_moveUpButton, 0, wxALIGN_BOTTOM|wxEXPAND|wxTOP, 5 );
	
	m_moveDownButton = new wxButton( sbSizer2->GetStaticBox(), wxID_ANY, _("Move down"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_moveDownButton, 0, wxBOTTOM|wxEXPAND|wxTOP, 5 );
	
	
	fgSizer2->Add( sbSizer2, 1, wxEXPAND, 5 );
	
	
	bSizer10->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_startButton = new wxButton( this, wxID_ANY, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_startButton, 0, wxALL, 5 );
	
	m_inputDelayLabel = new wxStaticText( this, wxID_ANY, _("Input Delay:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_inputDelayLabel->Wrap( -1 );
	bSizer9->Add( m_inputDelayLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_inputDelaySpinner = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 30, 15 );
	bSizer9->Add( m_inputDelaySpinner, 0, wxALL|wxTOP, 5 );
	
	
	bSizer10->Add( bSizer9, 0, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer10 );
	this->Layout();
	
	// Connect Events
	m_NetplayConsoleEntryTextCtrl->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( NetplayLobbyPanelBase::OnTextEnter ), NULL, this );
	m_startButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NetplayLobbyPanelBase::OnStart ), NULL, this );
}

NetplayLobbyPanelBase::~NetplayLobbyPanelBase()
{
	// Disconnect Events
	m_NetplayConsoleEntryTextCtrl->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( NetplayLobbyPanelBase::OnTextEnter ), NULL, this );
	m_startButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NetplayLobbyPanelBase::OnStart ), NULL, this );
	
}
