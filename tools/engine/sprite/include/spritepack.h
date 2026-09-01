/**
 * @file spritepack.h
 * @brief SpritePack 파일 포맷 파싱
 * 
 * 요구사항: 4.1, 4.2, 4.3, 4.4, 4.5
 */

#ifndef SPRITEPACK_H
#define SPRITEPACK_H

#include "sprite.h"
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SpritePack SpritePack;

/**
 * SpritePack 구조체
 */
struct SpritePack {
    uint16_t count;          /* 팩 내 스프라이트 개수 */
    Sprite* sprites;         /* 스프라이트 배열 */
    uint32_t* file_offsets;  /* 인덱스 파일로부터 읽은 파일 오프셋 (선택적) */
    FILE* pack_file;         /* 지연 로딩용 파일 핸들 */
    int lazy_load;           /* 지연 로딩 활성화 여부 */
    char* filename;          /* 지연 로딩용 팩 파일 이름 */
};

/**
 * 스프라이트 팩 로드 (전체 로드)
 * @param pack SpritePack 구조체 포인터
 * @param filename .spk 파일 경로
 * @return 성공 시 0, 실패 시 음수 에러 코드
 */
int spritepack_load(SpritePack* pack, const char* filename);

/**
 * 지연 로딩 방식으로 스프라이트 팩 로드 (인덱스 파일 필요)
 * @param pack SpritePack 구조체 포인터
 * @param filename .spk 파일 경로
 * @return 성공 시 0, 실패 시 음수 에러 코드
 */
int spritepack_load_lazy(SpritePack* pack, const char* filename);

/**
 * 팩에서 스프라이트 가져오기 (지연 로딩 시 로드 트리거)
 * @param pack SpritePack 포인터
 * @param index 스프라이트 인덱스
 * @return 스프라이트 포인터, 유효하지 않은 인덱스인 경우 NULL
 */
Sprite* spritepack_get(SpritePack* pack, uint16_t index);

uint16_t spritepack_size(SpritePack* pack);

/**
 * 스프라이트 팩 리소스 해제
 * @param pack SpritePack 구조체 포인터
 */
void spritepack_free(SpritePack* pack);

#ifdef __cplusplus
}
#endif

#endif /* SPRITEPACK_H */
