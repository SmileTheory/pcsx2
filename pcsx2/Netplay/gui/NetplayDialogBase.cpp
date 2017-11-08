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
	
	m_dialogButtonSizer = new wxStdDialogButtonSizer();
	m_dialogButtonSizerOK = new wxButton( this, wxID_OK );
	m_dialogButtonSizer->AddButton( m_dialogButtonSizerOK );
	m_dialogButtonSizerCancel = new wxButton( this, wxID_CANCEL );
	m_dialogButtonSizer->AddButton( m_dialogButtonSizerCancel );
	m_dialogButtonSizer->Realize();
	
	bSizer4->Add( m_dialogButtonSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer4 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( NetplayDialogBase::OnClose ) );
	m_dialogButtonSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NetplayDialogBase::OnCancelButtonClick ), NULL, this );
	m_dialogButtonSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NetplayDialogBase::OnOKButtonClick ), NULL, this );
}

NetplayDialogBase::~NetplayDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( NetplayDialogBase::OnClose ) );
	m_dialogButtonSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NetplayDialogBase::OnCancelButtonClick ), NULL, this );
	m_dialogButtonSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NetplayDialogBase::OnOKButtonClick ), NULL, this );
	
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
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_usernameLabel = new wxStaticText( this, wxID_ANY, _("Username:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_usernameLabel->Wrap( -1 );
	fgSizer3->Add( m_usernameLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_usernameTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_usernameTextCtrl, 0, wxALL|wxEXPAND, 5 );
	
	m_localPortLabel = new wxStaticText( this, wxID_ANY, _("Local Port:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_localPortLabel->Wrap( -1 );
	fgSizer3->Add( m_localPortLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_localPortSpinCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 65535, 7500 );
	fgSizer3->Add( m_localPortSpinCtrl, 0, wxALL|wxEXPAND, 5 );
	
	m_hostAddressLabel = new wxStaticText( this, wxID_ANY, _("Host Address:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_hostAddressLabel->Wrap( -1 );
	fgSizer3->Add( m_hostAddressLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_hostAddressTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_hostAddressTextCtrl, 0, wxALL|wxEXPAND, 5 );
	
	m_hostPortLabel = new wxStaticText( this, wxID_ANY, _("Host Port:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_hostPortLabel->Wrap( -1 );
	fgSizer3->Add( m_hostPortLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_hostPortSpinCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1065535, 7500 );
	fgSizer3->Add( m_hostPortSpinCtrl, 0, wxALL|wxEXPAND, 5 );
	
	
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

InputDelayPanelBase::InputDelayPanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	m_inputDelayLabel = new wxStaticText( this, wxID_ANY, _("Input Delay:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_inputDelayLabel->Wrap( -1 );
	bSizer11->Add( m_inputDelayLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_inputDelaySpinner = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 15, 1 );
	bSizer11->Add( m_inputDelaySpinner, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	bSizer10->Add( bSizer11, 0, wxEXPAND, 5 );
	
	
	bSizer10->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer10 );
	this->Layout();
}

InputDelayPanelBase::~InputDelayPanelBase()
{
}
