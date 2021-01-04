#pragma once

#include "window.h"
#include "Sound/CaptureSound.h"
#include "Sound/OutputSound.h"
#include "MMD/DrawMMD.h"
#include "MMD/Anim/PlayAnimQueue.h"
#include "State/StateManager.h"
#include "State/WalkStateManager.h"
#include "Util\MouseDrag.h"

#include <vector>
#include <string>
#include <mutex>

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
    void InitStateModel();

    void InitMouseDrag();

    void InputKey();

    CWindow m_Window;
    std::shared_ptr<CaptureSound> m_Capture;
    std::shared_ptr<OutputSound> m_Output;
    std::shared_ptr<DrawMMD> m_mmd;

    std::shared_ptr<StateManager<EState>> stateManager;
    std::shared_ptr<ActionManager> animManager;

    WalkStateManager walkManager;

    MouseDrag mouseDrag;

    std::mutex modelMutex;
};