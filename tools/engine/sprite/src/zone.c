/**
 * @file zone.c
 * @brief Zone (맵) 파일 파싱 구현부
 * 
 * 원본 MZone.cpp, ZoneFileHeader.cpp, MSector.cpp 기반
 */

#include "zone.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* 디버그 로깅 */
static int g_zone_debug = 0;
static void zdbg(FILE* f, const char* fmt, ...) {
    if (!g_zone_debug) return;
    long pos = ftell(f);
    fprintf(stderr, "[zone dbg] @0x%08lX ", pos);
    va_list ap; va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
}

void zone_set_debug(int enable) {
    g_zone_debug = enable ? 1 : 0;
}

/* ============================================================================
 * 내부 헬퍼 - 리틀 엔디언 읽기
 * ============================================================================ */

static int read_u8(FILE* f, uint8_t* out) {
    int c = fgetc(f);
    if (c == EOF) return -1;
    *out = (uint8_t)c;
    return 0;
}

static int read_u16_le(FILE* f, uint16_t* out) {
    uint8_t buf[2];
    if (fread(buf, 1, 2, f) != 2) return -1;
    *out = (uint16_t)(buf[0] | (buf[1] << 8));
    return 0;
}

static int read_u32_le(FILE* f, uint32_t* out) {
    uint8_t buf[4];
    if (fread(buf, 1, 4, f) != 4) return -1;
    *out = (uint32_t)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
    return 0;
}

static int read_i32_le(FILE* f, int32_t* out) {
    uint32_t u;
    if (read_u32_le(f, &u) != 0) return -1;
    *out = (int32_t)u;
    return 0;
}

/**
 * MString 포맷 읽기: 4바이트 길이 + 문자열 데이터 (파일 내 null 종단자 없음)
 */
static int read_mstring(FILE* f, char* out, size_t max_len) {
    uint32_t len;
    if (read_u32_le(f, &len) != 0) return -1;
    
    if (len == 0) {
        out[0] = '\0';
        return 0;
    }
    
    if (len >= max_len) {
        /* 문자열이 너무 길면 건너뛰기 */
        fseek(f, (long)len, SEEK_CUR);
        out[0] = '\0';
        return 0;
    }
    
    if (fread(out, 1, len, f) != len) return -1;
    out[len] = '\0';
    return 0;
}

/* ============================================================================
 * Zone 헤더 로딩
 * ============================================================================ */

static Error* load_zone_header(FILE* f, ZoneHeader* header) {
    memset(header, 0, sizeof(ZoneHeader));
    
    /* 버전 문자열 읽기 (MString 포맷) */
    if (read_mstring(f, header->version, ZONE_VERSION_LEN + 1) != 0) {
        return_err_code(ZONE_ERR_IO);
    }
    
    /* 버전 검증 */
    if (strcmp(header->version, ZONE_VERSION_STRING) != 0) {
        return_err_code(ZONE_ERR_VERSION);
    }
    
    /* Zone ID 읽기 */
    if (read_u16_le(f, &header->zone_id) != 0) return_err_code(ZONE_ERR_IO);
    
    /* Zone 그룹 ID 읽기 */
    if (read_u16_le(f, &header->zone_group_id) != 0) return_err_code(ZONE_ERR_IO);
    
    /* Zone 이름 읽기 */
    if (read_mstring(f, header->zone_name, ZONE_MAX_NAME_LEN) != 0) {
        return_err_code(ZONE_ERR_IO);
    }
    
    /* Zone 유형 읽기 */
    if (read_u8(f, &header->zone_type) != 0) return_err_code(ZONE_ERR_IO);
    
    /* Zone 레벨 읽기 */
    if (read_u8(f, &header->zone_level) != 0) return_err_code(ZONE_ERR_IO);
    
    /* 설명 읽기 */
    if (read_mstring(f, header->description, ZONE_MAX_DESC_LEN) != 0) {
        return_err_code(ZONE_ERR_IO);
    }
    
    return NULL;
}

