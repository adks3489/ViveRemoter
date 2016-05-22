
// ViveRemoterDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "ViveRemoter.h"
#include "ViveRemoterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CViveRemoterDlg 對話方塊



CViveRemoterDlg::CViveRemoterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VIVEREMOTER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
CViveRemoterDlg::~CViveRemoterDlg()
{
	m_OpenVR.Shutdown();
}
void CViveRemoterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViveRemoterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CViveRemoterDlg 訊息處理常式

BOOL CViveRemoterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	if(m_OpenVR.Init())
	{
		m_bRunning = true;
		m_spThread = std::make_shared<std::thread>(std::bind(&CViveRemoterDlg::RunMainLoop, this));
	}

	return TRUE;
}

void CViveRemoterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CViveRemoterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CViveRemoterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CViveRemoterDlg::RunMainLoop()
{
	while (m_bRunning)
	{
		m_OpenVR.ProcessVREvent();
		m_OpenVR.UpdateController();

		if(m_OpenVR.m_ControllerEvent.EventType == vr::VREvent_TrackedDeviceActivated)
			SetDlgItemText(IDC_TXT_STAT, L"Controller Connected");

		if(m_OpenVR.m_ControllerEvent.Button == vr::EVRButtonId::k_EButton_SteamVR_Touchpad)
			HandleTPadEvent();
		HandleTPad();


		m_OpenVR.m_ControllerEvent.Reset();
		Sleep(10);
	}
}

void CViveRemoterDlg::HandleTPadEvent()
{
	switch (m_OpenVR.m_ControllerEvent.EventType)
	{
		case vr::VREvent_ButtonTouch:
			if (m_OpenVR.m_ControllerEvent.Index == 0)
			{
				if (m_OpenVR.m_ControllerEvent.X < -0.9 || m_OpenVR.m_ControllerEvent.X > 0.9)
					break;

				TouchStartX = m_OpenVR.m_ControllerEvent.X;
				TouchStartY = m_OpenVR.m_ControllerEvent.Y;
				TouchTime = GetTickCount64();
			}
			break;
		case vr::VREvent_ButtonUntouch:
			if (m_OpenVR.m_ControllerEvent.Index == 0)
			{
				TouchStartX = 99;
				TouchStartY = 99;
			}
			break;
		case vr::VREvent_TouchPadMove:
			break;
		case vr::VREvent_ButtonPress:
			if (m_OpenVR.m_ControllerEvent.Index == 0)
			{
				POINT pos;
				GetCursorPos(&pos);
				INPUT input;
				ZeroMemory(&input, sizeof(input));
				input.type = INPUT_MOUSE;
				if (m_OpenVR.m_ControllerEvent.X < 0)
				{
					input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					m_bMouseLeftClick = true;
				}
				else
				{
					input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
					m_bMouseRightClick = true;
				}
				SendInput(1, &input, sizeof(input));
			}
			break;
		case vr::VREvent_ButtonUnpress:
			if (m_OpenVR.m_ControllerEvent.Index == 0)
			{
				POINT pos;
				GetCursorPos(&pos);
				INPUT input;
				ZeroMemory(&input, sizeof(input));
				input.type = INPUT_MOUSE;
				if (m_bMouseLeftClick)
				{
					input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					SendInput(1, &input, sizeof(input));
					m_bMouseLeftClick = false;
				}
				if (m_bMouseRightClick)
				{
					input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
					SendInput(1, &input, sizeof(input));
					m_bMouseRightClick = false;
				}
			}
			break;
	}
}

void CViveRemoterDlg::HandleTPad()
{
	if (m_OpenVR.m_spController1 == nullptr)
		return;
	if (m_OpenVR.m_spController1->TPadTouch && (TouchStartX!=99||TouchStartY!=99))
	{
		ULONGLONG UnTouchTime = GetTickCount64();
		TouchX = m_OpenVR.m_spController1->TPadX;
		TouchY = m_OpenVR.m_spController1->TPadY;
		if (UnTouchTime > TouchTime)
		{
			int offsetX = 0, offsetY = 0;
			POINT pos;
			GetCursorPos(&pos);
			offsetX = (TouchX - TouchStartX) * 300.0;
			offsetX = abs(offsetX) > 5 ? offsetX : 0;
			offsetY = (TouchStartY - TouchY) * 300.0;
			offsetY = abs(offsetY) > 5 ? offsetY : 0;
			SetCursorPos(pos.x + offsetX, pos.y + offsetY);
			TouchStartX = TouchX;
			TouchStartY = TouchY;
			TouchTime = UnTouchTime;
		}
	}
}