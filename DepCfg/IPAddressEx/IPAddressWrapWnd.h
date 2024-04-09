#pragma once


// CIPAddressWrapWnd

class CIPAddressWrapWnd : public CWnd
{
	DECLARE_DYNAMIC(CIPAddressWrapWnd)

	CIPAddressCtrl	m_wndIPAddress;
public:
	CIPAddressWrapWnd();
	virtual ~CIPAddressWrapWnd();

	BOOL Create(   DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );
protected:
	template < UINT msg > LRESULT OnMessage(WPARAM w, LPARAM l)
	{
		LRESULT r = m_wndIPAddress.SendMessage( msg, w, l );
		ResizeChild();
		return r;
	}

	void	ResizeChild();
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize( UINT nType, int cx, int cy );
	virtual BOOL OnCommand( WPARAM w, LPARAM l );
};