/* ============================================================================
 * 섹터 로딩
 * ============================================================================ */

static Error*
load_sectors(FILE* f, Zone* zone) {
    uint32_t total = (uint32_t)zone->width * (uint32_t)zone->height;
    
    zone->sectors = (Sector*)calloc(total, sizeof(Sector));
    if (!zone->sectors) return_err_code(ZONE_ERR_NOMEM);
    
    for (uint32_t i = 0; i < total; i++) {
        Sector* s = &zone->sectors[i];
        
        if (read_u16_le(f, &s->sprite_id) != 0) return_err_code(ZONE_ERR_IO);
        if (read_u8(f, &s->property) != 0) return_err_code(ZONE_ERR_IO);
        if (read_u8(f, &s->light) != 0) return_err_code(ZONE_ERR_IO);
    }
    
    return NULL;
}

/* ============================================================================
 * ImageObject 로딩
 * ============================================================================ */

static void free_image_object(ImageObject* obj) {
    if (obj->positions) {
        free(obj->positions);
        obj->positions = NULL;
    }
}

static Error* load_image_object_base(FILE* f, ImageObject* obj, uint8_t type_first_byte) {
    /* 원본 포맷에서는 오브젝트 타입 바이트가 두 번 나타납니다:
       - 오브젝트 레코드 전(클래스 선택용)에 한 번
       - MObject::SaveToFile 시작 시 다시 한 번
       여기서 두 번째 바이트를 읽고 검증합니다. */
    uint8_t inner_type;
    if (read_u8(f, &inner_type) != 0) return_err_code(ZONE_ERR_IO);
    /* 변형 파일에서 치명적 실패를 방지하기 위해 불일치를 허용하되 계속 진행 */
    (void)type_first_byte;
    if (g_zone_debug && inner_type != type_first_byte) {
        zdbg(f, "warn: inner_type(%u) != outer_type(%u)", inner_type, type_first_byte);
    }

    /* 기본 MObject 필드 읽기: ID (4), 섹터 X (2), 섹터 Y (2) */
    if (read_u32_le(f, &obj->id) != 0) return_err_code(ZONE_ERR_IO);
    if (read_u16_le(f, &obj->sector_x) != 0) return_err_code(ZONE_ERR_IO);
    if (read_u16_le(f, &obj->sector_y) != 0) return_err_code(ZONE_ERR_IO);
    zdbg(f, "MObject: id=%u sx=%u sy=%u", obj->id, obj->sector_x, obj->sector_y);

    /* MImageObject 필드 읽기 */
    if (read_u32_le(f, &obj->image_object_id) != 0) return_err_code(ZONE_ERR_IO);
    if (read_u16_le(f, &obj->sprite_id) != 0) return_err_code(ZONE_ERR_IO);
    if (read_i32_le(f, &obj->pixel_x) != 0) return_err_code(ZONE_ERR_IO);
    if (read_i32_le(f, &obj->pixel_y) != 0) return_err_code(ZONE_ERR_IO);
    if (read_u16_le(f, &obj->viewpoint) != 0) return_err_code(ZONE_ERR_IO);
    if (read_u8(f, &obj->is_animation) != 0) return_err_code(ZONE_ERR_IO);
    if (read_u8(f, &obj->trans_flags) != 0) return_err_code(ZONE_ERR_IO);
    zdbg(f, "MImageObject: imgID=%u sprite=%u px=%d py=%d vp=%u anim=%u trans=0x%02X",
         obj->image_object_id, obj->sprite_id, obj->pixel_x, obj->pixel_y,
         obj->viewpoint, obj->is_animation, obj->trans_flags);

    return NULL;
}

