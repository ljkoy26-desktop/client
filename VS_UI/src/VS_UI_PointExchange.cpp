// VS_UI_PointExchange.cpp
// 포인트 기반 거래소 UI 구현

#include "client_PCH.h"
#define assert(e) ((void)(e))
// macOS용으로 assert 비활성화

#pragma warning(disable:4786)

#include "VS_UI_PointExchange.h"
#include "VS_UI_Filepath.h"
#include "VS_UI_GameCommon.h"
#include "VS_UI_Item.h"
#include "VS_UI_Mouse_Pointer.h"
#include "VS_UI.h"
#include "UserOption.h"
#include "ServerInfo.h"

// 패킷용
#include "../Client/Packet/Cpackets/CGExchangeList.h"
#include "../Client/Packet/Cpackets/CGExchangeBuy.h"
#include "../Client/Packet/Gpackets/GCExchangeList.h"
#include "../Client/Packet/SocketInputStream.h"
#include "../Client/Packet/SocketOutputStream.h"

#include "MgamestringTable.H"

//-----------------------------------------------------------------------------
// C_VS_UI_POINT_EXCHANGE
//-----------------------------------------------------------------------------

C_VS_UI_POINT_EXCHANGE::C_VS_UI_POINT_EXCHANGE()
{
	m_pC_button_group = NULL;
	m_pC_scroll = NULL;

	m_currentTab = TAB_BROWSE;
	m_currentPage = 1;
	m_pageSize = LISTINGS_PER_PAGE;
	m_totalItems = 0;

	m_filterItemClass = 0xFF;  // 모든 클래스
	m_filterItemType = 0xFFFF;  // 모든 타입
	m_minPrice = 0;
	m_maxPrice = 0;

	m_pSelectedItem = NULL;
	m_pointBalance = 0;

	g_RegisterWindow(this);

	// 종족에 따라 스프라이트 팩 초기화
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:
		m_image_spk.Open(SPK_EXCHANGE_SLAYER);
		break;

	case RACE_VAMPIRE:
		m_image_spk.Open(SPK_VAMPIRE_INVENTORY);
		break;

	case RACE_OUSTERS:
		m_image_spk.Open(SPK_EXCHANGE_OUSTERS);
		break;
	}

	// 창 크기 설정
	int w_x = 30, w_y = 0;
	Set(w_x, w_y, 600, 480);

	// 버튼 그룹 초기화
	m_pC_button_group = new ButtonGroup(this);

	// 닫기 버튼 추가
	int closeBtnX = w - 40, closeBtnY = 10;
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(
		closeBtnX, closeBtnY,
		gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_CLOSE),
		gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_CLOSE),
		BUTTON_CLOSE, this, C_GLOBAL_RESOURCE::AB_BUTTON_CLOSE));

	// 새로고침 버튼 추가
	int refreshBtnX = w - 80, refreshBtnY = 10;
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(
		refreshBtnX, refreshBtnY,
		gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_HELP),
		gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_HELP),
		BUTTON_REFRESH, this, C_GLOBAL_RESOURCE::AB_BUTTON_HELP));

	// 탭 버튼 추가
	for (int i = 0; i < 4; i++)
	{
		C_VS_UI_EVENT_BUTTON* pTab = new C_VS_UI_EVENT_BUTTON(
			GetTabX(i), GetTabY(), TAB_WIDTH, TAB_HEIGHT,
			TAB_BROWSE_BTN + i, this, TAB_BROWSE_NORMAL + i * 2);
		pTab->SetClickOption(Button::RUN_WHEN_PUSH);
		m_pC_button_group->Add(pTab);
	}

	// 페이지 이동 버튼 추가
	int prevPageX = LISTING_START_X, prevPageY = h - 40;
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(
		prevPageX, prevPageY, 60, 25,
		BUTTON_PREV_PAGE, this, 0));

	int nextPageX = w - 80, nextPageY = h - 40;
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(
		nextPageX, nextPageY, 60, 25,
		BUTTON_NEXT_PAGE, this, 0));

	// 구매 확인 버튼 추가 (초기에는 숨김)
	int buyBtnX = w / 2 - 50, buyBtnY = h - 50;
	C_VS_UI_EVENT_BUTTON* pBuyBtn = new C_VS_UI_EVENT_BUTTON(
		buyBtnX, buyBtnY, 100, 30,
		BUY_CONFIRM, this, 0);
	pBuyBtn->SetClickOption(Button::RUN_WHEN_PUSHUP);
	m_pC_button_group->Add(pBuyBtn);
}

