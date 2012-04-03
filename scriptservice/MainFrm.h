#pragma once

#include "HiddenBrowserView.h"

class CMainFrame;
typedef CFrameWindowImpl<CMainFrame, CWindow, CWinTraits<WS_POPUP, WS_EX_WINDOWEDGE> > CHiddenBrowserMainFrm;

class CMainFrame : 
	public CHiddenBrowserMainFrm
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDS_MAINFRAME)

	CHiddenBrowserView m_view;

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		CHAIN_MSG_MAP(CHiddenBrowserMainFrm)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
