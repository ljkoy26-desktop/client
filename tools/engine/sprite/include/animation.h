/**
 * @file animation.h
 * @brief 애니메이션 프레임 및 오브젝트 관리
 * 
 * 요구사항: 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 3.1, 3.2, 3.3, 3.5, 3.6, 6.1, 6.2
 */

#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL.h>
#include <stdint.h>
#include "types.h"
#include "sprite.h"
#include "spritepack.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * 애니메이션 프레임 구조체
 * 요구사항: 2.1, 2.2, 2.3, 2.4, 2.5, 2.6
 * ============================================================================ */

/**
 * 애니메이션 프레임 상태
 * 
 * 애니메이션의 프레임 진행을 관리하며, 수동 프레임 진행 및
 * 루프 기반 프레임 계산을 모두 지원합니다.
 */
typedef struct AnimFrame {
    FrameID frame_id;       /**< 프레임 그룹 식별자 (요구사항 2.2) */
    uint8_t current_frame;  /**< 현재 프레임 인덱스 (요구사항 2.1) */
    uint8_t max_frame;      /**< 최대 프레임 수 (요구사항 2.1) */
    uint8_t blt_type;       /**< 렌더링 타입 (BltType) (요구사항 2.6) */
    uint8_t loop;           /**< 루프 모드 플래그 (요구사항 2.5) */
} AnimFrame;

/**
 * 애니메이션 프레임을 기본값으로 초기화
 * 
 * @param frame AnimFrame 구조체 포인터
 * @param blt_type 초기 렌더링 타입 (BltType)
 * 
 * 요구사항: 2.1, 2.6
 */
void anim_frame_init(AnimFrame* frame, uint8_t blt_type);

/**
 * 프레임 시퀀스 파라미터 설정
 * 
 * @param frame AnimFrame 구조체 포인터
 * @param id 프레임 그룹 식별자
 * @param max_frame 시퀀스 내 최대 프레임 수
 * 
 * 요구사항: 2.1, 2.2
 */
void anim_frame_set(AnimFrame* frame, FrameID id, uint8_t max_frame);

/**
 * 다음 프레임으로 진행
 * 
 * current_frame을 1 증가시킵니다. current_frame이 max_frame에 도달하면
 * 0으로 다시 순환합니다.
 * 
 * @param frame AnimFrame 구조체 포인터
 * 
 * 요구사항: 2.3, 2.4
 */
void anim_frame_next(AnimFrame* frame);

/**
 * 루프 모드에서 프레임 진행
 * 
 * 루프 모드에서는 현재 프레임이 (loop_counter % max_frame)으로 계산됩니다.
 * 이를 통해 여러 애니메이션이 동기화된 상태를 유지할 수 있습니다.
 * 
 * @param frame AnimFrame 구조체 포인터
 * @param loop_counter 전역 루프 카운터 값
 * 
 * 요구사항: 2.5
 */
void anim_frame_next_loop(AnimFrame* frame, uint32_t loop_counter);

/**
 * 현재 프레임 인덱스 반환
 * 
 * @param frame AnimFrame 구조체 포인터
 * @return 현재 프레임 인덱스
 * 
 * 요구사항: 2.1
 */
uint8_t anim_frame_get(const AnimFrame* frame);

/* ============================================================================
 * 애니메이션 오브젝트 구조체
 * 요구사항: 3.1, 3.2, 3.3, 3.5, 3.6, 6.1, 6.2
 * ============================================================================ */

/**
 * 애니메이션 오브젝트
 * 
 * 스프라이트 데이터와 프레임 관리를 결합하여 애니메이션 게임 오브젝트를 렌더링합니다.
 * 8방향 애니메이션을 지원합니다.
 */
typedef struct AnimObject {
    AnimFrame frame;        /**< 프레임 관리 데이터 (요구사항 3.4) */
    SpriteID sprite_id;     /**< 기본 스프라이트 ID (요구사항 3.1) */
    int pixel_x;            /**< X 픽셀 위치 (요구사항 3.2) */
    int pixel_y;            /**< Y 픽셀 위치 (요구사항 3.2) */
    uint8_t direction;      /**< 방향 (0-7) (요구사항 3.3, 6.1) */
    uint8_t transparent;    /**< 투명도 플래그 (요구사항 3.6) */
} AnimObject;

