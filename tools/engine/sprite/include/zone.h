/**
 * @file zone.h
 * @brief Dark Eden용 Zone (맵) 파일 파싱
 * 
 * 다음 항목을 포함하는 .map zone 파일을 파싱합니다:
 * - Zone 헤더 (버전, ID, 이름, 유형 등)
 * - 섹터 그리드 (타일 스프라이트 ID, 속성, 조명)
 * - ImageObject (건물, 장식물 등)
 * 
 * 원본 MZone.cpp 구현을 기반으로 합니다.
 */

#ifndef ZONE_H
#define ZONE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * 상수 정의
 * ============================================================================ */

#define ZONE_VERSION_STRING "=MAP_2000_05_10="
#define ZONE_VERSION_LEN    16

#define ZONE_MAX_NAME_LEN   256
#define ZONE_MAX_DESC_LEN   1024

/* 섹터 속성 플래그 (MSector.h 참조) */
#define SECTOR_BLOCK_UNDERGROUND    0x01
#define SECTOR_BLOCK_GROUND         0x02
#define SECTOR_BLOCK_FLYING         0x04
#define SECTOR_BLOCK_ALL            0x07
#define SECTOR_ITEM                 0x08
#define SECTOR_UNDERGROUND_CREATURE 0x10
#define SECTOR_GROUND_CREATURE      0x20
#define SECTOR_FLYING_CREATURE      0x40
#define SECTOR_PORTAL               0x80

/* Zone 유형 */
typedef enum ZoneType {
    ZONE_TYPE_NORMAL = 0,
    ZONE_TYPE_SLAYER_GUILD,
    ZONE_TYPE_RESERVED_SLAYER_GUILD,
    ZONE_TYPE_NPC_VAMPIRE_LAIR,
    ZONE_TYPE_PC_VAMPIRE_LAIR,
    ZONE_TYPE_NPC_HOME,
    ZONE_TYPE_NPC_SHOP,
    ZONE_TYPE_RANDOMAP
} ZoneType;

/* ImageObject 유형 (MObject::OBJECT_TYPE 숫자 값과 일치) */
typedef enum ImageObjectType {
    OBJECT_TYPE_OBJECT = 0,              /* 기본 */
    OBJECT_TYPE_CREATURE = 1,
    OBJECT_TYPE_ITEM = 2,
    OBJECT_TYPE_IMAGEOBJECT = 3,
    OBJECT_TYPE_SHADOWOBJECT = 4,
    OBJECT_TYPE_ANIMATIONOBJECT = 5,
    OBJECT_TYPE_SHADOWANIMATIONOBJECT = 6,
    OBJECT_TYPE_INTERACTIONOBJECT = 7,
    OBJECT_TYPE_PORTAL = 8,
    OBJECT_TYPE_EFFECT = 9
} ImageObjectType;

/* 에러 코드 */
typedef enum ZoneError {
    ZONE_OK = 0,
    ZONE_ERR_IO = -1,
    ZONE_ERR_FORMAT = -2,
    ZONE_ERR_VERSION = -3,
    ZONE_ERR_NOMEM = -4,
    ZONE_ERR_PARAM = -5
} ZoneError;

/* ============================================================================
 * 데이터 구조체
 * ============================================================================ */

/**
 * Zone 파일 헤더 정보
 */
typedef struct ZoneHeader {
    char version[ZONE_VERSION_LEN + 1];  /* 버전 문자열 */
    uint16_t zone_id;                     /* Zone ID */
    uint16_t zone_group_id;               /* Zone 그룹 ID */
    char zone_name[ZONE_MAX_NAME_LEN];    /* Zone 이름 */
    uint8_t zone_type;                    /* Zone 유형 (ZoneType 열거형) */
    uint8_t zone_level;                   /* Zone 레벨 (1-10) */
    char description[ZONE_MAX_DESC_LEN];  /* Zone 설명 */
} ZoneHeader;

/**
 * 단일 섹터 (타일) 데이터
 */
typedef struct Sector {
    uint16_t sprite_id;   /* 타일 스프라이트 ID (0xFFFF = 비어있음) */
    uint8_t property;     /* 속성 플래그 */
    uint8_t light;        /* 조명 레벨 */
} Sector;

/**
 * 섹터 그리드 내의 ImageObject 위치
 */
typedef struct ImageObjectPosition {
    uint16_t x;
    uint16_t y;
} ImageObjectPosition;

/**
 * ImageObject 데이터
 */
