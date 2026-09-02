#include "Client_PCH.h"
#include "CAvi.H"

CAVI::CAVI()
{
	bEndFlag=0;
}

// CAVI는 오프닝 인트로 동영상을 위해 Windows "MCI Digital Video" API(MCI_DGV_*
// 구조체/명령, MCIAVI.DRV)를 사용했다. 해당 드라이버는 16/32비트
// Video-for-Windows 시대의 기술로 64비트 Windows에서는 전혀 제공되지 않으므로,
// 누락된 MCI_DGV_* 구조체를 직접 정의해도(현재 Windows 10 SDK의 <mmsystem.h>에
// 더 이상 선언되어 있지 않음) 컴파일만 가능할 뿐 실제로는 동작하지 않는다.
// OS 수준의 드라이버가 x64 환경에 존재하지 않으므로
// mciSendCommand(MCI_OPEN, ...)는 런타임에 항상 실패한다.
// 비-Windows 플랫폼에만 스텁 처리하는 대신 모든 플랫폼에서 스텁 처리함.
// 참고: COpeningUpdate::PlayMPG()(COpeningUpdate.cpp)는 OpenMPG() 실패 시
// "Not Found <파일명>" MessageBox를 띄우며, 현재는 항상 실패한다.
// 자세한 내용은 참고자료/작업필요stub.md 참조.
int CAVI::OpenMPG(HWND hwnd, LPSTR szName, int w, int h) { (void)hwnd; (void)szName; (void)w; (void)h; return FALSE; }
int CAVI::OpenAVI(HWND hwnd, LPSTR szName, int w, int h) { (void)hwnd; (void)szName; (void)w; (void)h; return FALSE; }
void CAVI::Close() {}
int CAVI::Play() { return FALSE; }
void CAVI::Stop() {}
