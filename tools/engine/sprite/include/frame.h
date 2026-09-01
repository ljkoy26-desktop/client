/**
 * @file frame.h
 * @brief 애니메이션 데이터를 위한 프레임 구조체 (순수 C 구현)
 *
 * 요구사항: 3.4, 3.5
 *
 * Frame은 단일 애니메이션 프레임 데이터를 저장합니다:
 * - sprite_id: SpritePack 내의 인덱스
 * - cx, cy: 렌더링 좌표 오프셋
 */

#ifndef FRAME_H
#define FRAME_H

#include <stdio.h>
#include <stdint.h>
#include "vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * 타입 정의
 * ============================================================================ */

/**
 * 스프라이트 ID 타입 - SpritePack 내의 스프라이트를 식별
 */
typedef uint16_t TYPE_SPRITEID;

/**
 * 프레임 ID 타입 - FramePack 내의 프레임을 식별
 */
typedef uint16_t TYPE_FRAMEID;

/**
 * 무효/Null 스프라이트 ID 마커
 */
#define SPRITEID_NULL   0xFFFF

/**
 * 무효/Null 프레임 ID 마커
 */
#define FRAMEID_NULL    0xFFFF

/* ============================================================================
 * Frame 구조체
 * ============================================================================ */

/**
 * Frame 구조체 - 단일 프레임 데이터 저장
 *
 * 각 프레임의 구성:
 * - sprite_id: SpritePack 내의 인덱스 (0-65535)
 * - cx, cy: 렌더링 위치 좌표 오프셋
 */
typedef struct {
    TYPE_SPRITEID sprite_id;    /**< 스프라이트 인덱스 (0-65535) */
    int16_t cx;                 /**< X 좌표 오프셋 */
    int16_t cy;                 /**< Y 좌표 오프셋 */
} Frame;

/* ============================================================================
 * vector.h를 사용하는 Frame 배열 타입들
 * ============================================================================ */

/**
 * FrameArray: 단일 방향에 대한 프레임 배열
 * 프레임 번호로 인덱싱됨
 */
typedef vector(Frame) FrameArray;

/**
 * DirectionArray: 모든 방향에 대한 프레임 배열의 배열
 * 방향(0-7)으로 인덱싱됨
 */
typedef vector(FrameArray) DirectionArray;

/**
 * ActionArray: 모든 동작에 대한 방향 배열의 배열
 * 동작 타입(STAND, WALK, RUN 등)으로 인덱싱됨
 */
typedef vector(DirectionArray) ActionArray;

/* ============================================================================
 * Frame 함수
 * ============================================================================ */

/**
 * Frame 초기화
 * @param frame Frame 포인터
 * @param sprite_id 스프라이트 인덱스
 * @param cx X 좌표 오프셋
 * @param cy Y 좌표 오프셋
 */
void frame_init(Frame* frame, TYPE_SPRITEID sprite_id, int16_t cx, int16_t cy);

/**
 * Frame을 파일에 저장
 * @param frame Frame 포인터
 * @param file 저장할 FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int frame_save(const Frame* frame, FILE* file);

/**
 * 파일에서 Frame 로드
 * @param frame Frame 포인터
 * @param file 읽어올 FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int frame_load(Frame* frame, FILE* file);

/* ============================================================================
 * FrameArray 함수
 * ============================================================================ */

/**
 * FrameArray 초기화
 * @param arr FrameArray 포인터
 */
void frame_array_init(FrameArray* arr);

/**
 * FrameArray 메모리 해제
 * @param arr FrameArray 포인터
 */
void frame_array_free(FrameArray* arr);

/**
 * FrameArray 크기 반환
 * @param arr FrameArray 포인터
 * @return 프레임 개수
 */
int frame_array_size(const FrameArray* arr);

/**
 * 특정 인덱스의 Frame 반환
 * @param arr FrameArray 포인터
 * @param index 프레임 인덱스
 * @return Frame 포인터, 유효하지 않은 인덱스인 경우 NULL
 */
Frame* frame_array_get(const FrameArray* arr, int index);

/**
 * FrameArray를 파일에 저장
 * @param arr FrameArray 포인터
 * @param file FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int frame_array_save(const FrameArray* arr, FILE* file);

/**
 * 파일에서 FrameArray 로드
 * @param arr FrameArray 포인터
 * @param file FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int frame_array_load(FrameArray* arr, FILE* file);

/* ============================================================================
 * DirectionArray 함수
 * ============================================================================ */

/**
 * DirectionArray 초기화
 * @param arr DirectionArray 포인터
 */
void direction_array_init(DirectionArray* arr);

/**
 * DirectionArray 메모리 해제 (모든 중첩 FrameArray 포함)
 * @param arr DirectionArray 포인터
 */
void direction_array_free(DirectionArray* arr);

/**
 * DirectionArray 크기 반환
 * @param arr DirectionArray 포인터
 * @return 방향 개수
 */
int direction_array_size(const DirectionArray* arr);

/**
 * 특정 방향 인덱스의 FrameArray 반환
 * @param arr DirectionArray 포인터
 * @param index 방향 인덱스 (0-7)
 * @return FrameArray 포인터, 유효하지 않은 인덱스인 경우 NULL
 */
FrameArray* direction_array_get(DirectionArray* arr, int index);

/**
 * DirectionArray를 파일에 저장
 * @param arr DirectionArray 포인터
 * @param file FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int direction_array_save(const DirectionArray* arr, FILE* file);

/**
 * 파일에서 DirectionArray 로드
 * @param arr DirectionArray 포인터
 * @param file FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int direction_array_load(DirectionArray* arr, FILE* file);

/* ============================================================================
 * ActionArray 함수
 * ============================================================================ */

/**
 * ActionArray 초기화
 * @param arr ActionArray 포인터
 */
void action_array_init(ActionArray* arr);

/**
 * ActionArray 메모리 해제 (모든 중첩 배열 포함)
 * @param arr ActionArray 포인터
 */
void action_array_free(ActionArray* arr);

/**
 * ActionArray 크기 반환
 * @param arr ActionArray 포인터
 * @return 동작 개수
 */
int action_array_size(const ActionArray* arr);

/**
 * 특정 동작 인덱스의 DirectionArray 반환
 * @param arr ActionArray 포인터
 * @param index 동작 인덱스
 * @return DirectionArray 포인터, 유효하지 않은 인덱스인 경우 NULL
 */
DirectionArray* action_array_get(ActionArray* arr, int index);

/**
 * ActionArray를 파일에 저장
 * @param arr ActionArray 포인터
 * @param file FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int action_array_save(const ActionArray* arr, FILE* file);

/**
 * 파일에서 ActionArray 로드
 * @param arr ActionArray 포인터
 * @param file FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int action_array_load(ActionArray* arr, FILE* file);

#ifdef __cplusplus
}
#endif

#endif /* FRAME_H */
