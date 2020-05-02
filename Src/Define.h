#pragma once

enum class EState
{
    STATE_WAIT = 0,
    STATE_RHYTHM,
    STATE_READ,
    STATE_DANCE,
    STATE_WALK,
};

enum class EContextMenu : int
{
    CONTEXT_MODE = 1,
    CONTEXT_MODE_WAIT = 11,
    CONTEXT_MODE_RHYTHM = 12,
    CONTEXT_MODE_READ = 13,
    CONTEXT_MODE_DANCE = 14,
    CONTEXT_MOVE_LEFT = 2,
    CONTEXT_MOVE_RIGHT = 3,
    CONTEXT_MOVE_RANDOM = 4,
    CONTEXT_EXIT = 0,
};