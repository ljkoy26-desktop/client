/*
 * 이 include 파일은 cwebpage DLL을 사용하는 모든 C 소스 코드에
 * 포함되도록 만들어졌다.
 */

#ifndef __CWEBPAGE_H_INCLUDED
#define __CWEBPAGE_H_INCLUDED

#include <windows.h>

#ifdef __cplusplus
{
#endif

long WINAPI EmbedBrowserObject(HWND);
#define EMBEDBROWSEROBJECT EmbedBrowserObject
typedef long WINAPI EmbedBrowserObjectPtr(HWND);

void WINAPI UnEmbedBrowserObject(HWND);
#define UNEMBEDBROWSEROBJECT UnEmbedBrowserObject
typedef long WINAPI UnEmbedBrowserObjectPtr(HWND);

long WINAPI DisplayHTMLPage(HWND, LPCTSTR);
#define DISPLAYHTMLPAGE DisplayHTMLPage
typedef long WINAPI DisplayHTMLPagePtr(HWND, LPCTSTR);

long WINAPI DisplayHTMLStr(HWND, LPCTSTR);
#define DISPLAYHTMLSTR DisplayHTMLStr
typedef long WINAPI DisplayHTMLStrPtr(HWND, LPCTSTR);

#ifdef __cplusplus
}
#endif

#endif /* __CWEBPAGE_H_INCLUDED */
