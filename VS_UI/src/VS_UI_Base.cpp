// VS_UI_Base.cpp

#include "client_PCH.h"
#define assert(e) ((void)(e))
// macOS를 위해 assert를 비활성화함
#include "VS_UI_Base.h"
// GDI 제거됨 (SDL2) - 모든 플랫폼은 TextSystem을 사용한다 (SDL + freetype2)
#include "../../basic/Platform.h"
#include "TextSystem/FontHandleUtil.h"

// jhlee
#include <string>
#include <array>

extern RECT g_GameRect;
//----------------------------------------------------------------------------
// 전역 변수
//----------------------------------------------------------------------------
Base * gpC_base;

//----------------------------------------------------------------------------
// 연산
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Base
//
//
//-----------------------------------------------------------------------------
Base::Base()
{
	m_event_occured = false;
	m_p_DDSurface_back = NULL;

	m_chat_dialog_pi.hfont = NULL;

//	m_slayer_font	= C_FONT_MANAGER::NO_GID;
//	m_vampire_font = C_FONT_MANAGER::NO_GID;
//	m_bbs_font		= C_FONT_MANAGER::NO_GID;
//	m_item_font		= C_FONT_MANAGER::NO_GID;
//	m_normal_font	= C_FONT_MANAGER::NO_GID;
}

//-----------------------------------------------------------------------------
// ~Base
//
// GDI 제거됨 (SDL2) - TextSystem이 폰트를 자동으로 관리하므로 수동 정리가 필요 없다
//-----------------------------------------------------------------------------
Base::~Base()
{
	// TextSystem이 폰트 생명주기를 자동으로 관리한다
	// 폰트 핸들을 직접 삭제할 필요가 없다
}

//-----------------------------------------------------------------------------
// SetFont
//
// GDI 제거됨 (SDL2) - 모든 플랫폼이 TextSystem을 사용한다
//-----------------------------------------------------------------------------
void Base::SetFont(PrintInfo &pi, LOGFONT &lf, COLORREF textcolor, COLORREF backcolor, int bk_mode, int align)
{
	// GDI 제거됨: Windows에서는 CreateFontIndirect()를 사용했었다
	// 이제는 모든 플랫폼에서 TextSystem을 사용한다
	pi.hfont = TextSystem::EncodeFontSizeHandle(lf.lfHeight);
	pi.text_color = textcolor;
	pi.back_color = backcolor;
	pi.bk_mode = bk_mode;
	pi.text_align = align;
}

//-----------------------------------------------------------------------------
// SetDefaultLogfont
//
// GDI 제거됨 (SDL2) - TextSystem(모든 플랫폼)에 맞춰 단순화함
//-----------------------------------------------------------------------------
void Base::SetDefaultLogfont(LOGFONT &lf) const
{
	// TextSystem을 위한 기본 LOGFONT 값을 설정한다
	lf.lfHeight = 0;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
	strcpy(lf.lfFaceName, "Arial");
}

//-----------------------------------------------------------------------------
// SelectFont
//
//
//-----------------------------------------------------------------------------
/*
void Base::SelectFont(font_id_t id)
{
	switch (id)
	{
		case FONT_SLAYER:
			gC_font.SelectFont(m_slayer_font);
			gC_font.NotUseNoPublicEng();
			break;

		case FONT_VAMPIRE:
			gC_font.SelectFont(m_vampire_font);
			gC_font.NotUseNoPublicEng();
			break;

		case FONT_BBS:
			gC_font.SelectFont(m_bbs_font);
			gC_font.NotUseNoPublicEng();
			break;

		case FONT_ITEM:
			gC_font.SelectFont(m_normal_font);
			gC_font.UseNoPublicEng(OLIVE);
			break;

		case FONT_ITEM_SMALL:
			gC_font.SelectFont(m_item_font);
			gC_font.NotUseNoPublicEng();
			break;

		case FONT_NORMAL:
			gC_font.SelectFont(m_normal_font);
			gC_font.NotUseNoPublicEng();
			break;
	}
}*/

