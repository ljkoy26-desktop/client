/*-----------------------------------------------------------------------------

	CSpriteSurface_Adapter.cpp

	CSpriteSurface를 위한 SDL2 백엔드 어댑터 구현부.
	이 파일은 SpriteLibBackend를 사용하여 구현된 모든 BltSprite* 메소드를 포함한다.

	NOTE: 이 파일은 CSpriteSurface_SDL.cpp 에서 include 된다.
	      별도로 컴파일하지 말 것.

	2025.01.14

-----------------------------------------------------------------------------*/

/* 여기서는 #ifdef SPRITELIB_BACKEND_SDL 를 쓰지 않는다 - SDL 전용 파일에서만 include 되기 때문 */

#include "client_PCH.h"
#include "CSprite.h"
#include "CAlphaSprite.h"
#include "CIndexSprite.h"
#include "CShadowSprite.h"
#include "CSpriteSurface.h"
#include "CFilter.h"

#include "SpriteLibBackend.h"
#include "DebugLog.h"

/* ============================================================================
 * 디버그 설정
 * ============================================================================ */

// Sprite 어댑터의 상세 디버그 로그 활성화
#ifndef SPRITE_ADAPTER_DEBUG
#define SPRITE_ADAPTER_DEBUG 0
#endif

// 백엔드 스프라이트 생명주기 추적 활성화 
#ifndef SPRITE_ADAPTER_DEBUG_LIFECYCLE
#define SPRITE_ADAPTER_DEBUG_LIFECYCLE 0
#endif

