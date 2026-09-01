/**
 * @file frame.c
 * @brief Frame 구조체 구현부 (순수 C)
 *
 * 요구사항: 3.4, 3.5
 */

#include "frame.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * Frame 함수
 * ============================================================================ */

void frame_init(Frame* frame, TYPE_SPRITEID sprite_id, int16_t cx, int16_t cy)
{
    if (frame == NULL) return;

    frame->sprite_id = sprite_id;
    frame->cx = cx;
    frame->cy = cy;
}

int frame_save(const Frame* frame, FILE* file)
{
    if (frame == NULL || file == NULL) return 0;

    /* sprite_id 쓰기 (2바이트) */
    if (fwrite(&frame->sprite_id, sizeof(TYPE_SPRITEID), 1, file) != 1) {
        return 0;
    }

    /* cx 쓰기 (2바이트) */
    if (fwrite(&frame->cx, sizeof(int16_t), 1, file) != 1) {
        return 0;
    }

    /* cy 쓰기 (2바이트) */
    if (fwrite(&frame->cy, sizeof(int16_t), 1, file) != 1) {
        return 0;
    }

    return 1;
}

int frame_load(Frame* frame, FILE* file)
{
    if (frame == NULL || file == NULL) return 0;

    /* sprite_id 읽기 (2바이트) */
    if (fread(&frame->sprite_id, sizeof(TYPE_SPRITEID), 1, file) != 1) {
        return 0;
    }

    /* cx 읽기 (2바이트) */
    if (fread(&frame->cx, sizeof(int16_t), 1, file) != 1) {
        return 0;
    }

    /* cy 읽기 (2바이트) */
    if (fread(&frame->cy, sizeof(int16_t), 1, file) != 1) {
        return 0;
    }

    return 1;
}

/* ============================================================================
 * FrameArray 함수
 * ============================================================================ */

void frame_array_init(FrameArray* arr)
{
    if (arr == NULL) return;
    arr->v.data = NULL;
    arr->v.len = 0;
    arr->v.cap = 0;
}

void frame_array_free(FrameArray* arr)
{
    if (arr == NULL) return;
    vecFree(arr);
}

int frame_array_size(const FrameArray* arr)
{
    if (arr == NULL) return 0;
    return vecLen(arr);
}

Frame* frame_array_get(const FrameArray* arr, int index)
{
    if (arr == NULL || index < 0 || index >= vecLen(arr)) {
        return NULL;
    }
    return vecRef(arr, index);
}

int frame_array_save(const FrameArray* arr, FILE* file)
{
    if (arr == NULL || file == NULL) return 0;

    /* size를 uint16_t로 쓴다 (원본 포맷과 호환) */
    uint16_t size = (uint16_t)vecLen(arr);
    if (fwrite(&size, sizeof(uint16_t), 1, file) != 1) {
        return 0;
    }

    /* 각 frame을 쓴다 */
    for (int i = 0; i < vecLen(arr); i++) {
        Frame* frame = vecRef((FrameArray*)arr, i);
        if (!frame_save(frame, file)) {
            return 0;
        }
    }

    return 1;
}

int frame_array_load(FrameArray* arr, FILE* file)
{
    if (arr == NULL || file == NULL) return 0;

    /* 배열 초기화 */
    frame_array_init(arr);

    /* size를 uint16_t로 읽는다 */
    uint16_t size;
    if (fread(&size, sizeof(uint16_t), 1, file) != 1) {
        return 0;
    }

    if (size == 0) {
        return 1;  /* 빈 배열도 유효하다 */
    }

    /* 미리 할당 */
    vecInit(arr, size);

    /* 각 frame을 읽는다 */
    for (uint16_t i = 0; i < size; i++) {
        Frame frame;
        if (!frame_load(&frame, file)) {
            frame_array_free(arr);
            return 0;
        }
        vecAppend(arr, frame);
    }

    return 1;
}

/* ============================================================================
 * DirectionArray 함수
 * ============================================================================ */

void direction_array_init(DirectionArray* arr)
{
    if (arr == NULL) return;
    arr->v.data = NULL;
    arr->v.len = 0;
    arr->v.cap = 0;
}

