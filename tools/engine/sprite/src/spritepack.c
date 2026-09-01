/**
 * @file spritepack.c
 * @brief SpritePack 파일 포맷 파싱 구현부
 * 
 * SpritePack 파일 포맷 (.spk):
 *   - 스프라이트 개수 (2 바이트, uint16_t, 리틀 엔디언)
 *   - 스프라이트 0 데이터
 *   - 스프라이트 1 데이터
 *   - ... (각 스프라이트별 반복)
 * 
 * 인덱스 파일 포맷 (.spki):
 *   - 스프라이트 개수 (2 바이트, uint16_t, 리틀 엔디언)
 *   - 오프셋 0 (4 바이트, uint32_t)
 *   - 오프셋 1 (4 바이트, uint32_t)
 *   - ... (각 스프라이트별 반복)
 * 
 * 요구사항: 4.1, 4.2, 4.3, 4.4, 4.5
 */

#include "spritepack.h"
#include <stdlib.h>
#include <string.h>

/**
 * SpritePack 구조체를 안전한 기본값으로 초기화
 */
static void spritepack_init(SpritePack* pack) {
    pack->count = 0;
    pack->sprites = NULL;
    pack->file_offsets = NULL;
    pack->pack_file = NULL;
    pack->lazy_load = 0;
    pack->filename = NULL;
}

uint16_t spritepack_size(SpritePack* pack) {
    if (!pack) return 0;
    return pack->count;
}

/**
 * 스프라이트 팩 로드 (전체 로드)
 * 요구사항: 4.1, 4.2, 4.5
 * 
 * @param pack SpritePack 구조체 포인터
 * @param filename .spk 파일 경로
 * @return 성공 시 0, 실패 시 음수 에러 코드
 *         -1: 유효하지 않은 매개변수
 *         -2: 파일 열기 실패
 *         -3: 메모리 할당 실패
 *         -4: 파일 포맷 오류
 */
int spritepack_load(SpritePack* pack, const char* filename) {
    if (!pack || !filename) {
        return -1;
    }
    
    /* 팩 구조체 초기화 */
    spritepack_init(pack);
    
    /* 팩 파일 열기 */
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return -2;  /* 파일 열기 실패 */
    }
    
    /* 스프라이트 개수 읽기 (2 바이트, 리틀 엔디언) - 요구사항 4.1 */
    if (fread(&pack->count, 2, 1, file) != 1) {
        fclose(file);
        return -4;  /* 파일 포맷 오류 */
    }
    
    /* 빈 팩 처리 */
    if (pack->count == 0) {
        fclose(file);
        return 0;
    }
    
    /* 스프라이트 배열 할당 */
    pack->sprites = (Sprite*)calloc(pack->count, sizeof(Sprite));
    if (!pack->sprites) {
        fclose(file);
        pack->count = 0;
        return -3;  /* 메모리 할당 실패 */
    }
    
    /* 각 스프라이트 로드 - 요구사항 4.2 */
    for (uint16_t i = 0; i < pack->count; i++) {
        int result = sprite_load(&pack->sprites[i], file);
        if (result != 0) {
            /* 요구사항 4.5: 나머지 스프라이트를 계속 로드하고 오류 보고 */
            pack->sprites[i].is_valid = 0;
            /* 참고: 로딩은 계속 진행하지만 해당 스프라이트는 무효로 표시됨 */
        }
    }
    
    fclose(file);
    
    /* 일부 스프라이트가 실패해도 성공 반환 (요구사항 4.5 준수) */
    return 0;
}

/**
 * 지연 로딩 방식으로 스프라이트 팩 로드 (인덱스 파일 필요)
 * 요구사항: 4.3, 4.4
 * 
 * @param pack SpritePack 구조체 포인터
 * @param filename .spk 파일 경로
 * @return 성공 시 0, 실패 시 음수 에러 코드
 *         -1: 유효하지 않은 매개변수
 *         -2: 파일 열기 실패
 *         -3: 메모리 할당 실패
 *         -4: 파일 포맷 오류
 *         -5: 인덱스 파일을 찾을 수 없거나 유효하지 않음
 */
