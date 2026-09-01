/**
 * @file framepack.h
 * @brief 애니메이션 데이터를 위한 FramePack 구조체 (순수 C 구현)
 * 
 * 요구사항: 3.1, 3.2, 3.3
 * 
 * FramePack은 다양한 엔티티 유형에 대한 애니메이션 프레임 데이터를 저장합니다:
 * - CreatureFramePack: [CreatureType][Action][Direction][Frame] -> Frame
 * - DirectionFramePack: [Type][Direction][Frame] -> Frame (이펙트용)
 * - AnimationFramePack: [Type][Frame] -> Frame (단순 애니메이션용)
 * - ImageFramePack: [Type] -> Frame (정적 이미지용)
 */

#ifndef FRAMEPACK_H
#define FRAMEPACK_H

#include <stdio.h>
#include <stdint.h>
#include "frame.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * CreatureFramePack - [CreatureType][Action][Direction][Frame]
 * ============================================================================ */

/**
 * CreatureFramePack 구조체
 * 
 * 모든 크리처 유형에 대한 애니메이션 프레임 데이터를 저장합니다.
 * 구조: [CreatureType][Action][Direction][Frame] -> Frame
 */
typedef struct CreatureFramePack {
    vector(ActionArray) creatures;  /**< 크리처 유형별 ActionArray 배열 */
} CreatureFramePack;

/**
 * CreatureFramePack 초기화
 * @param pack CreatureFramePack 포인터
 */
void creature_framepack_init(CreatureFramePack* pack);

/**
 * CreatureFramePack 메모리 해제 (모든 중첩 배열 포함)
 * @param pack CreatureFramePack 포인터
 */
void creature_framepack_free(CreatureFramePack* pack);

/**
 * 크리처 유형 개수 반환
 * @param pack CreatureFramePack 포인터
 * @return 크리처 유형 개수
 */
int creature_framepack_size(const CreatureFramePack* pack);

/**
 * 특정 크리처 유형의 ActionArray 반환
 * @param pack CreatureFramePack 포인터
 * @param creature_type 크리처 유형 인덱스
 * @return ActionArray 포인터, 유효하지 않은 인덱스인 경우 NULL
 */
ActionArray* creature_framepack_get(CreatureFramePack* pack, int creature_type);

/**
 * 특정 프레임 반환
 * @param pack CreatureFramePack 포인터
 * @param creature_type 크리처 유형 인덱스
 * @param action 동작 인덱스
 * @param direction 방향 인덱스 (0-7)
 * @param frame_idx 프레임 인덱스
 * @return Frame 포인터, 유효하지 않은 인덱스인 경우 NULL
 */
Frame* creature_framepack_get_frame(CreatureFramePack* pack, 
                                    int creature_type, int action, 
                                    int direction, int frame_idx);

/**
 * 파일 경로에서 CreatureFramePack 로드
 * @param pack CreatureFramePack 포인터
 * @param filename .cfpk 파일 경로
 * @return 성공 시 1, 실패 시 0
 */
int creature_framepack_load(CreatureFramePack* pack, const char* filename);

/**
 * 파일 스트림에서 CreatureFramePack 로드
 * @param pack CreatureFramePack 포인터
 * @param file FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int creature_framepack_load_from_file(CreatureFramePack* pack, FILE* file);

/**
 * CreatureFramePack을 파일 경로에 저장
 * @param pack CreatureFramePack 포인터
 * @param filename 출력 파일 경로
 * @return 성공 시 1, 실패 시 0
 */
int creature_framepack_save(const CreatureFramePack* pack, const char* filename);

/**
 * CreatureFramePack을 파일 스트림에 저장
 * @param pack CreatureFramePack 포인터
 * @param file FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int creature_framepack_save_to_file(const CreatureFramePack* pack, FILE* file);

/**
 * 디버깅을 위해 FramePack 정보를 텍스트 파일로 출력
 * @param pack CreatureFramePack 포인터
 * @param filename 출력 파일 경로
 */
void creature_framepack_info_to_file(const CreatureFramePack* pack, const char* filename);

/* ============================================================================
 * DirectionFramePack - [Type][Direction][Frame]
 * ============================================================================ */

/**
 * DirectionFramePack 구조체
 * 
 * 방향 지원이 있는 애니메이션 프레임 데이터를 저장합니다.
 * 구조: [Type][Direction][Frame] -> Frame
 * 이펙트 및 기타 방향성 애니메이션에 사용됩니다.
 */
typedef struct {
    vector(DirectionArray) types;  /**< DirectionArray 배열 */
} DirectionFramePack;

/**
 * DirectionFramePack 초기화
 * @param pack DirectionFramePack 포인터
 */
void direction_framepack_init(DirectionFramePack* pack);

