#pragma once

#include "window.h"
#include "Sound/CaptureSound.h"
#include "Sound/OutputSound.h"
#include "MMD/DrawMMD.h"
#include "State/StateManager.h"

#include "Define.h"

#include <vector>

class ManageMMD
{
public:
    HRESULT Initialize();
    HRESULT Process();
    void Exit();

    void LoadModel();

    bool IsRunning();


    void DrawFFT(WAVEFORMATEX wf);

private:
    CWindow m_Window;
    std::shared_ptr<CaptureSound> m_Capture;
    std::shared_ptr<OutputSound> m_Output;
    std::shared_ptr<DrawMMD> m_mmd;

    std::shared_ptr<StateManager<EState>> stateManager;
};