void direction_array_free(DirectionArray* arr)
{
    if (arr == NULL) return;

    /* 중첩된 각 FrameArray를 해제한다 */
    for (int i = 0; i < vecLen(arr); i++) {
        FrameArray* fa = vecRef(arr, i);
        frame_array_free(fa);
    }

    vecFree(arr);
}

int direction_array_size(const DirectionArray* arr)
{
    if (arr == NULL) return 0;
    return vecLen(arr);
}

FrameArray* direction_array_get(DirectionArray* arr, int index)
{
    if (arr == NULL || index < 0 || index >= vecLen(arr)) {
        return NULL;
    }
    return vecRef(arr, index);
}

int direction_array_save(const DirectionArray* arr, FILE* file)
{
    if (arr == NULL || file == NULL) return 0;

    /* size를 uint8_t로 쓴다 (원본 포맷과 호환) */
    uint8_t size = (uint8_t)vecLen(arr);
    if (fwrite(&size, sizeof(uint8_t), 1, file) != 1) {
        return 0;
    }

    /* 각 FrameArray를 쓴다 */
    for (int i = 0; i < vecLen(arr); i++) {
        FrameArray* fa = vecRef((DirectionArray*)arr, i);
        if (!frame_array_save(fa, file)) {
            return 0;
        }
    }

    return 1;
}

int direction_array_load(DirectionArray* arr, FILE* file)
{
    if (arr == NULL || file == NULL) return 0;

    /* 배열 초기화 */
    direction_array_init(arr);

    /* size를 uint8_t로 읽는다 */
    uint8_t size;
    if (fread(&size, sizeof(uint8_t), 1, file) != 1) {
        return 0;
    }

    if (size == 0) {
        return 1;  /* 빈 배열도 유효하다 */
    }

    /* 미리 할당 */
    vecInit(arr, size);

    /* 각 FrameArray를 읽는다 */
    for (uint8_t i = 0; i < size; i++) {
        FrameArray fa;
        if (!frame_array_load(&fa, file)) {
            direction_array_free(arr);
            return 0;
        }
        vecAppend(arr, fa);
    }

    return 1;
}

/* ============================================================================
 * ActionArray 함수
 * ============================================================================ */

void action_array_init(ActionArray* arr)
{
    if (arr == NULL) return;
    arr->v.data = NULL;
    arr->v.len = 0;
    arr->v.cap = 0;
}

void action_array_free(ActionArray* arr)
{
    if (arr == NULL) return;

    /* 중첩된 각 DirectionArray를 해제한다 */
    for (int i = 0; i < vecLen(arr); i++) {
        DirectionArray* da = vecRef(arr, i);
        direction_array_free(da);
    }

    vecFree(arr);
}

int action_array_size(const ActionArray* arr)
{
    if (arr == NULL) return 0;
    return vecLen(arr);
}

DirectionArray* action_array_get(ActionArray* arr, int index)
{
    if (arr == NULL || index < 0 || index >= vecLen(arr)) {
        return NULL;
    }
    return vecRef(arr, index);
}

int action_array_save(const ActionArray* arr, FILE* file)
{
    if (arr == NULL || file == NULL) return 0;

    /* size를 uint8_t로 쓴다 (원본 포맷과 호환) */
    uint8_t size = (uint8_t)vecLen(arr);
    if (fwrite(&size, sizeof(uint8_t), 1, file) != 1) {
        return 0;
    }

    /* 각 DirectionArray를 쓴다 */
    for (int i = 0; i < vecLen(arr); i++) {
        DirectionArray* da = vecRef((ActionArray*)arr, i);
        if (!direction_array_save(da, file)) {
            return 0;
        }
    }

    return 1;
}

int action_array_load(ActionArray* arr, FILE* file)
{
    if (arr == NULL || file == NULL) return 0;

    /* 배열 초기화 */
    action_array_init(arr);

    /* size를 uint8_t로 읽는다 */
    uint8_t size;
    if (fread(&size, sizeof(uint8_t), 1, file) != 1) {
        return 0;
    }

    if (size == 0) {
        return 1;  /* 빈 배열도 유효하다 */
    }

    /* 미리 할당 */
    vecInit(arr, size);

    /* 각 DirectionArray를 읽는다 */
    for (uint8_t i = 0; i < size; i++) {
        DirectionArray da;
        if (!direction_array_load(&da, file)) {
            action_array_free(arr);
            return 0;
        }
        vecAppend(arr, da);
    }

    return 1;
}