static Error* load_image_object_positions(FILE* f, ImageObject* obj) {
    /* 원본 IMAGEOBJECT_POSITION_LIST는 2바이트 크기(WORD)를 사용 */
    uint16_t count16;
    if (read_u16_le(f, &count16) != 0) return_err_code(ZONE_ERR_IO);
    obj->position_count = count16;
    zdbg(f, "ImageObject positions count=%u", obj->position_count);

    if (obj->position_count == 0) {
        obj->positions = NULL;
        return NULL;
    }

    obj->positions = (ImageObjectPosition*)calloc(obj->position_count,
                                                  sizeof(ImageObjectPosition));
    if (!obj->positions) return_err_code(ZONE_ERR_NOMEM);

    uint32_t log_head = 16;
    uint32_t log_tail = 8;
    if (!g_zone_debug) { log_head = 0; log_tail = 0; }
    if (log_head > obj->position_count) log_head = obj->position_count;
    for (uint32_t i = 0; i < obj->position_count; i++) {
        if (read_u16_le(f, &obj->positions[i].x) != 0) return_err_code(ZONE_ERR_IO);
        if (read_u16_le(f, &obj->positions[i].y) != 0) return_err_code(ZONE_ERR_IO);
        if (g_zone_debug) {
            if (i < log_head) {
                zdbg(f, "  pos[%u]=(%u,%u)", i, obj->positions[i].x, obj->positions[i].y);
            } else if (i >= obj->position_count - log_tail) {
                zdbg(f, "  pos[%u]=(%u,%u)", i, obj->positions[i].x, obj->positions[i].y);
            }
        }
    }

    return NULL;
}

static Error* load_image_object(FILE* f, ImageObject* obj) {
    /* 선행 오브젝트 타입 바이트 읽기 */
    uint8_t type_first;
    if (read_u8(f, &type_first) != 0) return_err_code(ZONE_ERR_IO);
    obj->type = type_first;
    zdbg(f, "Object type=%u (outer)", obj->type);

    /* 기본 데이터 로드 (내부 타입 및 기본 필드 소비) */
    Error* err = load_image_object_base(f, obj, type_first);
    if (err != NULL) return_err_wrap(err);

    /* 원본 바이너리와 일치하도록 타입별 특화 데이터 로드 */
    switch (obj->type) {
        case OBJECT_TYPE_SHADOWOBJECT:
            /* 그림자 오브젝트는 원본에서 추가 직렬화 필드가 없음 */
            obj->extra.shadow.reserved = 0;
            zdbg(f, "ShadowObject: no extra fields");
            break;

        case OBJECT_TYPE_ANIMATIONOBJECT:
        case OBJECT_TYPE_SHADOWANIMATIONOBJECT: {
            /* CAnimationFrame::LoadFromFile: frame_id (2), max_frame (1) */
            if (read_u16_le(f, &obj->extra.animation.frame_id) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u8(f, &obj->extra.animation.max_frame) != 0)
                return_err_code(ZONE_ERR_IO);
            zdbg(f, "Animation: frame_id=%u max_frame=%u", obj->extra.animation.frame_id, obj->extra.animation.max_frame);

            /* MAnimationObject 추가 필드 */
            if (read_u8(f, &obj->extra.animation.blt_type) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u8(f, &obj->extra.animation.direction) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u8(f, &obj->extra.animation.sound_frame) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u16_le(f, &obj->extra.animation.sound_id) != 0)
                return_err_code(ZONE_ERR_IO);
            zdbg(f, "Animation: blt=%u dir=%u sound_fr=%u sound_id=%u",
                 obj->extra.animation.blt_type, obj->extra.animation.direction,
                 obj->extra.animation.sound_frame, obj->extra.animation.sound_id);

            /* ShowTimeChecker::LoadFromFile */
            if (read_u8(f, &obj->extra.animation.loop) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u32_le(f, &obj->extra.animation.min_delay) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u32_le(f, &obj->extra.animation.max_delay) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u8(f, &obj->extra.animation.start_hour) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u8(f, &obj->extra.animation.end_hour) != 0)
                return_err_code(ZONE_ERR_IO);
            zdbg(f, "ShowTime: loop=%u min=%u max=%u start=%u end=%u",
                 obj->extra.animation.loop, obj->extra.animation.min_delay,
                 obj->extra.animation.max_delay, obj->extra.animation.start_hour,
                 obj->extra.animation.end_hour);
            break;
        }

        case OBJECT_TYPE_INTERACTIONOBJECT:
        {
            /* 상호작용 오브젝트의 기본은 애니메이션 + 1개의 추가 타입 필드 */
            if (read_u16_le(f, &obj->extra.animation.frame_id) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u8(f, &obj->extra.animation.max_frame) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u8(f, &obj->extra.animation.blt_type) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u8(f, &obj->extra.animation.direction) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u8(f, &obj->extra.animation.sound_frame) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u16_le(f, &obj->extra.animation.sound_id) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u8(f, &obj->extra.animation.loop) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u32_le(f, &obj->extra.animation.min_delay) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u32_le(f, &obj->extra.animation.max_delay) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u8(f, &obj->extra.animation.start_hour) != 0)
                return_err_code(ZONE_ERR_IO);
            if (read_u8(f, &obj->extra.animation.end_hour) != 0)
                return_err_code(ZONE_ERR_IO);

            if (read_u16_le(f, &obj->extra.interaction.interaction_type) != 0)
                return_err_code(ZONE_ERR_IO);
            zdbg(f, "Interaction: type=%u", obj->extra.interaction.interaction_type);
            break;
        }

        case OBJECT_TYPE_IMAGEOBJECT:
        default:
            /* 추가 데이터 없음 */
            break;
    }

    return NULL;
}