/**
 * DirectionFramePack 메모리 해제
 * @param pack DirectionFramePack 포인터
 */
void direction_framepack_free(DirectionFramePack* pack);

/**
 * 유형 개수 반환
 * @param pack DirectionFramePack 포인터
 * @return 유형 개수
 */
int direction_framepack_size(const DirectionFramePack* pack);

/**
 * 특정 유형의 DirectionArray 반환
 * @param pack DirectionFramePack 포인터
 * @param type_idx 유형 인덱스
 * @return DirectionArray 포인터, 유효하지 않은 인덱스인 경우 NULL
 */
DirectionArray* direction_framepack_get(DirectionFramePack* pack, int type_idx);

/**
 * 파일에서 DirectionFramePack 로드
 * @param pack DirectionFramePack 포인터
 * @param filename 파일 경로
 * @return 성공 시 1, 실패 시 0
 */
int direction_framepack_load(DirectionFramePack* pack, const char* filename);

/**
 * 파일 스트림에서 DirectionFramePack 로드
 * @param pack DirectionFramePack 포인터
 * @param file FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int direction_framepack_load_from_file(DirectionFramePack* pack, FILE* file);

/* ============================================================================
 * AnimationFramePack - [Type][Frame]
 * ============================================================================ */

/**
 * AnimationFramePack 구조체
 * 
 * 방향 구분이 없는 단순 애니메이션 프레임 데이터를 저장합니다.
 * 구조: [Type][Frame] -> Frame
 */
typedef struct {
    vector(FrameArray) types;  /**< FrameArray 배열 */
} AnimationFramePack;

/**
 * AnimationFramePack 초기화
 * @param pack AnimationFramePack 포인터
 */
void animation_framepack_init(AnimationFramePack* pack);

/**
 * AnimationFramePack 메모리 해제
 * @param pack AnimationFramePack 포인터
 */
void animation_framepack_free(AnimationFramePack* pack);

/**
 * 유형 개수 반환
 * @param pack AnimationFramePack 포인터
 * @return 유형 개수
 */
int animation_framepack_size(const AnimationFramePack* pack);

/**
 * 특정 유형의 FrameArray 반환
 * @param pack AnimationFramePack 포인터
 * @param type_idx 유형 인덱스
 * @return FrameArray 포인터, 유효하지 않은 인덱스인 경우 NULL
 */
FrameArray* animation_framepack_get(AnimationFramePack* pack, int type_idx);

/**
 * 파일에서 AnimationFramePack 로드
 * @param pack AnimationFramePack 포인터
 * @param filename 파일 경로
 * @return 성공 시 1, 실패 시 0
 */
int animation_framepack_load(AnimationFramePack* pack, const char* filename);

/**
 * 파일 스트림에서 AnimationFramePack 로드
 * @param pack AnimationFramePack 포인터
 * @param file FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int animation_framepack_load_from_file(AnimationFramePack* pack, FILE* file);

/* ============================================================================
 * ImageFramePack - [Type] -> Frame
 * ============================================================================ */

/**
 * ImageFramePack 구조체
 * 
 * 유형당 단일 프레임을 저장합니다 (정적 이미지용).
 * 구조: [Type] -> Frame
 */
typedef struct {
    vector(Frame) frames;  /**< Frame 배열 */
} ImageFramePack;

/**
 * ImageFramePack 초기화
 * @param pack ImageFramePack 포인터
 */
void image_framepack_init(ImageFramePack* pack);

/**
 * ImageFramePack 메모리 해제
 * @param pack ImageFramePack 포인터
 */
void image_framepack_free(ImageFramePack* pack);

/**
 * 프레임 개수 반환
 * @param pack ImageFramePack 포인터
 * @return 프레임 개수
 */
int image_framepack_size(const ImageFramePack* pack);

/**
 * 특정 인덱스의 Frame 반환
 * @param pack ImageFramePack 포인터
 * @param index 프레임 인덱스
 * @return Frame 포인터, 유효하지 않은 인덱스인 경우 NULL
 */
Frame* image_framepack_get(ImageFramePack* pack, int index);

/**
 * 파일에서 ImageFramePack 로드
 * @param pack ImageFramePack 포인터
 * @param filename 파일 경로
 * @return 성공 시 1, 실패 시 0
 */
int image_framepack_load(ImageFramePack* pack, const char* filename);

/**
 * 파일 스트림에서 ImageFramePack 로드
 * @param pack ImageFramePack 포인터
 * @param file FILE 포인터
 * @return 성공 시 1, 실패 시 0
 */
int image_framepack_load_from_file(ImageFramePack* pack, FILE* file);

#ifdef __cplusplus
}
#endif

#endif /* FRAMEPACK_H */