/**
 * 애니메이션 오브젝트를 기본값으로 초기화
 * 
 * @param obj AnimObject 구조체 포인터
 * 
 * 요구사항: 3.1, 3.2, 3.3, 3.6
 */
void anim_object_init(AnimObject* obj);

/**
 * 애니메이션 오브젝트 속성 설정
 * 
 * @param obj AnimObject 구조체 포인터
 * @param sprite_id 기본 스프라이트 ID
 * @param px X 픽셀 위치
 * @param py Y 픽셀 위치
 * @param direction 방향 (0-7, 자동 순환 범위 조정)
 * @param trans 투명도 플래그
 * 
 * 요구사항: 3.1, 3.2, 3.3, 3.6, 6.1
 */
void anim_object_set(AnimObject* obj, SpriteID sprite_id, 
                     int px, int py, uint8_t direction, uint8_t trans);

/**
 * 애니메이션의 프레임 시퀀스 설정
 * 
 * @param obj AnimObject 구조체 포인터
 * @param frame_id 프레임 그룹 식별자
 * @param max_frame 최대 프레임 수
 * 
 * 요구사항: 3.4
 */
void anim_object_set_frame(AnimObject* obj, FrameID frame_id, uint8_t max_frame);

/**
 * 애니메이션 프레임 진행
 * 
 * @param obj AnimObject 구조체 포인터
 * 
 * 요구사항: 3.5
 */
void anim_object_next_frame(AnimObject* obj);

/**
 * 현재 스프라이트 ID 반환
 * 
 * 기본 sprite_id + 현재 프레임으로부터 실제 스프라이트 인덱스를 계산합니다.
 * 
 * @param obj AnimObject 구조체 포인터
 * @return 현재 스프라이트 ID (기본 ID + 프레임 오프셋)
 * 
 * 요구사항: 3.5
 */
SpriteID anim_object_get_sprite(const AnimObject* obj);

/**
 * 방향 설정
 * 
 * @param obj AnimObject 구조체 포인터
 * @param dir 방향 값 (0-7로 자동 래핑됨)
 * 
 * 요구사항: 6.1, 6.2
 */
void anim_object_set_direction(AnimObject* obj, uint8_t dir);

/* ============================================================================
 * 애니메이션 렌더링 함수
 * 요구사항: 4.1, 4.2, 4.3, 4.4, 4.5, 4.6
 * ============================================================================ */

/**
 * BltType에 따라 SDL 블렌드 모드 설정
 * 
 * @param texture 블렌드 모드를 설정할 SDL 텍스처
 * @param blt_type 렌더링 타입
 * 
 * 요구사항: 4.2, 4.3, 4.4, 4.5
 */
void anim_set_blend_mode(SDL_Texture* texture, BltType blt_type);

/**
 * 애니메이션 오브젝트를 화면에 렌더링
 * 
 * @param renderer SDL 렌더러
 * @param obj 렌더링할 애니메이션 오브젝트
 * @param pack 스프라이트가 포함된 SpritePack
 * @param cache 선택적 스프라이트 캐시 (NULL 가능)
 * @param colorkey 투명 컬러키
 * @param zoom 확대/축소 비율 (1.0 = 기본 크기)
 * @return 성공 시 0, 실패 시 음수
 * 
 * 요구사항: 4.1, 4.6
 */
int anim_render(SDL_Renderer* renderer, 
                const AnimObject* obj,
                SpritePack* pack,
                DecodedSprite* cache,
                uint16_t colorkey,
                float zoom);

/**
 * 특정 블렌드 모드로 렌더링
 * 
 * @param renderer SDL 렌더러
 * @param obj 렌더링할 애니메이션 오브젝트
 * @param pack 스프라이트가 포함된 SpritePack
 * @param cache 선택적 스프라이트 캐시 (NULL 가능)
 * @param colorkey 투명 컬러키
 * @param zoom 확대/축소 비율
 * @param blt_type 사용할 렌더링 타입
 * @return 성공 시 0, 실패 시 음수
 * 
 * 요구사항: 4.2, 4.3, 4.4, 4.5
 */
int anim_render_blt(SDL_Renderer* renderer,
                    const AnimObject* obj,
                    SpritePack* pack,
                    DecodedSprite* cache,
                    uint16_t colorkey,
                    float zoom,
                    BltType blt_type);

#ifdef __cplusplus
}
#endif

#endif /* ANIMATION_H */
