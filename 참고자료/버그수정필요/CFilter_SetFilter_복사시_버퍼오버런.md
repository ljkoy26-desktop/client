# CFilter::SetFilter(const CFilter&) 버퍼 오버런 의심

- 파일: `Client/SpriteLib/CFilter.cpp`
- 함수: `CFilter::SetFilter(const CFilter& filter)` (다른 `CFilter`를 복사해오는 버전)
- 발견 경위: `m_ppFilter`(`BYTE**`)를 리팩터링 가능한지 분석하던 중 발견

## 문제 코드

```cpp
void CFilter::SetFilter(const CFilter& filter)
{
	// size가 0이면 return
	if (filter.m_Width==0 || filter.m_Height==0)
		return;

	// memory 잡기
	Init( filter.m_Width, filter.m_Height );

	// byte수
	int width2 = m_Width << 1;   // == m_Width * 2

	for (int i=0; i<m_Height; i++)
	{
		memcpy((void*)m_ppFilter[i], (void*)filter.m_ppFilter[i], width2);
	}
}
```

## 왜 문제인가

`Init(width, height)`는 각 행(row)을 정확히 `width`바이트만 할당한다.

```cpp
void CFilter::Init(WORD width, WORD height)
{
	...
	m_ppFilter = new BYTE* [m_Height];
	for (int i=0; i<m_Height; i++)
		m_ppFilter[i] = new BYTE [m_Width];   // 딱 m_Width바이트
}
```

그런데 `SetFilter(const CFilter&)`는 `width2 = m_Width << 1` (= `m_Width * 2`)를 복사 길이로 사용한다. 즉 실제 할당된 크기(`m_Width`바이트)의 **2배**를 `memcpy`한다.

- 원본(`filter.m_ppFilter[i]`)에서 `m_Width`바이트를 초과해서 읽음 → out-of-bounds read
- 대상(`m_ppFilter[i]`)에 `m_Width`바이트를 초과해서 씀 → heap buffer overflow (out-of-bounds write)

`filter`와 `this`는 `Init()`을 통해 같은 크기(`filter.m_Width` == `m_Width`)로 맞춰지므로, 이 오버플로우는 항상 정확히 `m_Width`바이트만큼 발생한다.

## 영향 범위

- 현재 구조(행마다 개별 `new BYTE[width]`)에서는 다음에 할당된 힙 블록(다른 행의 버퍼이거나 전혀 무관한 다른 객체의 메모리)을 침범한다 → 조용한 힙 손상, 나중에 관계없어 보이는 곳에서 크래시/이상 동작으로 나타날 수 있음.
- 만약 향후 `m_ppFilter`를 단일 연속 버퍼(`BYTE*` 또는 `std::vector<BYTE>`)로 리팩터링하면, 오버런 대상이 "다음 행 데이터 자체"로 바뀔 뿐 버그 자체는 그대로 남는다. 즉 이 버그는 `m_ppFilter`의 자료구조 선택과 무관하게 별도로 고쳐야 한다.
- AddressSanitizer(`make debug-asan`)로 이 함수가 호출되는 경로를 실행하면 heap-buffer-overflow로 잡힐 가능성이 높다.

## 추정 원인

프로젝트 곳곳의 `WORD*` 서피스(픽셀당 2바이트, 예: `SetFilter(WORD* pSurface, WORD pitch, ...)`)를 복사하던 로직에서 "폭 × 2 (바이트)" 계산 관용구를 그대로 복붙하면서, `CFilter`가 실제로는 `BYTE`(1바이트) 단위 배열이라는 점을 반영하지 못한 것으로 보인다.

## 제안하는 수정 방향 (미적용, 참고용)

```cpp
// width2 = m_Width << 1;  // 삭제
for (int i=0; i<m_Height; i++)
{
	memcpy((void*)m_ppFilter[i], (void*)filter.m_ppFilter[i], m_Width);   // BYTE 1개당 1바이트이므로 그대로 m_Width
}
```

`m_ppFilter`를 단일 연속 버퍼로 리팩터링한 상태라면, 행 단위 루프 대신 전체를 한 번에 복사할 수도 있다:

```cpp
memcpy(m_pFilter, filter.m_pFilter, (size_t)m_Width * m_Height);
```

## 호출 경로 확인 필요

`CFilter::SetFilter(const CFilter&)`의 실제 호출부를 찾아 런타임에서 실제로 이 버그가 발동되는지(즉 이 오버로드가 실제로 쓰이는지) 확인 후 수정 우선순위를 정할 것. (이번 분석에서는 정적 코드 리딩만 수행했고, 실제 호출부 추적 및 수정은 하지 않았음.)