typedef struct ImageObject {
    uint8_t type;                  /* ImageObjectType */
    uint32_t id;                   /* 인스턴스 오브젝트 ID (MObject::m_ID) */
    uint16_t sector_x;             /* 섹터 X (MObject::m_X) */
    uint16_t sector_y;             /* 섹터 Y (MObject::m_Y) */
    uint32_t image_object_id;      /* 이미지 클래스 ID (MImageObject::m_ImageObjectID) */
    uint16_t sprite_id;            /* 스프라이트 ID */
    int32_t pixel_x;               /* 픽셀 X 위치 */
    int32_t pixel_y;               /* 픽셀 Y 위치 */
    uint16_t viewpoint;            /* 시점 (SIZE_SECTORPOSITION) */
    uint8_t is_animation;          /* MImageObject::m_bAnimation */
    uint8_t trans_flags;           /* MImageObject::m_bTrans (비트 플래그) */
    
    /* 위치 목록 - 이 오브젝트가 차지하는 섹터들 */
    uint32_t position_count;
    ImageObjectPosition* positions;
    
    /* 유형별 특화 데이터 */
    union {
        struct {
            /* 원본 그림자 오브젝트에는 추가 바이너리 필드가 없음 */
            uint16_t reserved;
        } shadow;
        struct {
            /* Map 파일에 저장된 frame_id (TYPE_FRAMEID, 2바이트) 및 max_frame (1바이트) */
            uint16_t frame_id;
            uint8_t  max_frame;
            /* MAnimationObject의 선택적 런타임 필드 */
            uint8_t  blt_type;
            uint8_t  direction;
            uint8_t  sound_frame;
            uint16_t sound_id;
            uint8_t  loop;
            uint32_t min_delay;
            uint32_t max_delay;
            uint8_t  start_hour;
            uint8_t  end_hour;
        } animation;
        struct {
            uint16_t interaction_type;
        } interaction;
    } extra;
} ImageObject;

/**
 * 전체 Zone 데이터
 */
typedef struct Zone {
    ZoneHeader header;
    
    uint16_t width;               /* 섹터 단위 Zone 너비 */
    uint16_t height;              /* 섹터 단위 Zone 높이 */
    
    Sector* sectors;              /* 섹터 그리드 (height * width) */
    
    uint32_t image_object_count;
    ImageObject* image_objects;
    
    /* 지연 로딩을 위한 파일 위치 */
    long fp_tile;                 /* 타일 데이터 파일 오프셋 */
    long fp_image_object;         /* 이미지 오브젝트 파일 오프셋 */
} Zone;

/* ============================================================================
 * API 함수 선언
 * ============================================================================ */

/**
 * 파일 경로에서 Zone 로드
 * @param path .map 파일 경로
 * @param zone 채워 넣을 Zone 구조체 포인터
 * @return 성공 시 ZONE_OK, 실패 시 에러 코드
 */
Error* zone_load(const char* path, Zone* zone);

/**
 * 파일 스트림에서 Zone 로드
 * @param file 열려 있는 파일 스트림
 * @param zone 채워 넣을 Zone 구조체 포인터
 * @return 성공 시 ZONE_OK, 실패 시 에러 코드
 */
Error* zone_load_from_file(FILE* file, Zone* zone);

/**
 * Zone 헤더만 로드 (섹터 데이터 제외, 고속)
 * @param path .map 파일 경로
 * @param header 채워 넣을 ZoneHeader 포인터
 * @return 성공 시 ZONE_OK, 실패 시 에러 코드
 */
Error* zone_load_header(const char* path, ZoneHeader* header);

/**
 * Zone 리소스 해제
 * @param zone Zone 구조체 포인터
 */
void zone_free(Zone* zone);

/**
 * 특정 위치의 섹터 반환
 * @param zone Zone 포인터
 * @param x X 좌표
 * @param y Y 좌표
 * @return 섹터 포인터, 범위를 벗어난 경우 NULL
 */
const Sector* zone_get_sector(const Zone* zone, uint16_t x, uint16_t y);

/**
 * 지상 크리처가 이동 가능한 위치인지 확인
 * @param zone Zone 포인터
 * @param x X 좌표
 * @param y Y 좌표
 * @return 이동 가능하면 true
 */
bool zone_can_walk_ground(const Zone* zone, uint16_t x, uint16_t y);

/**
 * 비행 크리처가 이동 가능한 위치인지 확인
 * @param zone Zone 포인터
 * @param x X 좌표
 * @param y Y 좌표
 * @return 이동 가능하면 true
 */
bool zone_can_walk_flying(const Zone* zone, uint16_t x, uint16_t y);

/**
 * 지하 크리처가 이동 가능한 위치인지 확인
 * @param zone Zone 포인터
 * @param x X 좌표
 * @param y Y 좌표
 * @return 이동 가능하면 true
 */
bool zone_can_walk_underground(const Zone* zone, uint16_t x, uint16_t y);

/**
 * 에러 문자열 반환
 * @param err 에러 코드
 * @return 사람이 읽을 수 있는 에러 설명 문자열
 */
const char* zone_strerror(ZoneError err);

/** 파서 내부의 상세 디버그 로깅 활성화 또는 비활성화 */
void zone_set_debug(int enable);

#ifdef __cplusplus
}
#endif

#endif /* ZONE_H */
