
// ViveRemoter.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CViveRemoterApp: 
// �аѾ\��@�����O�� ViveRemoter.cpp
//

class CViveRemoterApp : public CWinApp
{
public:
	CViveRemoterApp();

// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CViveRemoterApp theApp;