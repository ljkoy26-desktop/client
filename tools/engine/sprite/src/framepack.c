/**
 * @file framepack.c
 * @brief FramePack 구조체 구현부 (순수 C)
 *
 * 요구사항: 3.1, 3.2, 3.3
 */

#include "framepack.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * CreatureFramePack 구현부
 * ============================================================================ */

void creature_framepack_init(CreatureFramePack* pack)
{
    if (pack == NULL) return;
    pack->creatures.v.data = NULL;
    pack->creatures.v.len = 0;
    pack->creatures.v.cap = 0;
}

void creature_framepack_free(CreatureFramePack* pack)
{
    if (pack == NULL) return;
    
    /* 중첩된 각 ActionArray를 해제한다 */
    for (int i = 0; i < vecLen(&pack->creatures); i++) {
        ActionArray* aa = vecRef(&pack->creatures, i);
        action_array_free(aa);
    }
    
    vecFree(&pack->creatures);
}

int creature_framepack_size(const CreatureFramePack* pack)
{
    if (pack == NULL) return 0;
    return vecLen(&pack->creatures);
}

ActionArray* creature_framepack_get(CreatureFramePack* pack, int creature_type)
{
    if (pack == NULL || creature_type < 0 || creature_type >= vecLen(&pack->creatures)) {
        return NULL;
    }
    return vecRef(&pack->creatures, creature_type);
}

Frame* creature_framepack_get_frame(CreatureFramePack* pack, 
                                    int creature_type, int action, 
                                    int direction, int frame_idx)
{
    ActionArray* aa = creature_framepack_get(pack, creature_type);
    if (aa == NULL) return NULL;
    
    DirectionArray* da = action_array_get(aa, action);
    if (da == NULL) return NULL;
    
    FrameArray* fa = direction_array_get(da, direction);
    if (fa == NULL) return NULL;
    
    return frame_array_get(fa, frame_idx);
}

int creature_framepack_load(CreatureFramePack* pack, const char* filename)
{
    if (pack == NULL || filename == NULL) return 0;
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return 0;
    }
    
    int result = creature_framepack_load_from_file(pack, file);
    fclose(file);
    
    return result;
}

int creature_framepack_load_from_file(CreatureFramePack* pack, FILE* file)
{
    if (pack == NULL || file == NULL) return 0;
    
    /* pack 초기화 */
    creature_framepack_init(pack);

    /* size를 uint16_t(TYPE_FRAMEID)로 읽는다 */
    uint16_t size;
    if (fread(&size, sizeof(uint16_t), 1, file) != 1) {
        return 0;
    }

    if (size == 0) {
        return 1;  /* 빈 pack도 유효하다 */
    }

    /* 미리 할당 */
    vecInit(&pack->creatures, size);

    /* 각 ActionArray를 읽는다 */
    for (uint16_t i = 0; i < size; i++) {
        ActionArray aa;
        if (!action_array_load(&aa, file)) {
            creature_framepack_free(pack);
            return 0;
        }
        vecAppend(&pack->creatures, aa);
    }
    
    return 1;
}

int creature_framepack_save(const CreatureFramePack* pack, const char* filename)
{
    if (pack == NULL || filename == NULL) return 0;
    
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        return 0;
    }
    
    int result = creature_framepack_save_to_file(pack, file);
    fclose(file);
    
    return result;
}

int creature_framepack_save_to_file(const CreatureFramePack* pack, FILE* file)
{
    if (pack == NULL || file == NULL) return 0;
    
    /* size를 uint16_t로 쓴다 */
    uint16_t size = (uint16_t)vecLen(&pack->creatures);
    if (fwrite(&size, sizeof(uint16_t), 1, file) != 1) {
        return 0;
    }

    /* 각 ActionArray를 쓴다 */
    for (int i = 0; i < vecLen(&pack->creatures); i++) {
        ActionArray* aa = vecRef((vector(ActionArray)*)&pack->creatures, i);
        if (!action_array_save(aa, file)) {
            return 0;
        }
    }
    
    return 1;
}

