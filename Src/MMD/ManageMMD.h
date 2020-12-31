#pragma once

#include "window.h"
#include "Sound/CaptureSound.h"
#include "Sound/OutputSound.h"
#include "MMD/DrawMMD.h"
#include "MMD/Anim/PlayAnimQueue.h"
#include "State/StateManager.h"

#include "State/WalkStateManager.h"

#include <vector>
#include <string>

#include "Util\rapidxml-1.13\rapidxml.hpp"
#include "Util\rapidxml-1.13\rapidxml_utils.hpp"

class ManageMMD
{
public:
    HRESULT Initialize(const std::string& animPath, const std::string& modelPath,
        const float charaX = 0.f, const float charaY = 0.f, const float charaZ = 1.f,
        const float charaDirect = DX_PI_F,
        const float cameraX = 0.f, const float cameraY = 0.f, const float cameraZ = -35.f);

    HRESULT Process();

    void Exit();

    void LoadModel();

    bool IsRunning();


    void DrawFFT(WAVEFORMATEX wf);

private:
    void InitStateModel();

    static std::tuple<HMENU, ULONG, UINT, std::wstring> LoadContextNode(rapidxml::xml_node<>* node, HMENU& context);

    static void LoadContextNode(
        rapidxml::xml_node<>* node,
        HMENU context,
        std::vector<std::tuple<HMENU, ULONG, UINT, std::wstring>>& config,
        std::tuple<HMENU, ULONG, UINT, std::wstring>* sub = nullptr);

    CWindow m_Window;
    std::shared_ptr<CaptureSound> m_Capture;
    std::shared_ptr<OutputSound> m_Output;
    std::shared_ptr<DrawMMD> m_mmd;

    std::shared_ptr<StateManager<EState>> stateManager;
    std::shared_ptr<ActionManager> animManager;

    WalkStateManager walkManager;
};