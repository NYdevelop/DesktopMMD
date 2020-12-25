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

enum class EContextMenu : int
{
    CONTEXT_MODE = 1,
    CONTEXT_MODE_WAIT = 11,
    CONTEXT_MODE_RHYTHM = 12,
    CONTEXT_MODE_READ = 13,
    CONTEXT_MODE_DANCE = 14,
    CONTEXT_MODE_WAVE_HAND = 15,
    CONTEXT_MOVE_LEFT = 2,
    CONTEXT_MOVE_RIGHT = 3,
    CONTEXT_MOVE_RANDOM = 4,
    CONTEXT_EXIT = 0,
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
    ANIM_THINK2,
    ANIM_LOOK_SELF1,
    ANIM_LOOK_SELF2
};