void creature_framepack_info_to_file(const CreatureFramePack* pack, const char* filename)
{
    if (pack == NULL || filename == NULL) return;
    
    FILE* file = fopen(filename, "w");
    if (file == NULL) return;
    
    fprintf(file, "=== CreatureFramePack Info ===\n");
    fprintf(file, "Total CreatureTypes: %d\n\n", creature_framepack_size(pack));
    
    /* 모든 creature type을 순회한다 */
    for (int type = 0; type < vecLen(&pack->creatures); type++) {
        ActionArray* aa = vecRef((vector(ActionArray)*)&pack->creatures, type);

        if (action_array_size(aa) == 0) {
            fprintf(file, "[%d] (empty)\n", type);
            continue;
        }

        /* 모든 action을 순회한다 */
        for (int action = 0; action < action_array_size(aa); action++) {
            DirectionArray* da = action_array_get(aa, action);

            if (direction_array_size(da) == 0) {
                continue;
            }

            /* 모든 direction을 순회한다 */
            for (int dir = 0; dir < direction_array_size(da); dir++) {
                FrameArray* fa = direction_array_get(da, dir);

                if (frame_array_size(fa) == 0) {
                    continue;
                }

                /* 모든 frame을 순회한다 */
                for (int frame_idx = 0; frame_idx < frame_array_size(fa); frame_idx++) {
                    Frame* f = frame_array_get(fa, frame_idx);
                    
                    fprintf(file, "[%d][%d][%d][%d] = ( %d, %d, %d )\n",
                            type, action, dir, frame_idx,
                            f->sprite_id, f->cx, f->cy);
                }
            }
        }
    }
    
    fclose(file);
}

/* ============================================================================
 * DirectionFramePack 구현부
 * ============================================================================ */

void direction_framepack_init(DirectionFramePack* pack)
{
    if (pack == NULL) return;
    pack->types.v.data = NULL;
    pack->types.v.len = 0;
    pack->types.v.cap = 0;
}

void direction_framepack_free(DirectionFramePack* pack)
{
    if (pack == NULL) return;
    
    /* 중첩된 각 DirectionArray를 해제한다 */
    for (int i = 0; i < vecLen(&pack->types); i++) {
        DirectionArray* da = vecRef(&pack->types, i);
        direction_array_free(da);
    }
    
    vecFree(&pack->types);
}

int direction_framepack_size(const DirectionFramePack* pack)
{
    if (pack == NULL) return 0;
    return vecLen(&pack->types);
}

DirectionArray* direction_framepack_get(DirectionFramePack* pack, int type_idx)
{
    if (pack == NULL || type_idx < 0 || type_idx >= vecLen(&pack->types)) {
        return NULL;
    }
    return vecRef(&pack->types, type_idx);
}

int direction_framepack_load(DirectionFramePack* pack, const char* filename)
{
    if (pack == NULL || filename == NULL) return 0;
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return 0;
    }
    
    int result = direction_framepack_load_from_file(pack, file);
    fclose(file);
    
    return result;
}

int direction_framepack_load_from_file(DirectionFramePack* pack, FILE* file)
{
    if (pack == NULL || file == NULL) return 0;
    
    /* pack 초기화 */
    direction_framepack_init(pack);

    /* size를 uint16_t로 읽는다 */
    uint16_t size;
    if (fread(&size, sizeof(uint16_t), 1, file) != 1) {
        return 0;
    }

    if (size == 0) {
        return 1;
    }

    /* 미리 할당 */
    vecInit(&pack->types, size);

    /* 각 DirectionArray를 읽는다 */
    for (uint16_t i = 0; i < size; i++) {
        DirectionArray da;
        if (!direction_array_load(&da, file)) {
            direction_framepack_free(pack);
            return 0;
        }
        vecAppend(&pack->types, da);
    }
    
    return 1;
}

/* ============================================================================
 * AnimationFramePack 구현부
 * ============================================================================ */

