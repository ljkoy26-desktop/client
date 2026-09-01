/**
 * @file shadow_spritepack.c
 * @brief ShadowSpritePack 구현부
 * 
 * 요구사항: 2.1, 2.2
 * 
 * 파일 포맷:
 * 
 * .sspk (팩 파일):
 *   [size: 2 바이트] - 스프라이트 개수
 *   [스프라이트 데이터...] - 순차적 스프라이트 데이터
 * 
 * .sspki (인덱스 파일):
 *   [size: 2 바이트] - 스프라이트 개수
 *   [offset: 4 바이트] × size - 각 스프라이트의 파일 오프셋
 */

#include "shadow_spritepack.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * 초기화 및 정리
 * ============================================================================ */

void shadow_spritepack_init(ShadowSpritePack* pack) {
    if (!pack) return;
    
    pack->sprites = NULL;
    pack->size = 0;
    pack->is_lazy_load = 0;
    pack->loaded_count = 0;
    pack->pack_file = NULL;
    pack->file_index = NULL;
    pack->filename = NULL;
}

void shadow_spritepack_release(ShadowSpritePack* pack) {
    if (!pack) return;
    
    /* 모든 스프라이트 해제 */
    if (pack->sprites) {
        for (int i = 0; i < pack->size; i++) {
            shadow_sprite_release(&pack->sprites[i]);
        }
        free(pack->sprites);
        pack->sprites = NULL;
    }
    
    /* 지연 로딩 파일 닫기 */
    if (pack->pack_file) {
        fclose(pack->pack_file);
        pack->pack_file = NULL;
    }
    
    /* 파일 인덱스 해제 */
    if (pack->file_index) {
        free(pack->file_index);
        pack->file_index = NULL;
    }
    
    /* 파일 이름 해제 */
    if (pack->filename) {
        free(pack->filename);
        pack->filename = NULL;
    }
    
    pack->size = 0;
    pack->is_lazy_load = 0;
    pack->loaded_count = 0;
}

uint16_t shadow_spritepack_get_size(const ShadowSpritePack* pack) {
    return pack ? pack->size : 0;
}

/* ============================================================================
 * 스프라이트 접근
 * ============================================================================ */

ShadowSprite* shadow_spritepack_get(ShadowSpritePack* pack, uint16_t index) {
    if (!pack || !pack->sprites || index >= pack->size) {
        return NULL;
    }
    
    /* 지연 로딩 중이며 스프라이트가 아직 로드되지 않은 경우 */
    if (pack->is_lazy_load && !shadow_sprite_is_init(&pack->sprites[index])) {
        if (pack->pack_file && pack->file_index) {
            /* 스프라이트 위치로 탐색 */
            fseek(pack->pack_file, pack->file_index[index], SEEK_SET);
            
            /* 스프라이트 로드 */
            if (shadow_sprite_load_from_file(&pack->sprites[index], pack->pack_file)) {
                pack->loaded_count++;
                
                /* 모든 스프라이트가 로드되었으면 파일 닫고 리소스 해제 */
                if (pack->loaded_count >= pack->size) {
                    pack->is_lazy_load = 0;
                    fclose(pack->pack_file);
                    pack->pack_file = NULL;
                    free(pack->file_index);
                    pack->file_index = NULL;
                }
            }
        }
    }
    
    return &pack->sprites[index];
}

/* ============================================================================
 * 파일 로딩
 * ============================================================================ */

int shadow_spritepack_load_from_file(ShadowSpritePack* pack, FILE* file) {
    if (!pack || !file) return 0;
    
    /* 기존 데이터가 있으면 해제 */
    shadow_spritepack_release(pack);
    
    /* 스프라이트 개수 읽기 */
    if (fread(&pack->size, 2, 1, file) != 1) {
        return 0;
    }
    
    if (pack->size == 0) {
        return 1;
    }
    
    /* 스프라이트 배열 할당 */
    pack->sprites = (ShadowSprite*)malloc(pack->size * sizeof(ShadowSprite));
    if (!pack->sprites) {
        pack->size = 0;
        return 0;
    }
    
    /* 모든 스프라이트 초기화 */
    for (int i = 0; i < pack->size; i++) {
        shadow_sprite_init(&pack->sprites[i]);
    }
    
    /* 모든 스프라이트 로드 */
    for (int i = 0; i < pack->size; i++) {
        if (!shadow_sprite_load_from_file(&pack->sprites[i], file)) {
            /* 한 스프라이트가 실패해도 계속 로드 진행 */
        }
    }
    
    return 1;
}

