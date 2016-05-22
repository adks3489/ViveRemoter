
// ViveRemoterDlg.h : 標頭檔
//

#pragma once
#include <memory>
#include <thread>
#include "OpenVRWrapper.h"

// CViveRemoterDlg 對話方塊
class CViveRemoterDlg : public CDialogEx
{
// 建構
public:
	CViveRemoterDlg(CWnd* pParent = NULL);	// 標準建構函式
	~CViveRemoterDlg();
// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIVEREMOTER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	OpenVRWrapper m_OpenVR;
	std::shared_ptr<std::thread> m_spThread;
	bool m_bRunning;
	void RunMainLoop();
	void HandleTPadEvent();
	void HandleTPad();
	float TouchStartX = 99;
	float TouchStartY = 99;
	float TouchX;
	float TouchY;
	ULONGLONG TouchTime;
	bool m_bMouseLeftClick;
	bool m_bMouseRightClick;
};
