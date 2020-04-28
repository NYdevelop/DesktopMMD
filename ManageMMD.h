#pragma once

#include "window.h"
#include "CaptureSound.h"
#include "DrawMMD.h"

#include <vector>

class ManageMMD
{
public:
    void Debug();
    HRESULT Process();
    void Exit();

    bool IsRunning();

    void DrawFFT(WAVEFORMATEX wf);

private:
    CWindow m_Window;
    CaptureSound m_Capture;
    DrawMMD m_mmd;
};