#if SPRITE_ADAPTER_DEBUG
#define SA_DEBUG(fmt, ...) \
	fprintf(stderr, "[SpriteAdapter] %s:%d: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define SA_DEBUG(fmt, ...) do {} while(0)
#endif

#if SPRITE_ADAPTER_DEBUG_LIFECYCLE
#define SA_DEBUG_LIFECYCLE(fmt, ...) \
	fprintf(stderr, "[SpriteAdapter LIFECYCLE] %s:%d: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define SA_DEBUG_LIFECYCLE(fmt, ...) do {} while(0)
#endif

/* 에러 로깅 - 항상 활성화 */
#define SA_DEBUG_ERROR(fmt, ...) \
	fprintf(stderr, "[SpriteAdapter ERROR] %s:%d: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

/* ============================================================================
 * 헬퍼 함수
 * ============================================================================ */

/**
 * CSprite로부터 백엔드 스프라이트를 가져오거나 생성한다
 * 지연 생성(lazy creation)과 동기화를 처리한다
 */
static spritectl_sprite_t get_backend_sprite(CSprite* pSprite)
{
	if (!pSprite || !pSprite->IsInit()) {
		static int notInitCount = 0;
		if (notInitCount < 3) {
			printf("[get_backend_sprite] ERROR: pSprite=%p, IsInit=%d\n", pSprite, pSprite ? pSprite->IsInit() : 0);
			notInitCount++;
		}
		return SPRITECTL_INVALID_SPRITE;
	}

	/* 지연 생성: 백엔드 스프라이트가 없으면 생성한다 */
	if (pSprite->GetBackendSprite() == SPRITECTL_INVALID_SPRITE) {
		WORD width = pSprite->GetWidth();
		WORD height = pSprite->GetHeight();

		/* 올바른 투명 처리를 위해 RLE 데이터로 백엔드 스프라이트를 생성한다 */
		spritectl_sprite_t new_sprite = spritectl_create_sprite_rle(width, height);
		if (!new_sprite) {
			return SPRITECTL_INVALID_SPRITE;
		}

		/* CSprite의 RLE 데이터를 백엔드 스프라이트로 복사한다 */
		for (WORD y = 0; y < height; y++) {
			WORD* src_line = pSprite->GetPixelLine(y);
			if (!src_line) {
				SA_DEBUG_ERROR("get_backend_sprite: Invalid scanline at y=%d", y);
				spritectl_destroy_sprite(new_sprite);
				return SPRITECTL_INVALID_SPRITE;
			}

			/* 범위 검사를 포함하여 RLE 데이터 크기를 얻는다 */
			WORD* pSrc = src_line;
			WORD* pSrcStart = src_line;  /* 검증을 위해 시작 위치를 기억해 둔다 */

			int count = *pSrc++;  /* run의 개수 */

			/* 무한 루프나 메모리 손상을 막기 위해 count를 검증한다 */
			if (count < 0 || count > 16384) {  /* 임의로 정한 합리적인 상한값 */
				SA_DEBUG_ERROR("get_backend_sprite: Invalid RLE count=%d at y=%d", count, y);
				spritectl_destroy_sprite(new_sprite);
				return SPRITECTL_INVALID_SPRITE;
			}

			/* 범위 검사를 포함하여 전체 RLE 데이터 크기(count + run 데이터)를 계산한다 */
			int rle_size = 1;  /* count 바이트 */
			if (count > 0) {
				for (int j = 0; j < count; j++) {
					/* transCount와 colorCount를 읽을 만큼 데이터가 남아있는지 확인한다 */
					if ((pSrc - pSrcStart) > width * 2) {  /* 안전 검사 */
						SA_DEBUG_ERROR("get_backend_sprite: RLE data exceeds bounds at segment %d, y=%d", j, y);
						spritectl_destroy_sprite(new_sprite);
						return SPRITECTL_INVALID_SPRITE;
					}

					pSrc++;  /* transCount 건너뛰기 */
					int colorCount = *pSrc++;  /* 색상 픽셀 개수 */

					/* colorCount를 검증한다 */
					if (colorCount < 0 || colorCount > width) {
						SA_DEBUG_ERROR("get_backend_sprite: Invalid colorCount=%d at segment %d, y=%d", colorCount, j, y);
						spritectl_destroy_sprite(new_sprite);
						return SPRITECTL_INVALID_SPRITE;
					}

					rle_size += 2 + colorCount;  /* trans + color 개수 + 픽셀 데이터 */
					pSrc += colorCount;
				}
			}

			/* 최종 rle_size를 검증한다 */
			if (rle_size <= 0 || rle_size > 65535) {
				SA_DEBUG_ERROR("get_backend_sprite: Invalid rle_size=%d at y=%d", rle_size, y);
				spritectl_destroy_sprite(new_sprite);
				return SPRITECTL_INVALID_SPRITE;
			}

			/* 헬퍼 함수를 사용해 RLE 데이터를 설정한다 */
			if (rle_size > 1) {  /* 비어있지 않은 스캔라인 */
				if (spritectl_sprite_set_scanline_rle(new_sprite, y, src_line, rle_size) != 0) {
					/* 정리 후 폴백 처리 */
					spritectl_destroy_sprite(new_sprite);
					return SPRITECTL_INVALID_SPRITE;
				}
			}
		}

		pSprite->SetBackendSprite(new_sprite);
		pSprite->SetBackendDirty(false);
	}
	/* dirty 상태면 동기화한다 */
	else if (pSprite->IsBackendDirty()) {
		/* 기존 스프라이트를 파괴하고 다시 생성한다 */
		spritectl_destroy_sprite(pSprite->GetBackendSprite());
		pSprite->SetBackendSprite(SPRITECTL_INVALID_SPRITE);

		/* 재생성 (다음 호출에서 생성될 것이다) */
		return get_backend_sprite(pSprite);
	}

	return pSprite->GetBackendSprite();
}

/**
 * CAlphaSprite로부터 백엔드 스프라이트를 가져오거나 생성한다
 * 지연 생성(lazy creation)과 동기화를 처리한다
 * NOTE: CAlphaSprite는 알파 채널이 포함된 RLE 압축을 사용한다
 */
static spritectl_sprite_t get_backend_alpha_sprite(CAlphaSprite* pSprite)
{
	if (!pSprite || !pSprite->IsInit()) {
		return SPRITECTL_INVALID_SPRITE;
	}

	/* 지연 생성: 백엔드 스프라이트가 없으면 생성한다 */
	if (pSprite->GetBackendSprite() == SPRITECTL_INVALID_SPRITE) {
		WORD width = pSprite->GetWidth();
		WORD height = pSprite->GetHeight();

		size_t pixel_count = width * height;
		size_t data_size = pixel_count * sizeof(WORD);

		/* 픽셀 데이터를 할당하고 압축을 해제한다 */
		WORD* pixels = (WORD*)malloc(data_size);
		if (!pixels) {
			return SPRITECTL_INVALID_SPRITE;
		}

		/* RLE 포맷을 원본 픽셀로 압축 해제한다 */
		/* 투명 색상으로 초기화한다 */
		WORD colorkey = CAlphaSprite::GetColorkey();
		for (size_t i = 0; i < pixel_count; i++) {
			pixels[i] = colorkey;
		}

		/* 라인별로 압축을 해제한다 */
		for (WORD y = 0; y < height; y++) {
			WORD* pPixels = pSprite->GetPixelLine(y);
			WORD* dst_line = pixels + (y * width);

			int count = *pPixels++;  // RLE run 개수
			int x = 0;

			if (count > 0) {
				for (int i = 0; i < count; i++) {
					int transCount = *pPixels++;   // 투명 픽셀 개수
					int colorCount = *pPixels++;   // 색상 픽셀 개수

					x += transCount;  // 투명 픽셀은 건너뛴다

					/* 알파를 포함한 색상 픽셀을 복사한다 */
					for (int j = 0; j < colorCount; j++) {
						WORD alpha2 = *pPixels++;  // 알파 값
						WORD color = *pPixels++;   // 색상 값

						if (x < width) {
							/* 프리멀티플라이드 알파 등의 포맷으로 저장 */
							/* 지금은 색상 값을 그대로 저장한다 */
							dst_line[x] = color;
						}
						x++;
					}
				}
			}
		}

		/* 백엔드 스프라이트를 생성한다 */
		spritectl_sprite_t new_sprite = spritectl_create_sprite(
			width, height, SPRITECTL_FORMAT_RGB565,
			pixels, data_size);

		free(pixels);
		pSprite->SetBackendSprite(new_sprite);
		pSprite->SetBackendDirty(false);
	}
	/* dirty 상태면 동기화한다 */
	else if (pSprite->IsBackendDirty()) {
		/* 기존 스프라이트를 파괴하고 다시 생성한다 */
		spritectl_destroy_sprite(pSprite->GetBackendSprite());
		pSprite->SetBackendSprite(SPRITECTL_INVALID_SPRITE);

		/* 재생성 (다음 호출에서 생성될 것이다) */
		return get_backend_alpha_sprite(pSprite);
	}

	return pSprite->GetBackendSprite();
}

/**
 * CShadowSprite로부터 백엔드 스프라이트를 가져오거나 생성한다
 * 지연 생성(lazy creation)과 동기화를 처리한다
 */
static spritectl_sprite_t get_backend_shadow_sprite(CShadowSprite* pSprite)
{
	SA_DEBUG_LIFECYCLE("get_backend_shadow_sprite: pSprite=%p, IsInit=%d",
	                   (void*)pSprite, pSprite ? pSprite->IsInit() : 0);

	if (!pSprite || !pSprite->IsInit()) {
		return SPRITECTL_INVALID_SPRITE;
	}

	/* 지연 생성: 백엔드 스프라이트가 없으면 생성한다 */
	if (pSprite->GetBackendSprite() == SPRITECTL_INVALID_SPRITE) {
		WORD width = pSprite->GetWidth();
		WORD height = pSprite->GetHeight();

		size_t pixel_count = width * height;
		size_t data_size = pixel_count * sizeof(WORD);

		SA_DEBUG_LIFECYCLE("get_backend_shadow_sprite: Creating backend sprite, size=%dx%d (%zu pixels, %zu bytes)",
		                   width, height, pixel_count, data_size);

		/* 픽셀 데이터를 할당하고 디코딩한다 */
		WORD* pixels = (WORD*)malloc(data_size);
		if (!pixels) {
			SA_DEBUG_LIFECYCLE("get_backend_shadow_sprite: Failed to allocate pixel buffer");
			return SPRITECTL_INVALID_SPRITE;
		}

		SA_DEBUG_LIFECYCLE("get_backend_shadow_sprite: Allocated temp pixels=%p", (void*)pixels);

		memset(pixels, 0, data_size);
		pSprite->Blt(pixels, width * sizeof(WORD));

		/* 백엔드 스프라이트를 생성한다 */
		spritectl_sprite_t new_sprite = spritectl_create_sprite(
			width, height, SPRITECTL_FORMAT_RGB565,
			pixels, data_size);

		SA_DEBUG_LIFECYCLE("get_backend_shadow_sprite: Created backend sprite=%p from temp pixels=%p",
		                   (void*)new_sprite, (void*)pixels);

		// 스프라이트 생성 후(데이터가 복사된 뒤)에 임시 픽셀 버퍼를 해제한다
		free(pixels);
		pixels = NULL;  // 댕글링 포인터 방지

		pSprite->SetBackendSprite(new_sprite);
		pSprite->SetBackendDirty(false);

		SA_DEBUG_LIFECYCLE("get_backend_shadow_sprite: Set backend sprite=%p for CShadowSprite=%p",
		                   (void*)new_sprite, (void*)pSprite);
	}
	/* dirty 상태면 동기화한다 */
	else if (pSprite->IsBackendDirty()) {
		SA_DEBUG_LIFECYCLE("get_backend_shadow_sprite: Backend dirty, destroying old sprite=%p",
		                   (void*)pSprite->GetBackendSprite());
		/* 기존 스프라이트를 파괴하고 다시 생성한다 */
		spritectl_destroy_sprite(pSprite->GetBackendSprite());
		pSprite->SetBackendSprite(SPRITECTL_INVALID_SPRITE);

		/* 재생성 (다음 호출에서 생성될 것이다) */
		return get_backend_shadow_sprite(pSprite);
	}
	else {
		SA_DEBUG_LIFECYCLE("get_backend_shadow_sprite: Reusing existing backend sprite=%p",
		                   (void*)pSprite->GetBackendSprite());
	}

	return pSprite->GetBackendSprite();
}

/**
 * CIndexSprite로부터 백엔드 스프라이트를 가져오거나 생성한다
 * 지연 생성(lazy creation)과 동기화를 처리한다
 */
static spritectl_sprite_t get_backend_index_sprite(CIndexSprite* pSprite)
{
	if (!pSprite || !pSprite->IsInit()) {
		return SPRITECTL_INVALID_SPRITE;
	}

	/* 지연 생성: 백엔드 스프라이트가 없으면 생성한다 */
	if (pSprite->GetBackendSprite() == SPRITECTL_INVALID_SPRITE) {
		WORD width = pSprite->GetWidth();
		WORD height = pSprite->GetHeight();

		size_t pixel_count = width * height;
		size_t data_size = pixel_count * sizeof(WORD);

		/* 픽셀 데이터를 할당하고 디코딩한다 (인덱스 스프라이트는 RLE로 압축되어 있다) */
		WORD* pixels = (WORD*)malloc(data_size);
		if (!pixels) {
			return SPRITECTL_INVALID_SPRITE;
		}
		memset(pixels, 0, data_size);
		pSprite->Blt(pixels, width * sizeof(WORD));

		/* 백엔드 스프라이트를 생성한다 */
		spritectl_sprite_t new_sprite = spritectl_create_sprite(
			width, height, SPRITECTL_FORMAT_RGB565,
			pixels, data_size);

		free(pixels);
		pSprite->SetBackendSprite(new_sprite);
		pSprite->SetBackendDirty(false);
	}
	/* dirty 상태면 동기화한다 */
	else if (pSprite->IsBackendDirty()) {
		/* 기존 스프라이트를 파괴하고 다시 생성한다 */
		spritectl_destroy_sprite(pSprite->GetBackendSprite());
		pSprite->SetBackendSprite(SPRITECTL_INVALID_SPRITE);

		/* 재생성 (다음 호출에서 생성될 것이다) */
		return get_backend_index_sprite(pSprite);
	}

	return pSprite->GetBackendSprite();
}

/* ============================================================================
 * BltSprite 메소드
 * ============================================================================ */

void CSpriteSurface::BltSprite(POINT* pPoint, CSprite* pSprite) {
	if (!pPoint || !pSprite) {
		return;
	}

	/* 백엔드 스프라이트를 가져온다 */
	spritectl_sprite_t backend_sprite = get_backend_sprite(pSprite);
	if (!backend_sprite) {
		LOG_WARN("[BltSprite] ERROR: get_backend_sprite returned invalid sprite! IsInit=%d\n", pSprite->IsInit());
		return;
	}

	/* 백엔드 서피스에 블릿한다 */
	int flags = 0;
	int alpha = 255;
	spritectl_blt_sprite(m_backend_surface, pPoint->x, pPoint->y,
	                    backend_sprite, flags, alpha);
}

void CSpriteSurface::BltSpriteNoClip(POINT* pPoint, CSprite* pSprite) {
	/* 현재는 BltSprite와 동일하게 처리한다 */
	BltSprite(pPoint, pSprite);
}

void CSpriteSurface::BltSpriteHalf(POINT* pPoint, CSprite* pSprite) {
	if (!pPoint || !pSprite) {
		return;
	}

	/* 백엔드 스프라이트를 가져온다 */
	spritectl_sprite_t backend_sprite = get_backend_sprite(pSprite);
	if (!backend_sprite) {
		return;
	}

	/* 배율: 128 = 0.5배 */
	int scale = 128;
	int flags = 0;
	spritectl_blt_sprite_scaled(m_backend_surface, pPoint->x, pPoint->y,
	                            backend_sprite, scale, flags);
}

void CSpriteSurface::BltSpriteAlpha(POINT* pPoint, CSprite* pSprite, BYTE alphaDepth) {
	if (!pPoint || !pSprite) {
		return;
	}

	/* 백엔드 스프라이트를 가져온다 */
	spritectl_sprite_t backend_sprite = get_backend_sprite(pSprite);
	if (!backend_sprite) {
		return;
	}

	/* 알파를 적용하여 블릿한다 */
	int flags = SPRITECTL_BLT_ALPHA;
	spritectl_blt_sprite(m_backend_surface, pPoint->x, pPoint->y,
	                    backend_sprite, flags, alphaDepth);
}

void CSpriteSurface::BltSpriteScale(POINT* pPoint, CSprite* pSprite, int scale) {
	if (!pPoint || !pSprite) {
		return;
	}

	/* 백엔드 스프라이트를 가져온다 */
	spritectl_sprite_t backend_sprite = get_backend_sprite(pSprite);
	if (!backend_sprite) {
		return;
	}

	/* 배율 파라미터: 256 = 1배, 128 = 0.5배, 512 = 2배 */
	int scale_factor = scale;
	int flags = 0;
	spritectl_blt_sprite_scaled(m_backend_surface, pPoint->x, pPoint->y,
	                            backend_sprite, scale_factor, flags);
}

/* ============================================================================
 * 그 외 BltSprite 변형 함수들의 스텁 구현
 * 추후 반복 작업에서 구현될 예정이다
 * ============================================================================ */

void CSpriteSurface::BltSpriteColor(POINT* pPoint, CSprite* pSprite, BYTE rgb) {
	/* TODO: 색상 틴트 구현 */
	BltSprite(pPoint, pSprite);
}

void CSpriteSurface::BltSpriteDarkness(POINT* pPoint, CSprite* pSprite, BYTE DarkBits) {
	/* TODO: 어둡게 효과 구현 */
	BltSprite(pPoint, pSprite);
}

void CSpriteSurface::BltSpriteColorSet(POINT* pPoint, CSprite* pSprite, WORD colorSet) {
	/* TODO: 색상 세트 구현 */
	BltSprite(pPoint, pSprite);
}

void CSpriteSurface::BltSpriteEffect(POINT* pPoint, CSprite* pSprite) {
	/* TODO: 이펙트 구현 */
	BltSprite(pPoint, pSprite);
}

void CSpriteSurface::BltSpriteAlpha4444SmallNotTrans(POINT* pPoint, CSprite* pSprite, BYTE alpha, BYTE shift) {
	/* TODO: 구현 필요 */
	BltSpriteAlpha(pPoint, pSprite, alpha);
}

void CSpriteSurface::BltSpriteAlpha4444NotTrans(POINT* pPoint, CSprite* pSprite, BYTE alpha) {
	/* TODO: 구현 필요 */
	BltSpriteAlpha(pPoint, pSprite, alpha);
}

void CSpriteSurface::BltSprite1555SmallNotTrans(POINT* pPoint, CSprite* pSprite, BYTE shift) {
	/* TODO: 구현 필요 */
	BltSprite(pPoint, pSprite);
}

void CSpriteSurface::BltSprite1555NotTrans(POINT* pPoint, CSprite* pSprite) {
	/* TODO: 구현 필요 */
	BltSprite(pPoint, pSprite);
}

void CSpriteSurface::BltSpritePalEffect(POINT* pPoint, CSpritePal* pSprite, MPalette &pal) {
	/* TODO: 팔레트 이펙트 구현 */
}

void CSpriteSurface::BltSpritePal1555SmallNotTrans(POINT* pPoint, CSpritePal* pSprite, BYTE shift, MPalette &pal) {
	/* TODO: 구현 필요 */
}

void CSpriteSurface::BltSpritePal1555NotTrans(POINT* pPoint, CSpritePal* pSprite, MPalette &pal) {
	/* TODO: 구현 필요 */
}

void CSpriteSurface::BltSpriteAlphaFilter(POINT* pPoint, CSprite* pSprite) {
	/* TODO: 알파 필터 구현 */
	BltSprite(pPoint, pSprite);
}

void CSpriteSurface::BltSpriteAlphaFilterDarkness(POINT* pPoint, CSprite* pSprite, BYTE DarkBits) {
	/* TODO: 구현 필요 */
	BltSpriteDarkness(pPoint, pSprite, DarkBits);
}

void CSpriteSurface::BltSpriteDarkerFilter(POINT* pPoint, CSprite* pSprite) {
	/* TODO: 더 어둡게 필터 구현 */
	BltSprite(pPoint, pSprite);
}

/* ============================================================================
 * Alpha Sprite 메소드
 * ============================================================================ */

void CSpriteSurface::BltAlphaSprite(POINT* pPoint, CAlphaSprite* pSprite) {
	if (!pPoint || !pSprite) {
		return;
	}

	/* 백엔드 스프라이트를 가져온다 */
	spritectl_sprite_t backend_sprite = get_backend_alpha_sprite(pSprite);
	if (!backend_sprite) {
		return;
	}

	/* 백엔드 서피스에 블릿한다 */
	int flags = SPRITECTL_BLT_ALPHA;
	int alpha = 255;
	spritectl_blt_sprite(m_backend_surface, pPoint->x, pPoint->y,
	                    backend_sprite, flags, alpha);
}

void CSpriteSurface::BltAlphaSpriteAlpha(POINT* pPoint, CAlphaSprite* pSprite, BYTE alpha) {
	if (!pPoint || !pSprite) {
		return;
	}

	/* 백엔드 스프라이트를 가져온다 */
	spritectl_sprite_t backend_sprite = get_backend_alpha_sprite(pSprite);
	if (!backend_sprite) {
		return;
	}

	/* 알파를 적용하여 백엔드 서피스에 블릿한다 */
	int flags = SPRITECTL_BLT_ALPHA;
	spritectl_blt_sprite(m_backend_surface, pPoint->x, pPoint->y,
	                    backend_sprite, flags, alpha);
}

void CSpriteSurface::BltAlphaSprite4444(POINT* pPoint, CAlphaSprite* pSprite) {
	/* TODO: 4444 포맷 변환 구현 */
	/* 지금은 일반 블릿을 사용한다 */
	BltAlphaSprite(pPoint, pSprite);
}

void CSpriteSurface::BltAlphaSprite4444NotTrans(POINT* pPoint, CAlphaSprite* pSprite) {
	/* TODO: 4444 NotTrans 구현 */
	BltAlphaSprite(pPoint, pSprite);
}

void CSpriteSurface::BltAlphaSprite4444SmallNotTrans(POINT* pPoint, CAlphaSprite* pSprite, BYTE shift) {
	/* TODO: 스케일링 구현 */
	BltAlphaSprite(pPoint, pSprite);
}

void CSpriteSurface::BltAlphaSpritePal(POINT* pPoint, CAlphaSpritePal* pSprite, MPalette &pal) {
	if (!pPoint || !pSprite) {
		LOG_ERROR("[BltAlphaSpritePal] ERROR: Invalid parameters (pPoint=%p, pSprite=%p)\n", pPoint, pSprite);
		return;
	}

	/* 스프라이트가 초기화되었는지 확인한다 */
	if (pSprite->IsNotInit()) {
		LOG_ERROR("[BltAlphaSpritePal] ERROR: Sprite not initialized\n");
		return;
	}

	/* 기본 클리핑 검사 - 서피스 완전히 바깥이면 건너뛴다 */
	int spriteWidth = pSprite->GetWidth();
	int spriteHeight = pSprite->GetHeight();

	/* 서피스 크기를 가져온다 */
	int surfaceWidth = m_width;
	int surfaceHeight = m_height;

	/* 스프라이트가 서피스 완전히 바깥에 있는지 확인한다 */
	bool outsideBounds = (pPoint->x >= surfaceWidth) ||
	                     (pPoint->y >= surfaceHeight) ||
	                     (pPoint->x + spriteWidth <= 0) ||
	                     (pPoint->y + spriteHeight <= 0);

	if (outsideBounds) {
		/* 스프라이트가 완전히 바깥에 있으므로 렌더링을 건너뛴다 */
		static int skipCount = 0;
		if (skipCount < 5) {
			LOG_WARN("[BltAlphaSpritePal] WARNING: Sprite at (%d,%d) size=%dx%d outside surface %dx%d, skipping\n",
			       pPoint->x, pPoint->y, spriteWidth, spriteHeight, surfaceWidth, surfaceHeight);
			skipCount++;
		}
		return;
	}

	/* 추가 검사: 스프라이트가 서피스 경계 바깥에서 시작하면 지금은 건너뛴다 */
	/* TODO: 제대로 된 부분 클리핑 구현 */
	if (pPoint->x < 0 || pPoint->y < 0 ||
	    pPoint->x + spriteWidth > surfaceWidth ||
	    pPoint->y + spriteHeight > surfaceHeight) {
		static int partialCount = 0;
		if (partialCount < 5) {
			LOG_WARN("[BltAlphaSpritePal] WARNING: Partial clipping at (%d,%d) size=%dx%d, skipping (TODO: implement)\n",
			       pPoint->x, pPoint->y, spriteWidth, spriteHeight);
			partialCount++;
		}
		return;
	}

	/* 픽셀에 직접 접근하기 위해 백엔드 서피스를 잠근다 */
	spritectl_surface_info_t surface_info;
	if (spritectl_lock_surface(m_backend_surface, &surface_info) != 0) {
		static int lockFailCount = 0;
		if (lockFailCount < 3) {
			LOG_WARN("[BltAlphaSpritePal] ERROR: Failed to lock surface\n");
			lockFailCount++;
		}
		return;
	}

	/* 픽셀 포인터와 피치를 가져온다 (피치는 Windows와 마찬가지로 바이트 단위) */
	WORD* pixels = (WORD*)surface_info.pixels;
	int pitch = surface_info.pitch;

	/* 오프셋을 적용해 목적지 포인터를 계산한다 */
	WORD* pDest = (WORD*)((BYTE*)pixels + pPoint->y * pitch + (pPoint->x << 1));

	/* 팔레트를 적용해 렌더링하도록 스프라이트의 Blt 메소드를 호출한다 */
	/* 피치를 바이트 단위로 전달한다 (Windows 버전과 동일) */
	/* TODO: 부분적으로만 보이는 스프라이트에 대한 제대로 된 클리핑 구현 */
	pSprite->Blt(pDest, pitch, pal);

	/* 서피스 잠금을 해제한다 */
	spritectl_unlock_surface(m_backend_surface);
}

void CSpriteSurface::BltAlphaSpritePalAlpha(POINT* pPoint, CAlphaSpritePal* pSprite, BYTE alpha, MPalette &pal) {
	/* TODO: 구현 필요 */
}

void CSpriteSurface::BltAlphaSpritePal4444(POINT* pPoint, CAlphaSpritePal* pSprite, MPalette &pal) {
	/* TODO: 구현 필요 */
}

void CSpriteSurface::BltAlphaSpritePal4444NotTrans(POINT* pPoint, CAlphaSpritePal* pSprite, MPalette &pal) {
	/* TODO: 구현 필요 */
}

void CSpriteSurface::BltAlphaSpritePal4444SmallNotTrans(POINT* pPoint, CAlphaSpritePal* pSprite, BYTE shift, MPalette &pal) {
	/* TODO: 구현 필요 */
}

/* ============================================================================
 * Index Sprite 메소드
 * ============================================================================ */

void CSpriteSurface::BltIndexSprite(POINT* pPoint, CIndexSprite* pSprite) {
	if (!pPoint || !pSprite) {
		return;
	}

	/* 백엔드 스프라이트를 가져온다 */
	spritectl_sprite_t backend_sprite = get_backend_index_sprite(pSprite);
	if (!backend_sprite) {
		return;
	}

	/* 백엔드 서피스에 블릿한다 */
	int flags = 0;
	int alpha = 255;
	spritectl_blt_sprite(m_backend_surface, pPoint->x, pPoint->y,
	                    backend_sprite, flags, alpha);
}

void CSpriteSurface::BltIndexSpriteDarkness(POINT* pPoint, CIndexSprite* pSprite, BYTE DarkBits) {
	/* TODO: 어둡게 효과 구현 */
	BltIndexSprite(pPoint, pSprite);
}

void CSpriteSurface::BltIndexSpriteAlpha(POINT* pPoint, CIndexSprite* pSprite, BYTE alpha) {
	if (!pPoint || !pSprite) {
		return;
	}

	/* 백엔드 스프라이트를 가져온다 */
	spritectl_sprite_t backend_sprite = get_backend_index_sprite(pSprite);
	if (!backend_sprite) {
		return;
	}

	/* 알파를 적용하여 백엔드 서피스에 블릿한다 */
	int flags = SPRITECTL_BLT_ALPHA;
	spritectl_blt_sprite(m_backend_surface, pPoint->x, pPoint->y,
	                    backend_sprite, flags, alpha);
}

void CSpriteSurface::BltIndexSpriteColor(POINT* pPoint, CIndexSprite* pSprite, BYTE rgb) {
	/* TODO: 색상 틴트 구현 */
	BltIndexSprite(pPoint, pSprite);
}

void CSpriteSurface::BltIndexSpriteColorSet(POINT* pPoint, CIndexSprite* pSprite, WORD colorSet) {
	/* TODO: 색상 세트 구현 */
	BltIndexSprite(pPoint, pSprite);
}

void CSpriteSurface::BltIndexSpriteEffect(POINT* pPoint, CIndexSprite* pSprite) {
	/* TODO: 이펙트 구현 */
	BltIndexSprite(pPoint, pSprite);
}

void CSpriteSurface::BltIndexSpriteBrightness(POINT* pPoint, CIndexSprite* pSprite, BYTE BrightBits) {
	/* TODO: 밝기 구현 */
	BltIndexSprite(pPoint, pSprite);
}

/* ============================================================================
 * Shadow Sprite 메소드
 * ============================================================================ */

void CSpriteSurface::BltShadowSprite(POINT* pPoint, CShadowSprite* pSprite) {
	if (!pPoint || !pSprite) {
		return;
	}

	/* 백엔드 스프라이트를 가져온다 */
	spritectl_sprite_t backend_sprite = get_backend_shadow_sprite(pSprite);
	if (!backend_sprite) {
		return;
	}

	/* 백엔드 서피스에 블릿한다 */
	int flags = 0;
	int alpha = 255;
	spritectl_blt_sprite(m_backend_surface, pPoint->x, pPoint->y,
	                    backend_sprite, flags, alpha);
}

void CSpriteSurface::BltShadowSpriteSmall(POINT* pPoint, CShadowSprite* pSprite, BYTE shift) {
	/* TODO: 스케일링 구현 */
	BltShadowSprite(pPoint, pSprite);
}

void CSpriteSurface::BltShadowSpriteDarkness(POINT* pPoint, CShadowSprite* pSprite, BYTE DarkBits) {
	/* TODO: 어둡게 효과 구현 */
	BltShadowSprite(pPoint, pSprite);
}

void CSpriteSurface::BltShadowSprite4444(POINT* pPoint, CShadowSprite* pSprite, WORD pixel) {
	/* TODO: 4444 포맷 구현 */
	BltShadowSprite(pPoint, pSprite);
}

void CSpriteSurface::BltShadowSpriteSmall4444(POINT* pPoint, CShadowSprite* pSprite, WORD pixel, BYTE shift) {
	/* TODO: 스케일링 + 4444 구현 */
	BltShadowSprite(pPoint, pSprite);
}

/* ============================================================================
 * Sprite Outline 메소드
 * ============================================================================ */

void CSpriteSurface::BltSpriteOutline(CSpriteOutlineManager *pSOM, WORD color) {
	/* TODO: 스프라이트 윤곽선 구현 */
}

void CSpriteSurface::BltSpriteOutlineOnly(CSpriteOutlineManager* pSOM, WORD color) {
	/* TODO: 구현 필요 */
}

void CSpriteSurface::BltSpriteOutlineDarkness(CSpriteOutlineManager* pSOM, WORD color, BYTE DarkBits) {
	/* TODO: 구현 필요 */
}
