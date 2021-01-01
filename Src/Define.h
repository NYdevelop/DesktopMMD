#pragma once

enum class EState
{
    STATE_WAIT = 0,
    STATE_RHYTHM,
    STATE_READ,
    STATE_DANCE,
    STATE_WALK,
    STATE_WAVE_HAND,
};

enum class EAnimIndex : int
{
    ANIM_NO_SOUND = 1,
    ANIM_BLINK,
    ANIM_UNAZUKI,
    ANIM_UNAZUKI_STABLE,
    ANIM_LIP = 6,
    ANIM_DANCE,
    ANIM_WALK = 9,
    ANIM_WAVE_HAND,
    ANIM_BREATH = 11,
};