static Error*
load_image_objects(FILE* f, Zone* zone) {
    int32_t count;
    if (read_i32_le(f, &count) != 0) return_err_code(ZONE_ERR_IO);
    
    if (count <= 0) {
        zone->image_object_count = 0;
        zone->image_objects = NULL;
        return NULL;
    }
    
    zone->image_object_count = (uint32_t)count;
    zone->image_objects = (ImageObject*)calloc(count, sizeof(ImageObject));
    if (!zone->image_objects) return_err_code(ZONE_ERR_NOMEM);
    zdbg(f, "ImageObjects: count=%d", count);
    
    for (int32_t i = 0; i < count; i++) {
        ImageObject* obj = &zone->image_objects[i];
        zdbg(f, "-- Object[%d] begin --", i);
        /* 오브젝트 데이터 로드 */
        Error *err = load_image_object(f, obj);
        if (err != NULL) return_err_wrap(err);
        zdbg(f, "Object[%d]: type=%u id=%u imgID=%u sprite=%u px=%d py=%d vp=%u anim=%u trans=0x%02X",
             i, obj->type, obj->id, obj->image_object_id, obj->sprite_id,
             obj->pixel_x, obj->pixel_y, obj->viewpoint, obj->is_animation, obj->trans_flags);
        /* 위치 목록 로드 */
        err = load_image_object_positions(f, obj);
        if (err != NULL) return_err_wrap(err);
        zdbg(f, "Object[%d]: positions_loaded=%u", i, obj->position_count);
        zdbg(f, "-- Object[%d] end --", i);
    }
    
    return NULL;
}

/* ============================================================================
 * 공개 API
 * ============================================================================ */

Error* zone_load(const char* path, Zone* zone) {
    if (!path || !zone) return_err_code(ZONE_ERR_PARAM);
    
    FILE* f = fopen(path, "rb");
    if (!f) return_err_code(ZONE_ERR_IO);
    
    Error* err = zone_load_from_file(f, zone);
    fclose(f);
    if (err != NULL) {
        return_err_wrap(err);
    }
    return NULL;
}