void animation_framepack_init(AnimationFramePack* pack)
{
    if (pack == NULL) return;
    pack->types.v.data = NULL;
    pack->types.v.len = 0;
    pack->types.v.cap = 0;
}

void animation_framepack_free(AnimationFramePack* pack)
{
    if (pack == NULL) return;
    
    /* 중첩된 각 FrameArray를 해제한다 */
    for (int i = 0; i < vecLen(&pack->types); i++) {
        FrameArray* fa = vecRef(&pack->types, i);
        frame_array_free(fa);
    }
    
    vecFree(&pack->types);
}

int animation_framepack_size(const AnimationFramePack* pack)
{
    if (pack == NULL) return 0;
    return vecLen(&pack->types);
}

FrameArray* animation_framepack_get(AnimationFramePack* pack, int type_idx)
{
    if (pack == NULL || type_idx < 0 || type_idx >= vecLen(&pack->types)) {
        return NULL;
    }
    return vecRef(&pack->types, type_idx);
}

int animation_framepack_load(AnimationFramePack* pack, const char* filename)
{
    if (pack == NULL || filename == NULL) return 0;
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return 0;
    }
    
    int result = animation_framepack_load_from_file(pack, file);
    fclose(file);
    
    return result;
}

int animation_framepack_load_from_file(AnimationFramePack* pack, FILE* file)
{
    if (pack == NULL || file == NULL) return 0;
    
    /* pack 초기화 */
    animation_framepack_init(pack);

    /* size를 uint16_t로 읽는다 */
    uint16_t size;
    if (fread(&size, sizeof(uint16_t), 1, file) != 1) {
        return 0;
    }

    if (size == 0) {
        return 1;
    }

    /* 미리 할당 */
    vecInit(&pack->types, size);

    /* 각 FrameArray를 읽는다 */
    for (uint16_t i = 0; i < size; i++) {
        FrameArray fa;
        if (!frame_array_load(&fa, file)) {
            animation_framepack_free(pack);
            return 0;
        }
        vecAppend(&pack->types, fa);
    }
    
    return 1;
}

/* ============================================================================
 * ImageFramePack 구현부
 * ============================================================================ */

void image_framepack_init(ImageFramePack* pack)
{
    if (pack == NULL) return;
    pack->frames.v.data = NULL;
    pack->frames.v.len = 0;
    pack->frames.v.cap = 0;
}

void image_framepack_free(ImageFramePack* pack)
{
    if (pack == NULL) return;
    vecFree(&pack->frames);
}

int image_framepack_size(const ImageFramePack* pack)
{
    if (pack == NULL) return 0;
    return vecLen(&pack->frames);
}

Frame* image_framepack_get(ImageFramePack* pack, int index)
{
    if (pack == NULL || index < 0 || index >= vecLen(&pack->frames)) {
        return NULL;
    }
    return vecRef(&pack->frames, index);
}

int image_framepack_load(ImageFramePack* pack, const char* filename)
{
    if (pack == NULL || filename == NULL) return 0;
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return 0;
    }
    
    int result = image_framepack_load_from_file(pack, file);
    fclose(file);
    
    return result;
}

int image_framepack_load_from_file(ImageFramePack* pack, FILE* file)
{
    if (pack == NULL || file == NULL) return 0;
    
    /* pack 초기화 */
    image_framepack_init(pack);

    /* size를 uint16_t로 읽는다 */
    uint16_t size;
    if (fread(&size, sizeof(uint16_t), 1, file) != 1) {
        return 0;
    }

    if (size == 0) {
        return 1;
    }

    /* 미리 할당 */
    vecInit(&pack->frames, size);

    /* 각 Frame을 읽는다 */
    for (uint16_t i = 0; i < size; i++) {
        Frame f;
        if (!frame_load(&f, file)) {
            image_framepack_free(pack);
            return 0;
        }
        vecAppend(&pack->frames, f);
    }
    
    return 1;
}