int shadow_spritepack_load(ShadowSpritePack* pack, const char* filename) {
    if (!pack || !filename) return 0;
    
    FILE* file = fopen(filename, "rb");
    if (!file) return 0;
    
    int result = shadow_spritepack_load_from_file(pack, file);
    
    fclose(file);
    return result;
}

int shadow_spritepack_load_lazy(ShadowSpritePack* pack, const char* filename) {
    if (!pack || !filename) return 0;
    
    /* 기존 데이터가 있으면 해제 */
    shadow_spritepack_release(pack);
    
    /* 인덱스 파일명 생성 (.sspki) */
    size_t len = strlen(filename);
    char* indexFilename = (char*)malloc(len + 2);
    if (!indexFilename) return 0;
    
    strcpy(indexFilename, filename);
    strcat(indexFilename, "i");
    
    /* 인덱스 파일 열기 */
    FILE* indexFile = fopen(indexFilename, "rb");
    if (!indexFile) {
        free(indexFilename);
        return 0;
    }
    
    /* 인덱스 파일에서 스프라이트 개수 읽기 */
    if (fread(&pack->size, 2, 1, indexFile) != 1) {
        fclose(indexFile);
        free(indexFilename);
        return 0;
    }
    
    if (pack->size == 0) {
        fclose(indexFile);
        free(indexFilename);
        return 1;
    }
    
    /* 스프라이트 배열 할당 */
    pack->sprites = (ShadowSprite*)malloc(pack->size * sizeof(ShadowSprite));
    if (!pack->sprites) {
        fclose(indexFile);
        free(indexFilename);
        pack->size = 0;
        return 0;
    }
    
    /* 모든 스프라이트 초기화 */
    for (int i = 0; i < pack->size; i++) {
        shadow_sprite_init(&pack->sprites[i]);
    }
    
    /* 파일 인덱스 할당 및 읽기 */
    pack->file_index = (int32_t*)malloc(pack->size * sizeof(int32_t));
    if (!pack->file_index) {
        free(pack->sprites);
        pack->sprites = NULL;
        fclose(indexFile);
        free(indexFilename);
        pack->size = 0;
        return 0;
    }
    
    for (int i = 0; i < pack->size; i++) {
        if (fread(&pack->file_index[i], 4, 1, indexFile) != 1) {
            free(pack->file_index);
            pack->file_index = NULL;
            free(pack->sprites);
            pack->sprites = NULL;
            fclose(indexFile);
            free(indexFilename);
            pack->size = 0;
            return 0;
        }
    }
    
    fclose(indexFile);
    
    /* 지연 로딩을 위해 팩 파일 열기 */
    pack->pack_file = fopen(filename, "rb");
    if (!pack->pack_file) {
        free(pack->file_index);
        pack->file_index = NULL;
        free(pack->sprites);
        pack->sprites = NULL;
        free(indexFilename);
        pack->size = 0;
        return 0;
    }
    
    /* 팩 파일의 크기 헤더 건너뛰기 */
    uint16_t packSize;
    if (fread(&packSize, 2, 1, pack->pack_file) != 1) {
        fclose(pack->pack_file);
        pack->pack_file = NULL;
        free(pack->file_index);
        pack->file_index = NULL;
        free(pack->sprites);
        pack->sprites = NULL;
        free(indexFilename);
        pack->size = 0;
        return 0;
    }
    
    /* 참조용 파일 이름 저장 */
    pack->filename = (char*)malloc(len + 1);
    if (pack->filename) {
        strcpy(pack->filename, filename);
    }
    
    pack->is_lazy_load = 1;
    pack->loaded_count = 0;
    
    free(indexFilename);
    return 1;
}

int shadow_spritepack_load_part(ShadowSpritePack* pack, int first, int last) {
    if (!pack || !pack->sprites) return 0;
    
    /* 범위 보정 */
    if (first < 0) first = 0;
    if (last >= pack->size) last = pack->size - 1;
    if (first > last) return 0;
    
    /* 범위 내의 각 스프라이트 로드 */
    for (int i = first; i <= last; i++) {
        shadow_spritepack_get(pack, (uint16_t)i);
    }
    
    return 1;
}

int shadow_spritepack_release_part(ShadowSpritePack* pack, int first, int last) {
    if (!pack || !pack->sprites) return 0;
    
    /* 범위 보정 */
    if (first < 0) first = 0;
    if (last >= pack->size) last = pack->size - 1;
    if (first > last) return 0;
    
    /* 범위 내의 각 스프라이트 해제 */
    for (int i = first; i <= last; i++) {
        shadow_sprite_release(&pack->sprites[i]);
    }
    
    return 1;
}
