/**
 * @file index_spritepack.h
 * @brief .ispk 파일 로드를 위한 IndexedSpritePack
 * 
 * 요구사항: 1.1, 1.2, 1.3
 * 
 * IndexedSpritePack은 .ispk(팩) 및 .ispki(인덱스) 파일에서 로드된 IndexedSprite 컬렉션을 관리합니다.
 * 전체 로드와 지연(온디맨드) 로드를 모두 지원합니다.
 */

#ifndef INDEX_SPRITEPACK_H
#define INDEX_SPRITEPACK_H

#include "index_sprite.h"
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * IndexSpritePack 구조체
 * ============================================================================ */

typedef struct IndexSpritePack IndexSpritePack;

/**
 * IndexedSpritePack 구조체
 * 
 * 지연 로딩을 지원하는 IndexedSprite 컬렉션을 관리합니다.
 */
struct IndexSpritePack {
    IndexSprite* sprites;    /**< 스프라이트 배열 */
    uint16_t size;           /**< 팩 내 스프라이트 개수 */
    int is_lazy_load;        /**< 지연 로딩 활성화 여부 */
    uint16_t loaded_count;   /**< 로드된 스프라이트 개수 (지연 로딩용) */
    FILE* pack_file;         /**< 지연 로딩용 파일 핸들 */
    int32_t* file_index;     /**< 각 스프라이트의 파일 오프셋 */
    char* filename;          /**< 팩 파일 이름 (참조용) */
};

/* ============================================================================
 * 함수 선언
 * ============================================================================ */

/**
 * IndexSpritePack 구조체 초기화
 * 
 * @param pack IndexSpritePack 포인터
 */
void index_spritepack_init(IndexSpritePack* pack);

/**
 * IndexSpritePack 리소스 해제
 * 
 * @param pack IndexSpritePack 포인터
 */
void index_spritepack_release(IndexSpritePack* pack);

/**
 * 팩 내 스프라이트 개수 반환
 * 
 * @param pack IndexSpritePack 포인터
 * @return 스프라이트 개수
 */
uint16_t index_spritepack_get_size(const IndexSpritePack* pack);

/**
 * 인덱스로 스프라이트 반환 (지연 로딩인 경우 로드 트리거)
 * 
 * @param pack IndexSpritePack 포인터
 * @param index 스프라이트 인덱스
 * @return IndexSprite 포인터, 유효하지 않은 인덱스인 경우 NULL
 * 
 * 요구사항: 1.3
 */
IndexSprite* index_spritepack_get(IndexSpritePack* pack, uint16_t index);

/**
 * 파일 경로에서 IndexSpritePack 로드 (전체 로드)
 * 
 * @param pack IndexSpritePack 포인터
 * @param filename .ispk 파일 경로
 * @return 성공 시 1, 실패 시 0
 * 
 * 요구사항: 1.1
 */
int index_spritepack_load(IndexSpritePack* pack, const char* filename);

/**
 * 파일 스트림에서 IndexSpritePack 로드 (전체 로드)
 * 
 * @param pack IndexSpritePack 포인터
 * @param file 파일 핸들
 * @return 성공 시 1, 실패 시 0
 */
int index_spritepack_load_from_file(IndexSpritePack* pack, FILE* file);

/**
 * 지연 로딩 방식으로 IndexSpritePack 로드 (인덱스 파일 필요)
 * 
 * index_spritepack_get()을 통해 접근할 때 온디맨드로 스프라이트를 로드합니다.
 * 팩 파일과 동일한 위치에 인덱스 파일(.ispki)이 존재해야 합니다.
 * 
 * @param pack IndexSpritePack 포인터
 * @param filename .ispk 파일 경로
 * @return 성공 시 1, 실패 시 0
 * 
 * 요구사항: 1.2, 1.3
 */
int index_spritepack_load_lazy(IndexSpritePack* pack, const char* filename);

/**
 * 특정 범위의 스프라이트 로드 (부분 로딩용)
 * 
 * @param pack IndexSpritePack 포인터
 * @param first 시작 스프라이트 인덱스
 * @param last 끝 스프라이트 인덱스 (포함)
 * @return 성공 시 1, 실패 시 0
 */
int index_spritepack_load_part(IndexSpritePack* pack, int first, int last);

/**
 * 특정 범위의 스프라이트 해제 (메모리 절약용)
 * 
 * @param pack IndexSpritePack 포인터
 * @param first 시작 스프라이트 인덱스
 * @param last 끝 스프라이트 인덱스 (포함)
 * @return 성공 시 1, 실패 시 0
 */
int index_spritepack_release_part(IndexSpritePack* pack, int first, int last);

#ifdef __cplusplus
}
#endif

#endif /* INDEX_SPRITEPACK_H */
