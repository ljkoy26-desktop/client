#include "MZoneTileProvider.h"

MZoneTileProvider::MZoneTileProvider()
	: m_pZone(nullptr)
{
}

MZoneTileProvider::~MZoneTileProvider()
{
	// 참고: MZone을 소유하지 않으므로 삭제하지 않는다
}
