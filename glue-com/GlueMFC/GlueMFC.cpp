
// GlueMFC.cpp : Defines the class behaviors for the application.
//
#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "GlueMFC.h"
#include "MainFrm.h"

#include "GlueMFCDoc.h"
#include "GlueMFCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGlueMFCApp

BEGIN_MESSAGE_MAP(CGlueMFCApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CGlueMFCApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW_FRAME, &CGlueMFCApp::OnFileNewFrame)
	ON_COMMAND(ID_FILE_NEW, &CGlueMFCApp::OnFileNew)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()

// the one and only glue
IGlue42Ptr theGlue;


// CGlueMFCApp construction

CGlueMFCApp::CGlueMFCApp() noexcept
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	throw_if_fail(hr);

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("GlueMFC.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	hr = CoCreateInstance(CLSID_Glue42, nullptr, CLSCTX_INPROC_SERVER, IID_IGlue42, reinterpret_cast<void**>(&theGlue));
	throw_if_fail(hr);
}

HRESULT CGlueMFCApp::raw_CreateApp(BSTR appDefName, GlueValue state, IAppAnnouncer* announcer)
{
	// todo: create and announce child
	OnFileNewFrame();
	return S_OK;
}

// The one and only CGlueMFCApp object

CGlueMFCApp theApp;


// CGlueMFCApp initialization

BOOL CGlueMFCApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGlueMFCDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CGlueMFCView));
	if (!pDocTemplate)
		return FALSE;
	m_pDocTemplate = pDocTemplate;
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// start glue
	GlueInstance instance = {};
	instance.ApplicationName = _com_util::ConvertStringToBSTR("GlueMFC");
	instance.UserName = _com_util::ConvertStringToBSTR("CHANGE_ME");
	theGlue->Start(instance);

	// register a child app
	GlueAppDefinition mfcDef;
	mfcDef.Name = _com_util::ConvertStringToBSTR("MFCChild");
	mfcDef.Title = _com_util::ConvertStringToBSTR("MFC Child");
	mfcDef.Category = _com_util::ConvertStringToBSTR("MFC");

	theGlue->AppFactoryRegistry->RegisterAppFactory(mfcDef, this);

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	auto view = dynamic_cast<CGlueMFCView*>(dynamic_cast<CMainFrame*>(m_pMainWnd)->GetActiveView());
	view->RegisterGlueWindow(m_pMainWnd);

	return TRUE;
}

int CGlueMFCApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CGlueMFCApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CGlueMFCApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CGlueMFCApp message handlers

void CGlueMFCApp::OnFileNewFrame()
{
	ASSERT(m_pDocTemplate != nullptr);

	CDocument* pDoc = nullptr;
	CFrameWnd* pFrame = nullptr;

	// Create a new instance of the document referenced
	// by the m_pDocTemplate member.
	if (m_pDocTemplate != nullptr)
		pDoc = m_pDocTemplate->CreateNewDocument();

	if (pDoc != nullptr)
	{
		// If creation worked, use create a new frame for
		// that document.
		pFrame = m_pDocTemplate->CreateNewFrame(pDoc, nullptr);
		if (pFrame != nullptr)
		{
			// Set the title, and initialize the document.
			// If document initialization fails, clean-up
			// the frame window and document.

			m_pDocTemplate->SetDefaultTitle(pDoc);
			if (!pDoc->OnNewDocument())
			{
				pFrame->DestroyWindow();
				pFrame = nullptr;
			}
			else
			{
				// Otherwise, update the frame
				m_pDocTemplate->InitialUpdateFrame(pFrame, pDoc, TRUE);
			}
		}
	}

	// If we failed, clean up the document and show a
	// message to the user.

	if (pFrame == nullptr || pDoc == nullptr)
	{
		delete pDoc;
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
	}
}

void CGlueMFCApp::OnFileNew()
{
	CDocument* pDoc = nullptr;
	CFrameWnd* pFrame;
	pFrame = DYNAMIC_DOWNCAST(CFrameWnd, CWnd::GetActiveWindow());

	if (pFrame != nullptr)
		pDoc = pFrame->GetActiveDocument();

	if (pFrame == nullptr || pDoc == nullptr)
	{
		// if it's the first document, create as normal
		CWinApp::OnFileNew();
	}
	else
	{
		// Otherwise, see if we have to save modified, then
		// ask the document to reinitialize itself.
		if (!pDoc->SaveModified())
			return;

		CDocTemplate* pTemplate = pDoc->GetDocTemplate();
		ASSERT(pTemplate != nullptr);

		if (pTemplate != nullptr)
			pTemplate->SetDefaultTitle(pDoc);
		pDoc->OnNewDocument();
	}
}


