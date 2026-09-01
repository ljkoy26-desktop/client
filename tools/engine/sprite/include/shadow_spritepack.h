/**
 * @file shadow_spritepack.h
 * @brief .sspk 파일 로드를 위한 ShadowSpritePack
 * 
 * 요구사항: 2.1, 2.2
 * 
 * ShadowSpritePack은 .sspk(팩) 및 .sspki(인덱스) 파일에서 로드된 ShadowSprite 컬렉션을 관리합니다.
 * 전체 로드와 지연(온디맨드) 로드를 모두 지원합니다.
 */

#ifndef SHADOW_SPRITEPACK_H
#define SHADOW_SPRITEPACK_H

#include "shadow_sprite.h"
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * ShadowSpritePack 구조체
 * ============================================================================ */

/**
 * ShadowSpritePack 구조체
 * 
 * 지연 로딩을 지원하는 ShadowSprite 컬렉션을 관리합니다.
 */
typedef struct {
    ShadowSprite* sprites;   /**< 스프라이트 배열 */
    uint16_t size;           /**< 팩 내 스프라이트 개수 */
    int is_lazy_load;        /**< 지연 로딩 활성화 여부 */
    uint16_t loaded_count;   /**< 로드된 스프라이트 개수 (지연 로딩용) */
    FILE* pack_file;         /**< 지연 로딩용 파일 핸들 */
    int32_t* file_index;     /**< 각 스프라이트의 파일 오프셋 */
    char* filename;          /**< 팩 파일 이름 (참조용) */
} ShadowSpritePack;

/* ============================================================================
 * 함수 선언
 * ============================================================================ */

/**
 * ShadowSpritePack 구조체 초기화
 * 
 * @param pack ShadowSpritePack 포인터
 */
void shadow_spritepack_init(ShadowSpritePack* pack);

/**
 * ShadowSpritePack 리소스 해제
 * 
 * @param pack ShadowSpritePack 포인터
 */
void shadow_spritepack_release(ShadowSpritePack* pack);

/**
 * 팩 내 스프라이트 개수 반환
 * 
 * @param pack ShadowSpritePack 포인터
 * @return 스프라이트 개수
 */
uint16_t shadow_spritepack_get_size(const ShadowSpritePack* pack);

/**
 * 인덱스로 스프라이트 반환 (지연 로딩인 경우 로드 트리거)
 * 
 * @param pack ShadowSpritePack 포인터
 * @param index 스프라이트 인덱스
 * @return ShadowSprite 포인터, 유효하지 않은 인덱스인 경우 NULL
 * 
 * 요구사항: 2.2
 */
ShadowSprite* shadow_spritepack_get(ShadowSpritePack* pack, uint16_t index);

/**
 * 파일 경로에서 ShadowSpritePack 로드 (전체 로드)
 * 
 * @param pack ShadowSpritePack 포인터
 * @param filename .sspk 파일 경로
 * @return 성공 시 1, 실패 시 0
 * 
 * 요구사항: 2.1
 */
int shadow_spritepack_load(ShadowSpritePack* pack, const char* filename);

/**
 * 파일 스트림에서 ShadowSpritePack 로드 (전체 로드)
 * 
 * @param pack ShadowSpritePack 포인터
 * @param file 파일 핸들
 * @return 성공 시 1, 실패 시 0
 */
int shadow_spritepack_load_from_file(ShadowSpritePack* pack, FILE* file);

/**
 * 지연 로딩 방식으로 ShadowSpritePack 로드 (인덱스 파일 필요)
 * 
 * shadow_spritepack_get()을 통해 접근할 때 온디맨드로 스프라이트를 로드합니다.
 * 팩 파일과 동일한 위치에 인덱스 파일(.sspki)이 존재해야 합니다.
 * 
 * @param pack ShadowSpritePack 포인터
 * @param filename .sspk 파일 경로
 * @return 성공 시 1, 실패 시 0
 * 
 * 요구사항: 2.1, 2.2
 */
int shadow_spritepack_load_lazy(ShadowSpritePack* pack, const char* filename);

/**
 * 특정 범위의 스프라이트 로드 (부분 로딩용)
 * 
 * @param pack ShadowSpritePack 포인터
 * @param first 시작 스프라이트 인덱스
 * @param last 끝 스프라이트 인덱스 (포함)
 * @return 성공 시 1, 실패 시 0
 */
int shadow_spritepack_load_part(ShadowSpritePack* pack, int first, int last);

/**
 * 특정 범위의 스프라이트 해제 (메모리 절약용)
 * 
 * @param pack ShadowSpritePack 포인터
 * @param first 시작 스프라이트 인덱스
 * @param last 끝 스프라이트 인덱스 (포함)
 * @return 성공 시 1, 실패 시 0
 */
int shadow_spritepack_release_part(ShadowSpritePack* pack, int first, int last);

#ifdef __cplusplus
}
#endif

#endif /* SHADOW_SPRITEPACK_H */
