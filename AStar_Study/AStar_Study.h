
// AStar_Study.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAStar_StudyApp:
// �йش����ʵ�֣������ AStar_Study.cpp
//

class CAStar_StudyApp : public CWinApp
{
public:
	CAStar_StudyApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAStar_StudyApp theApp;