Error* zone_load_from_file(FILE* f, Zone* zone) {
    if (!f || !zone) return_err_code(ZONE_ERR_PARAM);
    
    memset(zone, 0, sizeof(Zone));
    
    /* 헤더 로드 */
    Error *err = load_zone_header(f, &zone->header);
    if (err != NULL) {
        zone_free(zone);
        return_err_wrap(err);
    }
    
    /* 파일 위치 읽기 (타일 및 이미지 오브젝트 데이터용) */
    uint32_t fp_tile, fp_image_object;
    if (read_u32_le(f, &fp_tile) != 0) {
        zone_free(zone);
        return_err_code(ZONE_ERR_IO);
    }
    if (read_u32_le(f, &fp_image_object) != 0) {
        zone_free(zone);
        return_err_code(ZONE_ERR_IO);
    }
    zone->fp_tile = (long)fp_tile;
    zone->fp_image_object = (long)fp_image_object;
    
    /* Zone 크기 읽기 */
    if (read_u16_le(f, &zone->width) != 0) {
        zone_free(zone);
        return_err_code(ZONE_ERR_IO);
    }
    if (read_u16_le(f, &zone->height) != 0) {
        zone_free(zone);
        return_err_code(ZONE_ERR_IO);
    }
    
    if (zone->width == 0 || zone->height == 0) {
        zone_free(zone);
        return_err_code(ZONE_ERR_FORMAT);
    }
    
    /* 섹터 로드 */
    err = load_sectors(f, zone);
    if (err != NULL) {
        zone_free(zone);
        return_err_wrap(err);
    }
    
    /* 이미지 오브젝트 로드 */
    err = load_image_objects(f, zone);
    if (err != NULL) {
        zone_free(zone);
        return_err_wrap(err);
    }
    
    return NULL;
}

Error* zone_load_header(const char* path, ZoneHeader* header) {
    if (!path || !header) return_err_code(ZONE_ERR_PARAM);
    
    FILE* f = fopen(path, "rb");
    if (!f) return_err_code(ZONE_ERR_IO);
    
    Error* err = load_zone_header(f, header);
    fclose(f);
    if (err != NULL) {
        return_err_wrap(err);
    }
    return NULL;
}

void zone_free(Zone* zone) {
    if (!zone) return;
    
    if (zone->sectors) {
        free(zone->sectors);
        zone->sectors = NULL;
    }
    
    if (zone->image_objects) {
        for (uint32_t i = 0; i < zone->image_object_count; i++) {
            free_image_object(&zone->image_objects[i]);
        }
        free(zone->image_objects);
        zone->image_objects = NULL;
    }
    
    zone->image_object_count = 0;
    zone->width = 0;
    zone->height = 0;
}

const Sector* zone_get_sector(const Zone* zone, uint16_t x, uint16_t y) {
    if (!zone || !zone->sectors) return NULL;
    if (x >= zone->width || y >= zone->height) return NULL;
    
    return &zone->sectors[(uint32_t)y * zone->width + x];
}

bool zone_can_walk_ground(const Zone* zone, uint16_t x, uint16_t y) {
    const Sector* s = zone_get_sector(zone, x, y);
    if (!s) return false;
    return (s->property & SECTOR_BLOCK_GROUND) == 0;
}

bool zone_can_walk_flying(const Zone* zone, uint16_t x, uint16_t y) {
    const Sector* s = zone_get_sector(zone, x, y);
    if (!s) return false;
    return (s->property & SECTOR_BLOCK_FLYING) == 0;
}

bool zone_can_walk_underground(const Zone* zone, uint16_t x, uint16_t y) {
    const Sector* s = zone_get_sector(zone, x, y);
    if (!s) return false;
    return (s->property & SECTOR_BLOCK_UNDERGROUND) == 0;
}

const char* zone_strerror(ZoneError err) {
    switch (err) {
        case ZONE_OK: return "OK";
        case ZONE_ERR_IO: return "입출력(I/O) 오류";
        case ZONE_ERR_FORMAT: return "유효하지 않은 포맷";
        case ZONE_ERR_VERSION: return "지원되지 않는 버전";
        case ZONE_ERR_NOMEM: return "메모리 부족";
        case ZONE_ERR_PARAM: return "유효하지 않은 매개변수";
        default: return "알 수 없는 오류";
    }
}
