/*-----------------------------------------------------------------------------

	WEBBROWSER.h

	webbrowser interface.

	2005,1,31, sobeit

-----------------------------------------------------------------------------*/

#ifndef __VS_UI_WEBBROWSER_H__
#define __VS_UI_WEBBROWSER_H__

#ifdef PLATFORM_WINDOWS
//#include <ddraw.h>
/* ATLBASE.H가 먼저 와야 한다: EXDISP.H의 COM 자동화 인터페이스 선언에 필요한
   COM/OLE 전제 조건(IUnknown, IDispatch, 인터페이스 매크로 등)을 끌어오기 때문이다. */
#include "ATLBASE.H"
#include "EXDISP.H"

//-----------------------------------------------------------------------------
// MWEBBROWSER
//
//-----------------------------------------------------------------------------
class C_VS_UI_WEBBROWSER
{
public:
	
private:
	IWebBrowser2*			m_pWebBrowser;
	
	HWND				m_hWnd_Explorer;

	bool			m_IsMouseInWebBrowser;
public:
	C_VS_UI_WEBBROWSER();
	~C_VS_UI_WEBBROWSER();

	bool	Start(HWND hwndApp, char* pURL, void* pWebOjbect);
	void	Finish();

	void	SetWebBrowserSize(int StartX, int StartY, int Width, int Height);
	void	SetWebBrowserStyle(DWORD dwstyle);
	DWORD	GetWebBrowserStyle();
	int		GetMouseCursorInfo();
	bool	IsInRectPoint(int X, int Y);

//	IWebBrowser2* GetIWebBrowser2() {return m_pWebBrowser;}
};

#else
// WebBrowser는 비-Windows 플랫폼에서 사용 불가
class C_VS_UI_WEBBROWSER
{
public:
    C_VS_UI_WEBBROWSER() {}
    ~C_VS_UI_WEBBROWSER() {}

    // 스텁 구현 - 웹 브라우저 기능은 비-Windows 플랫폼에서 사용 불가
    bool Start(void* hwndApp, char* pURL, void* pWebOjbect) { return false; }
    void Finish() {}
    void SetWebBrowserSize(int StartX, int StartY, int Width, int Height) {}
    void SetWebBrowserStyle(unsigned long dwstyle) {}
    unsigned long GetWebBrowserStyle() { return 0; }
    int GetMouseCursorInfo() { return 0; }
    bool IsInRectPoint(int X, int Y) { return false; }

private:
    bool m_IsMouseInWebBrowser;
};

#endif // PLATFORM_WINDOWS

#endif