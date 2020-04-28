#pragma once

enum EState : int
{
    STATE_WAIT = 0,
    STATE_RHYTHM,
    STATE_READ,
};

enum EContextMenu : int
{
    CONTEXT_MODE = 1,
    CONTEXT_MODE_WAIT = 11,
    CONTEXT_MODE_RHYTHM = 12,
    CONTEXT_MODE_READ = 13,
    CONTEXT_EXIT = 2,
};