int spritepack_load_lazy(SpritePack* pack, const char* filename) {
    if (!pack || !filename) {
        return -1;
    }
    
    /* 팩 구조체 초기화 */
    spritepack_init(pack);
    
    /* 파일명 뒤에 'i'를 덧붙여 인덱스 파일명 생성 */
    size_t filename_len = strlen(filename);
    char* index_filename = (char*)malloc(filename_len + 2);
    if (!index_filename) {
        return -3;
    }
    strcpy(index_filename, filename);
    strcat(index_filename, "i");
    
    /* 인덱스 파일 열기 - 요구사항 4.3 */
    FILE* index_file = fopen(index_filename, "rb");
    if (!index_file) {
        free(index_filename);
        return -5;  /* 인덱스 파일 없음 */
    }
    
    /* 인덱스 파일에서 스프라이트 개수 읽기 - 요구사항 4.4 */
    if (fread(&pack->count, 2, 1, index_file) != 1) {
        fclose(index_file);
        free(index_filename);
        return -4;
    }
    
    /* 빈 팩 처리 */
    if (pack->count == 0) {
        fclose(index_file);
        free(index_filename);
        return 0;
    }
    
    /* 파일 오프셋 배열 할당 - 요구사항 4.4 */
    pack->file_offsets = (uint32_t*)malloc(pack->count * sizeof(uint32_t));
    if (!pack->file_offsets) {
        fclose(index_file);
        free(index_filename);
        pack->count = 0;
        return -3;
    }
    
    /* 파일 오프셋 읽기 (각 4 바이트) - 요구사항 4.4 */
    for (uint16_t i = 0; i < pack->count; i++) {
        if (fread(&pack->file_offsets[i], 4, 1, index_file) != 1) {
            fclose(index_file);
            free(index_filename);
            free(pack->file_offsets);
            pack->file_offsets = NULL;
            pack->count = 0;
            return -4;
        }
    }
    
    fclose(index_file);
    
    /* 스프라이트 배열 할당 (모두 미초기화 상태) */
    pack->sprites = (Sprite*)calloc(pack->count, sizeof(Sprite));
    if (!pack->sprites) {
        free(index_filename);
        free(pack->file_offsets);
        pack->file_offsets = NULL;
        pack->count = 0;
        return -3;
    }
    
    /* 팩 파일을 열고 지연 로딩을 위해 열린 상태 유지 */
    pack->pack_file = fopen(filename, "rb");
    if (!pack->pack_file) {
        free(index_filename);
        free(pack->file_offsets);
        free(pack->sprites);
        pack->file_offsets = NULL;
        pack->sprites = NULL;
        pack->count = 0;
        return -2;
    }
    
    /* 참조용 파일 이름 저장 */
    pack->filename = (char*)malloc(filename_len + 1);
    if (pack->filename) {
        strcpy(pack->filename, filename);
    }
    
    pack->lazy_load = 1;
    
    free(index_filename);
    return 0;
}

/**
 * 팩에서 스프라이트 가져오기 (지연 로딩 시 로드 트리거)
 * 요구사항: 4.3 (랜덤 액세스 로딩)
 * 
 * @param pack SpritePack 포인터
 * @param index 스프라이트 인덱스
 * @return 스프라이트 포인터, 유효하지 않은 인덱스인 경우 NULL
 */
Sprite* spritepack_get(SpritePack* pack, uint16_t index) {
    if (!pack || !pack->sprites) {
        return NULL;
    }
    
    /* 인덱스 범위 확인 */
    if (index >= pack->count) {
        return NULL;
    }
    
    /* 지연 로딩 중이며 스프라이트가 아직 로드되지 않은 경우 */
    if (pack->lazy_load && !pack->sprites[index].is_valid && 
        pack->pack_file && pack->file_offsets) {
        
        /* 인덱스를 사용하여 스프라이트 위치로 탐색 */
        if (fseek(pack->pack_file, pack->file_offsets[index], SEEK_SET) != 0) {
            return NULL;
        }
        
        /* 스프라이트 로드 */
        int result = sprite_load(&pack->sprites[index], pack->pack_file);
        if (result != 0) {
            /* 무효로 표시하지만 포인터는 반환 */
            pack->sprites[index].is_valid = 0;
        }
    }
    
    return &pack->sprites[index];
}

/**
 * 스프라이트 팩 리소스 해제
 * 
 * @param pack SpritePack 구조체 포인터
 */
void spritepack_free(SpritePack* pack) {
    if (!pack) {
        return;
    }
    
    /* 모든 스프라이트 해제 */
    if (pack->sprites) {
        for (uint16_t i = 0; i < pack->count; i++) {
            sprite_free(&pack->sprites[i]);
        }
        free(pack->sprites);
        pack->sprites = NULL;
    }
    
    /* 파일 오프셋 해제 */
    if (pack->file_offsets) {
        free(pack->file_offsets);
        pack->file_offsets = NULL;
    }
    
    /* 열려 있는 팩 파일 닫기 */
    if (pack->pack_file) {
        fclose(pack->pack_file);
        pack->pack_file = NULL;
    }
    
    /* 파일 이름 해제 */
    if (pack->filename) {
        free(pack->filename);
        pack->filename = NULL;
    }
    
    pack->count = 0;
    pack->lazy_load = 0;
}