C_VS_UI_POINT_EXCHANGE::~C_VS_UI_POINT_EXCHANGE()
{
	g_UnregisterWindow(this);
	if (m_pC_button_group)
	{
		delete m_pC_button_group;
		m_pC_button_group = NULL;
	}
}

//-----------------------------------------------------------------------------
// Start
//-----------------------------------------------------------------------------

void C_VS_UI_POINT_EXCHANGE::Start()
{
	PI_Processor::Start();

	if (m_pC_button_group)
		m_pC_button_group->Init();

	AttrPin(true);
	gpC_window_manager->AppearWindow(this);

	// 초기 목록 데이터 요청
	RefreshList();
}

//-----------------------------------------------------------------------------
// Finish
//-----------------------------------------------------------------------------

void C_VS_UI_POINT_EXCHANGE::Finish()
{
	PI_Processor::Finish();

	gpC_window_manager->DisappearWindow(this);
}

//-----------------------------------------------------------------------------
// Show
//-----------------------------------------------------------------------------

void C_VS_UI_POINT_EXCHANGE::Show()
{
	// 배경과 창 프레임 그리기
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		gpC_global_resource->DrawDialogLocked(x, y, w, h);

		// 배경 이미지 그리기
		m_image_spk.BltLocked(x, y, EXCHANGE_WINDOW);

		// 탭 버튼 그리기
		DrawTabButtons();

		// 현재 탭에 따라 내용 그리기
		switch (m_currentTab)
		{
		case TAB_BROWSE:
			DrawBrowseList();
			break;

		case TAB_MY_LISTINGS:
			DrawMyListings();
			break;

		case TAB_MY_ORDERS:
			DrawMyOrders();
			break;

		case TAB_CLAIM:
			DrawClaimList();
			break;
		}

		// 페이지 정보 그리기
		DrawPageInfo();

		// 선택된 아이템 정보가 있으면 그리기
		if (m_pSelectedItem)
		{
			DrawSelectedItemInfo();
		}

		gpC_base->m_p_DDSurface_back->Unlock();
	}

	// 버튼 그리기
	if (m_pC_button_group)
		m_pC_button_group->Show();
}

//-----------------------------------------------------------------------------
// Process
//-----------------------------------------------------------------------------

void C_VS_UI_POINT_EXCHANGE::Process()
{
	// UI 업데이트와 이벤트 처리
	if (m_pC_button_group)
		m_pC_button_group->Process();
}

//-----------------------------------------------------------------------------
// IsPixel
//-----------------------------------------------------------------------------

bool C_VS_UI_POINT_EXCHANGE::IsPixel(int _x, int _y)
{
	return IsInRect(_x, _y);
}

//-----------------------------------------------------------------------------
// ShowButtonWidget (ButtonVisual 인터페이스)
//-----------------------------------------------------------------------------

void C_VS_UI_POINT_EXCHANGE::ShowButtonWidget(C_VS_UI_EVENT_BUTTON* p_button)
{
	// 버튼 위젯 그리기
	if (p_button && gpC_base->m_p_DDSurface_back->Lock())
	{
		// 전역 리소스를 사용해 버튼을 그린다
		gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(
			x + p_button->x, y + p_button->y, p_button->m_image_index);

		gpC_base->m_p_DDSurface_back->Unlock();
	}
}

//-----------------------------------------------------------------------------
// UnacquireMouseFocus
//-----------------------------------------------------------------------------

void C_VS_UI_POINT_EXCHANGE::UnacquireMouseFocus()
{
	if (m_pC_button_group)
		m_pC_button_group->UnacquireMouseFocus();

	m_pSelectedItem = NULL;
}

//-----------------------------------------------------------------------------
// 클릭
//-----------------------------------------------------------------------------