//-----------------------------------------------------------------------------
// Init
//
//
//-----------------------------------------------------------------------------
void Base::Init(CSpriteSurface *surface, void (*fp)(DWORD, int, int, void *))
{
	InitSurface(surface);
	InitFont();
	m_C_ui_result_receiver.SetResultReceiver(fp);
	gC_ci->RunCursorBlinker();
}



void Base::InitFont()
{
	// 1. std::string 기반 폰트 테이블 정의
	const std::array<std::array<std::string, 2>, 4> fontTable = { {
		{ "奔覆眉",          "宋体" },      // [0]
		{ "MS Sans Serif",  "MS Sans Serif" }, // [1]
		{ "蹈框眉",          "新宋体" },    // [2]
		{ "奔覆",            "宋体" }       // [3]
	} };

	const int langIdx = gC_ci->IsChinese() ? 1 : 0;

	// 2. LOGFONT 설정 및 SetFont 호출을 전담하는 Lambda 헬퍼
	auto ApplyFont = [this, &fontTable, langIdx](
		auto& fontPi,
		int fontTypeIdx,
		int height,
		COLORREF color,
		LONG weight = FW_NORMAL,
		BYTE italic = FALSE,
		int bkMode = OPAQUE,
		UINT align = TA_LEFT)
	{
		LOGFONT lf;
		SetDefaultLogfont(lf);
		lf.lfHeight = height;
		lf.lfWeight = weight;
		lf.lfItalic = italic;

		// std::string에서 안전하게 LOGFONT 버퍼로 복사
		const std::string& fontName = fontTable[fontTypeIdx][langIdx];
		strncpy_s(lf.lfFaceName, sizeof(lf.lfFaceName), fontName.c_str(), _TRUNCATE);

		SetFont(fontPi, lf, color, 0, bkMode, align);
	};

	// 3. 폰트 생성 로직 (단 한 줄씩으로 축소)
	ApplyFont(m_small_pi, 0, 10, RGB(20, 70, 0));
	ApplyFont(m_chatting_pi, 0, 12, RGB(20, 70, 0));
	ApplyFont(m_user_id_pi, 0, 12, RGB(20, 70, 0), FW_BOLD);

	ApplyFont(m_value_pi, 1, 14, RGB(255, 255, 255), FW_BOLD, TRUE, TRANSPARENT, TA_RIGHT);
	ApplyFont(m_value2_pi, 1, 14, RGB(20, 70, 0), FW_BOLD, TRUE);

	ApplyFont(m_item_name_pi, 2, 14, RGB(255, 255, 255), FW_BOLD);
	ApplyFont(m_item_desc_pi, 0, 12, RGB(192, 192, 255));
	ApplyFont(m_dialog_menu_pi, 0, 14, RGB(255, 255, 255));
	ApplyFont(m_dialog_msg_pi, 0, 13, RGB(255, 255, 255));
	ApplyFont(m_desc_menu_pi, 0, 14, RGB(255, 255, 255), FW_BOLD);
	ApplyFont(m_desc_msg_pi, 0, 14, RGB(255, 255, 255));
	ApplyFont(m_money_pi, 0, 16, RGB(255, 255, 255));
	ApplyFont(m_char_value_pi, 0, 16, RGB(255, 255, 255), FW_BOLD);

	ApplyFont(m_info_pi, 3, 14, RGB(255, 255, 255), FW_BOLD);
	ApplyFont(m_item_pi, 3, 12, RGB(255, 255, 255));
	ApplyFont(m_char_name_pi, 3, 13, RGB(255, 255, 255), FW_BOLD);
	ApplyFont(m_char_chat_pi, 3, 13, RGB(255, 255, 255));
	ApplyFont(m_party_name_pi, 3, 12, RGB(20, 70, 0));
	ApplyFont(m_xmas_pi, 3, 10, RGB(20, 70, 0), FW_NORMAL, TRUE);
	ApplyFont(m_char_chat_large_pi, 3, 16, RGB(255, 255, 255), FW_BOLD);

	ApplyFont(m_money2_pi, 0, 16, RGB(255, 255, 255), FW_NORMAL, FALSE, TRANSPARENT, TA_RIGHT);
}


