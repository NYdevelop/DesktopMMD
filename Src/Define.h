#pragma once

enum class EState
{
    STATE_WAIT = 0,
    STATE_RHYTHM,
    STATE_READ,
    STATE_DANCE,
};

enum class EContextMenu : int
{
    CONTEXT_MODE = 1,
    CONTEXT_MODE_WAIT = 11,
    CONTEXT_MODE_RHYTHM = 12,
    CONTEXT_MODE_READ = 13,
    CONTEXT_MODE_DANCE = 14,
    CONTEXT_EXIT = 2,
};