bool C_VS_UI_POINT_EXCHANGE::Click(int clickX, int clickY)
{
	// 목록 아이템을 클릭했는지 확인
	int relX = clickX - x;
	int relY = clickY - y;

	for (size_t i = 0; i < m_listingItems.size(); i++)
	{
		ExchangeListingItem& item = m_listingItems[i];
		if (relX >= item.displayX && relX < item.displayX + w - LISTING_START_X * 2 &&
			relY >= item.displayY && relY < item.displayY + LISTING_HEIGHT)
		{
			SelectItem(&item);
			return true;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// MouseControl
//-----------------------------------------------------------------------------

bool C_VS_UI_POINT_EXCHANGE::MouseControl(UINT message, int _x, int _y)
{
	// TODO: Implement mouse control for item hover, etc.
	return true;
}

//-----------------------------------------------------------------------------
// Run
//-----------------------------------------------------------------------------

void C_VS_UI_POINT_EXCHANGE::Run(id_t id)
{
	switch (id)
	{
	case BUTTON_CLOSE:
		Finish();
		break;

	case BUTTON_REFRESH:
		RefreshList();
		break;

	case BUY_CONFIRM:
		BuyItem();
		break;

	case BUTTON_PREV_PAGE:
		GoToPrevPage();
		break;

	case BUTTON_NEXT_PAGE:
		GoToNextPage();
		break;

	case TAB_BROWSE_BTN:
		SwitchTab(TAB_BROWSE);
		break;

	case TAB_MY_LISTINGS_BTN:
		SwitchTab(TAB_MY_LISTINGS);
		break;

	case TAB_MY_ORDERS_BTN:
		SwitchTab(TAB_MY_ORDERS);
		break;

	case TAB_CLAIM_BTN:
		SwitchTab(TAB_CLAIM);
		break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// 탭 동작
//-----------------------------------------------------------------------------

void C_VS_UI_POINT_EXCHANGE::SwitchTab(int tabID)
{
	m_currentTab = tabID;
	m_currentPage = 1;
	m_pSelectedItem = NULL;

	// 선택된 탭에 따라 화면 갱신
	switch (tabID)
	{
	case TAB_BROWSE:
		UpdateListings();
		break;

	case TAB_MY_LISTINGS:
		UpdateMyListings();
		break;

	case TAB_MY_ORDERS:
		UpdateMyOrders();
		break;

	case TAB_CLAIM:
		UpdateClaimList();
		break;
	}
}

void C_VS_UI_POINT_EXCHANGE::RefreshList()
{
	// 서버로 CGExchangeList 패킷 전송
	if (!g_pSocket) return;

	CGExchangeList* pPacket = new CGExchangeList();
	pPacket->setPage(m_currentPage);
	pPacket->setPageSize(m_pageSize);
	pPacket->setItemClass(m_filterItemClass);
	pPacket->setItemType(m_filterItemType);
	pPacket->setMinPrice(m_minPrice);
	pPacket->setMaxPrice(m_maxPrice);

	g_pSocket->sendPacket(pPacket);

	delete pPacket;
}

//-----------------------------------------------------------------------------
// 페이지 동작
//-----------------------------------------------------------------------------

void C_VS_UI_POINT_EXCHANGE::GoToPrevPage()
{
	if (m_currentPage > 1)
	{
		m_currentPage--;
		RefreshList();
	}
}

void C_VS_UI_POINT_EXCHANGE::GoToNextPage()
{
	int maxPage = (m_totalItems + m_pageSize - 1) / m_pageSize;
	if (m_currentPage < maxPage)
	{
		m_currentPage++;
		RefreshList();
	}
}

//-----------------------------------------------------------------------------
// 아이템 동작
//-----------------------------------------------------------------------------

void C_VS_UI_POINT_EXCHANGE::SelectItem(ExchangeListingItem* pItem)
{
	m_pSelectedItem = pItem;
}

void C_VS_UI_POINT_EXCHANGE::BuyItem()
{
	if (!m_pSelectedItem || !g_pSocket)
		return;

	// 서버로 구매 요청 전송
	CGExchangeBuy* pPacket = new CGExchangeBuy();
	pPacket->setListingID(m_pSelectedItem->listingID);

	g_pSocket->sendPacket(pPacket);

	delete pPacket;
}

void C_VS_UI_POINT_EXCHANGE::CreateListing(MItem* pItem, int price)
{
	// 서버로 목록 등록 요청 전송
	// TODO: Implement create listing packet - CGExchangeCreateListing
}

void C_VS_UI_POINT_EXCHANGE::CancelListing(ExchangeListingItem* pListing)
{
	// 서버로 등록 취소 요청 전송
	// TODO: Implement cancel packet - CGExchangeCancelListing
}

void C_VS_UI_POINT_EXCHANGE::ClaimItem(ExchangeListingItem* pClaim)
{
	// 서버로 수령 요청 전송
	// TODO: Implement claim packet - CGExchangeClaim
}

//-----------------------------------------------------------------------------
// 화면 갱신
//-----------------------------------------------------------------------------

void C_VS_UI_POINT_EXCHANGE::UpdateListings()
{
	// GCExchangeList 패킷을 수신하면 호출된다
	// 패킷 핸들러가 m_listingItems를 채운다
}

void C_VS_UI_POINT_EXCHANGE::UpdateMyListings()
{
	// TODO: Update UI with my listings
}

void C_VS_UI_POINT_EXCHANGE::UpdateMyOrders()
{
	// TODO: Update UI with my orders
}

void C_VS_UI_POINT_EXCHANGE::UpdateClaimList()
{
	// TODO: Update UI with claimable items
}

void C_VS_UI_POINT_EXCHANGE::UpdatePointBalance(int balance)
{
	m_pointBalance = balance;
}

//-----------------------------------------------------------------------------
// 레이아웃 보조 함수
//-----------------------------------------------------------------------------

int C_VS_UI_POINT_EXCHANGE::GetTabX(int tabIndex) const
{
	return LISTING_START_X + tabIndex * (TAB_WIDTH + 10);
}

int C_VS_UI_POINT_EXCHANGE::GetListingX(int index) const
{
	return LISTING_START_X;
}

int C_VS_UI_POINT_EXCHANGE::GetListingY(int index) const
{
	return LISTING_START_Y + index * LISTING_HEIGHT;
}

//-----------------------------------------------------------------------------
// 그리기 보조 함수
//-----------------------------------------------------------------------------

void C_VS_UI_POINT_EXCHANGE::DrawTabButtons()
{
	// 올바른 하이라이트 상태로 탭 버튼 그리기
	for (int i = 0; i < 4; i++)
	{
		int spriteIndex = (i == m_currentTab) ?
			TAB_BROWSE_HIGHLIGHT + i * 2 : TAB_BROWSE_NORMAL + i * 2;

		if (gpC_base->m_p_DDSurface_back->Lock())
		{
			m_image_spk.BltLocked(x + GetTabX(i), y + GetTabY(), spriteIndex);
			gpC_base->m_p_DDSurface_back->Unlock();
		}
	}
}

void C_VS_UI_POINT_EXCHANGE::DrawPageInfo()
{
	// 페이지 정보 그리기
	char pageText[64];
	sprintf(pageText, "Page %d/%d", m_currentPage,
		(m_totalItems + m_pageSize - 1) / m_pageSize);

	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		// TODO: Draw text - need font interface
		gpC_base->m_p_DDSurface_back->Unlock();
	}
}

void C_VS_UI_POINT_EXCHANGE::DrawBrowseList()
{
	// 목록 아이템 그리기
	for (size_t i = 0; i < m_listingItems.size(); i++)
	{
		DrawListingItem(m_listingItems[i], i);
	}
}

void C_VS_UI_POINT_EXCHANGE::DrawMyListings()
{
	// TODO: Draw my listings tab
	DrawBrowseList(); // 지금은 둘러보기 목록 그리기를 재사용
}

void C_VS_UI_POINT_EXCHANGE::DrawMyOrders()
{
	// TODO: Draw my orders tab
	DrawBrowseList(); // 지금은 둘러보기 목록 그리기를 재사용
}

void C_VS_UI_POINT_EXCHANGE::DrawClaimList()
{
	// TODO: Draw claim items tab
	DrawBrowseList(); // 지금은 둘러보기 목록 그리기를 재사용
}

void C_VS_UI_POINT_EXCHANGE::DrawListingItem(const ExchangeListingItem& item, int index)
{
	int itemX = x + GetListingX(index);
	int itemY = y + GetListingY(index);
	int itemW = w - LISTING_START_X * 2;
	int itemH = LISTING_HEIGHT;

	// 아이템 배경 그리기
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		// 선택됐으면 하이라이트
		if (m_pSelectedItem && m_pSelectedItem->listingID == item.listingID)
		{
			// 선택 하이라이트 그리기
			// TODO: Draw highlight rectangle
		}

		// TODO: Draw item icon, name, price, seller name
		// 아이템 스프라이트와 폰트 렌더링 접근이 필요하다

		gpC_base->m_p_DDSurface_back->Unlock();
	}
}

void C_VS_UI_POINT_EXCHANGE::DrawSelectedItemInfo()
{
	// 선택된 아이템의 상세 정보 그리기
	// TODO: Implement detailed item info display
}