//-----------------------------------------------------------------------------
// InitFont
//-----------------------------------------------------------------------------
//void Base::InitFont()
//{
//	LOGFONT lf;
//
//	//
//	// 괜찮은 style
//	//
//	// - 바탕, 13
//	//
//	// !Times New Romans로 하지 말것. OS간 위치 안맞는 문제 발생함.
//	//
//
//	const char szFontName[4][2][20] = {
//		// 한글 폰트         중국어 폰트
//		{ "奔覆眉",			"宋体" },
//		{ "MS Sans Serif",	"MS Sans Serif" },
//		{ "蹈框眉",			"新宋体" },
//		{ "奔覆",			"宋体" }
//	};
//
//	char Language;
//
//	if(gC_ci->IsChinese())
//		Language = 1;
//	else
//		Language = 0;
//
//	SetDefaultLogfont(lf); //by larosel
//	lf.lfHeight = 10;
//	strcpy(lf.lfFaceName, szFontName[0][Language]);
//	SetFont(m_small_pi, lf, RGB(20, 70, 0));
//
//	SetDefaultLogfont(lf); //by larosel
//	lf.lfHeight = 12;
//	strcpy(lf.lfFaceName, szFontName[0][Language]);
//	SetFont(m_chatting_pi, lf, RGB(20, 70, 0));
//
//	SetDefaultLogfont(lf); //by larosel
//	lf.lfHeight = 12;
//	lf.lfWeight = FW_BOLD;
//	strcpy(lf.lfFaceName, szFontName[0][Language]);
//	SetFont(m_user_id_pi, lf, RGB(20, 70, 0));
//
//	// 새 스타일...
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 14;
//	lf.lfItalic = 1;
//	lf.lfWeight = FW_BOLD;
//	strcpy(lf.lfFaceName, szFontName[1][Language]);
//	SetFont(m_value_pi, lf, RGB(255, 255, 255), 0, TRANSPARENT, TA_RIGHT);
//
//	// 새 스타일...
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 14;
//	lf.lfItalic = 1;
//	lf.lfWeight = FW_BOLD;
//	strcpy(lf.lfFaceName, szFontName[1][Language]);
//	SetFont(m_value2_pi, lf, RGB(20, 70, 0));
//
//	// 새 스타일...
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 14;
//	lf.lfWeight = FW_BOLD;
//	strcpy(lf.lfFaceName, szFontName[2][Language]);
//	SetFont(m_item_name_pi, lf, RGB(255, 255, 255));
//
//	// 새 스타일...
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 12;
//	strcpy(lf.lfFaceName, szFontName[0][Language]);
//	SetFont(m_item_desc_pi, lf, RGB(192, 192, 255));
//
//	// 새 스타일...
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 14;
////	lf.lfWeight = FW_BOLD;
//	strcpy(lf.lfFaceName, szFontName[0][Language]);
//	SetFont(m_dialog_menu_pi, lf, RGB(255, 255, 255));
//
//	// 새 스타일...
//	SetDefaultLogfont(lf);
//	strcpy(lf.lfFaceName, szFontName[0][Language]);
//	lf.lfHeight = 13;
//	SetFont(m_dialog_msg_pi, lf, RGB(255, 255, 255));
//
//	// 새 스타일...
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 14;
//	lf.lfWeight = FW_BOLD;
//	strcpy(lf.lfFaceName, szFontName[0][Language]);
//	SetFont(m_desc_menu_pi, lf, RGB(255, 255, 255));
//
//	// 새 스타일...
//	SetDefaultLogfont(lf);
//	strcpy(lf.lfFaceName, szFontName[0][Language]);
//	lf.lfHeight = 14;
//	SetFont(m_desc_msg_pi, lf, RGB(255, 255, 255));
//
//	// 새 스타일...
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 16;
//	strcpy(lf.lfFaceName, szFontName[0][Language]);
//	SetFont(m_money_pi, lf, RGB(255, 255, 255));
//
//	// 새 스타일...
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 16;
//	lf.lfWeight = FW_BOLD;
//	strcpy(lf.lfFaceName, szFontName[0][Language]);
//	SetFont(m_char_value_pi, lf, RGB(255, 255, 255));
//
//	//
//	//Client가 사용하는 폰트 by larosel 밖의 폰트보다 2씩 크게 해줘야함.
//	//
//	//SetDefaultLogfont(lf);
//	//lf.lfHeight = 12;
//	//strcpy(lf.lfFaceName, "굴림");
//	//SetFont(m_chat_dialog_pi, lf, RGB(255, 255, 255));
//
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 14;
//	lf.lfWeight = FW_BOLD;
//	strcpy(lf.lfFaceName, szFontName[3][Language]);
//	SetFont(m_info_pi, lf, RGB(255, 255, 255));
//
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 12;
//	strcpy(lf.lfFaceName, szFontName[3][Language]);
//	SetFont(m_item_pi, lf, RGB(255, 255, 255));
//
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 13;
//	lf.lfWeight = FW_BOLD;
//	strcpy(lf.lfFaceName, szFontName[3][Language]);
//	SetFont(m_char_name_pi, lf, RGB(255, 255, 255));
//
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 13;
//	strcpy(lf.lfFaceName, szFontName[3][Language]);
//	SetFont(m_char_chat_pi, lf, RGB(255, 255, 255));
//
//	//파티
//	SetDefaultLogfont(lf); //by larosel
//	lf.lfHeight = 12;
////	lf.lfWeight = FW_BOLD;
//	strcpy(lf.lfFaceName, szFontName[3][Language]);
//	SetFont(m_party_name_pi, lf, RGB(20, 70, 0));
//
//	//크리스마스
//	SetDefaultLogfont(lf); //by larosel
//	lf.lfHeight = 10;
////	lf.lfWeight = FW_BOLD;
//	lf.lfItalic = true;
//	strcpy(lf.lfFaceName, szFontName[3][Language]);
//	SetFont(m_xmas_pi, lf, RGB(20, 70, 0));
//
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 16;
//	lf.lfWeight = FW_BOLD;
//	strcpy(lf.lfFaceName, szFontName[3][Language]);
//	SetFont( m_char_chat_large_pi, lf, RGB(255,255,255));
//
//	// 새 스타일...
//	SetDefaultLogfont(lf);
//	lf.lfHeight = 16;
//	strcpy(lf.lfFaceName, szFontName[0][Language]);
//	SetFont(m_money2_pi, lf, RGB(255, 255, 255), 0, TRANSPARENT, TA_RIGHT);
//
//
//}

