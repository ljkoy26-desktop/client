/**
 * @file animation.c
 * @brief 애니메이션 프레임 및 오브젝트 관리 구현부
 *
 * 요구사항: 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 3.1, 3.2, 3.3, 3.5, 3.6, 6.1, 6.2
 */

#include "animation.h"
#include "color.h"
#include <string.h>

/* ============================================================================
 * 애니메이션 프레임 구현부
 * ============================================================================ */

void anim_frame_init(AnimFrame* frame, uint8_t blt_type) {
    if (frame == NULL) {
        return;
    }
    
    frame->frame_id = FRAMEID_NULL;
    frame->current_frame = 0;
    frame->max_frame = 1;
    frame->blt_type = blt_type;
    frame->loop = 0;
}

void anim_frame_set(AnimFrame* frame, FrameID id, uint8_t max_frame) {
    if (frame == NULL) {
        return;
    }
    
    frame->frame_id = id;
    frame->max_frame = (max_frame > 0) ? max_frame : 1;
    frame->current_frame = 0;
}

void anim_frame_next(AnimFrame* frame) {
    if (frame == NULL || frame->max_frame == 0) {
        return;
    }
    
    frame->current_frame++;
    if (frame->current_frame >= frame->max_frame) {
        frame->current_frame = 0;
    }
}

void anim_frame_next_loop(AnimFrame* frame, uint32_t loop_counter) {
    if (frame == NULL || frame->max_frame == 0) {
        return;
    }
    
    frame->current_frame = (uint8_t)(loop_counter % frame->max_frame);
}

uint8_t anim_frame_get(const AnimFrame* frame) {
    if (frame == NULL) {
        return 0;
    }
    
    return frame->current_frame;
}

/* ============================================================================
 * 애니메이션 오브젝트 구현부
 * ============================================================================ */

void anim_object_init(AnimObject* obj) {
    if (obj == NULL) {
        return;
    }
    
    anim_frame_init(&obj->frame, BLT_NORMAL);
    obj->sprite_id = SPRITEID_NULL;
    obj->pixel_x = 0;
    obj->pixel_y = 0;
    obj->direction = 0;
    obj->transparent = 0;
}

void anim_object_set(AnimObject* obj, SpriteID sprite_id, 
                     int px, int py, uint8_t direction, uint8_t trans) {
    if (obj == NULL) {
        return;
    }
    
    obj->sprite_id = sprite_id;
    obj->pixel_x = px;
    obj->pixel_y = py;
    obj->direction = direction % DIR_MAX;
    obj->transparent = trans;
}

void anim_object_set_frame(AnimObject* obj, FrameID frame_id, uint8_t max_frame) {
    if (obj == NULL) {
        return;
    }
    
    anim_frame_set(&obj->frame, frame_id, max_frame);
}

void anim_object_next_frame(AnimObject* obj) {
    if (obj == NULL) {
        return;
    }
    
    anim_frame_next(&obj->frame);
}

SpriteID anim_object_get_sprite(const AnimObject* obj) {
    if (obj == NULL || obj->sprite_id == SPRITEID_NULL) {
        return SPRITEID_NULL;
    }
    
    return obj->sprite_id + anim_frame_get(&obj->frame);
}

void anim_object_set_direction(AnimObject* obj, uint8_t dir) {
    if (obj == NULL) {
        return;
    }
    
    obj->direction = dir % DIR_MAX;
}

/* ============================================================================
 * 애니메이션 렌더링 구현부
 * ============================================================================ */

void anim_set_blend_mode(SDL_Texture* texture, BltType blt_type) {
    if (texture == NULL) {
        return;
    }
    
    switch (blt_type) {
        case BLT_NORMAL:
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            SDL_SetTextureColorMod(texture, 255, 255, 255);
            SDL_SetTextureAlphaMod(texture, 255);
            break;
            
        case BLT_EFFECT:
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
            SDL_SetTextureColorMod(texture, 255, 255, 255);
            SDL_SetTextureAlphaMod(texture, 255);
            break;
            
        case BLT_SHADOW:
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            /* 그림자 효과를 위해 텍스처를 어둡게 만든다 */
            SDL_SetTextureColorMod(texture, 0, 0, 0);
            SDL_SetTextureAlphaMod(texture, 128);
            break;

        case BLT_SCREEN:
            /* 스크린 블렌드를 가산 블렌딩으로 근사한다 */
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
            SDL_SetTextureColorMod(texture, 255, 255, 255);
            SDL_SetTextureAlphaMod(texture, 255);
            break;

        default:
            /* 기본값은 일반 블렌딩 */
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            SDL_SetTextureColorMod(texture, 255, 255, 255);
            SDL_SetTextureAlphaMod(texture, 255);
            break;
    }
}

int anim_render(SDL_Renderer* renderer, 
                const AnimObject* obj,
                SpritePack* pack,
                DecodedSprite* cache,
                uint16_t colorkey,
                float zoom) {
    if (obj == NULL) {
        return -1;
    }
    
    return anim_render_blt(renderer, obj, pack, cache, colorkey, zoom, 
                           (BltType)obj->frame.blt_type);
}

int anim_render_blt(SDL_Renderer* renderer,
                    const AnimObject* obj,
                    SpritePack* pack,
                    DecodedSprite* cache,
                    uint16_t colorkey,
                    float zoom,
                    BltType blt_type) {
    SpriteID sprite_id;
    Sprite* sprite;
    DecodedSprite local_decoded;
    DecodedSprite* decoded;
    SDL_Rect dst_rect;
    int result = 0;
    
    if (renderer == NULL || obj == NULL || pack == NULL) {
        return -1;
    }
    
    /* 현재 스프라이트 ID를 가져온다 */
    sprite_id = anim_object_get_sprite(obj);
    if (sprite_id == SPRITEID_NULL) {
        return -1;
    }

    /* 팩에 스프라이트가 존재하는지 확인한다 */
    if (sprite_id >= pack->count) {
        return -1;
    }

    /* 캐시가 제공되면 사용하고, 아니면 로컬 디코딩된 스프라이트를 사용한다 */
    decoded = cache ? cache : &local_decoded;

    /* 캐시를 사용하지 않거나 캐시가 비어있으면 스프라이트를 로드하고 디코딩한다 */
    if (!cache || cache->pixels == NULL) {
        sprite = spritepack_get(pack, sprite_id);
        if (sprite == NULL || !sprite->is_valid) {
            return -1;
        }

        if (sprite_decode(sprite, decoded, colorkey) != 0) {
            return -1;
        }

        /* 텍스처 생성 */
        if (decoded_sprite_create_texture(decoded, renderer) != 0) {
            if (!cache) {
                decoded_sprite_free(decoded);
            }
            return -1;
        }
    }

    /* 블렌드 모드 설정 */
    anim_set_blend_mode(decoded->texture, blt_type);

    /* 대상 사각형 계산 */
    dst_rect.x = obj->pixel_x;
    dst_rect.y = obj->pixel_y;
    dst_rect.w = (int)(decoded->width * zoom);
    dst_rect.h = (int)(decoded->height * zoom);

    /* 렌더링 */
    if (SDL_RenderCopy(renderer, decoded->texture, NULL, &dst_rect) != 0) {
        result = -1;
    }

    /* 캐시를 사용하지 않으면 로컬 디코딩된 스프라이트를 정리한다 */
    if (!cache) {
        decoded_sprite_free(decoded);
    }
    
    return result;
}
