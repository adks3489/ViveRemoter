
// ViveRemoterDlg.h : ���Y��
//

#pragma once
#include <memory>
#include <thread>
#include "OpenVRWrapper.h"

// CViveRemoterDlg ��ܤ��
class CViveRemoterDlg : public CDialogEx
{
// �غc
public:
	CViveRemoterDlg(CWnd* pParent = NULL);	// �зǫغc�禡
	~CViveRemoterDlg();
// ��ܤ�����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIVEREMOTER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �䴩


// �{���X��@
protected:
	HICON m_hIcon;

	// ���ͪ��T�������禡
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