//-----------------------------------------------------------------------------
// InitSurface
//
//
//-----------------------------------------------------------------------------
void Base::InitSurface(CSpriteSurface *surface)
{
	assert(surface);
	m_p_DDSurface_back = surface;

	bool ret = m_DDSurface_offscreen.InitOffsurface(g_GameRect.right, g_GameRect.bottom);
	if (!ret)
		_Error(FAILED_JOB);

	//
	// offscreen의 colorkey를 설정한다. 상황에 따라서 Image에서 안 쓰는 색을
	// 결정해야 한다.
	//
	m_colorkey_red = RED;

	m_DDSurface_offscreen.SetTransparency(m_colorkey_red); // 기본 colorkey = red

	// GDI 제거됨 (SDL2) - 모든 플랫폼이 SDL 백엔드를 사용한다: 텍스트 렌더링에 sprite surface를 직접 사용한다
	g_SetFL2Surface(m_p_DDSurface_back);
}

//-----------------------------------------------------------------------------
// DispatchMessage
//
//
//-----------------------------------------------------------------------------
void Base::DispatchMessage()
{
	m_C_ui_result_receiver._DispatchMessage();
}

//-----------------------------------------------------------------------------
// SendMessage
//
//
//-----------------------------------------------------------------------------
void Base::SendMessage(DWORD message, int left, int right, void * void_ptr)
{
	m_C_ui_result_receiver._SendMessage(message, left, right, void_ptr);
}

//-----------------------------------------------------------------------------
// StartEvent
//
//
//-----------------------------------------------------------------------------
void	Base::StartEvent()
{
	//assert(m_event_occured == false);

	m_event_occured = true;
}

//-----------------------------------------------------------------------------
// FinishEvent
//
//
//-----------------------------------------------------------------------------
void	Base::FinishEvent()
{
	//assert(m_event_occured == true);

	m_event_occured = false;
}
