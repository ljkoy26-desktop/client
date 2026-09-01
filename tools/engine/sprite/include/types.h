/**
 * @file types.h
 * @brief Dark Eden SDL 라이브러리를 위한 공용 타입 정의
 * 
 * 요구사항: 2.6, 3.3, 6.1
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * 스프라이트 및 프레임 ID
 * ============================================================================ */

/**
 * 스프라이트 ID 타입 - SpritePack 내의 스프라이트를 식별
 */
typedef uint16_t SpriteID;

/**
 * 프레임 ID 타입 - 애니메이션 프레임 그룹을 식별
 */
typedef uint16_t FrameID;

/**
 * 무효/Null 스프라이트 ID 마커
 */
#define SPRITEID_NULL   0xFFFF

/**
 * 무효/Null 프레임 ID 마커
 */
#define FRAMEID_NULL    0xFFFF

/* ============================================================================
 * 방향 열거형
 * 요구사항 6.1: 8방향(0-7) 지원
 * ============================================================================ */

/**
 * 8방향 애니메이션을 위한 방향 열거형
 * 값은 기존 Dark Eden 클라이언트 방향 시스템에 대응됩니다.
 */
typedef enum Direction {
    DIR_LEFT = 0,       /**< 좌측 방향 */
    DIR_LEFTDOWN,       /**< 좌하단 대각선 방향 */
    DIR_DOWN,           /**< 하단 방향 */
    DIR_RIGHTDOWN,      /**< 우하단 대각선 방향 */
    DIR_RIGHT,          /**< 우측 방향 */
    DIR_RIGHTUP,        /**< 우상단 대각선 방향 */
    DIR_UP,             /**< 상단 방향 */
    DIR_LEFTUP,         /**< 좌상단 대각선 방향 */
    DIR_MAX             /**< 방향 총 개수 (8) */
} Direction;

/* ============================================================================
 * BltType 열거형
 * 요구사항 2.6: 렌더링 모드 선택을 위한 BltType 저장
 * ============================================================================ */

/**
 * 다양한 렌더링 모드를 위한 블릿 타입 열거형
 * 
 * BLT_NORMAL: 표준 알파 블렌딩 (SDL_BLENDMODE_BLEND)
 * BLT_EFFECT: 발광 효과를 위한 가산 블렌딩 (SDL_BLENDMODE_ADD)
 * BLT_SHADOW: 어둡고 반투명하게 렌더링하는 그림자 효과
 * BLT_SCREEN: 스크린 블렌드 효과 (SDL_BLENDMODE_ADD로 근사)
 */
typedef enum BltType {
    BLT_NORMAL = 0,     /**< 표준 알파 블렌딩 */
    BLT_EFFECT,         /**< 이펙트용 가산 블렌딩 */
    BLT_SHADOW,         /**< 그림자 렌더링 모드 */
    BLT_SCREEN          /**< 스크린 블렌드 모드 */
} BltType;

/* ============================================================================
 * 기본값
 * ============================================================================ */

/**
 * 투명 처리를 위한 기본 컬러키 값 (RGB565 값 0)
 */
#define DEFAULT_COLORKEY 0

#ifdef __cplusplus
}
#endif

#endif /* TYPES_H */
