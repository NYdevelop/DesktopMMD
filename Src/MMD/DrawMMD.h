#pragma once

#include "DxLib.h"

#include "MMD/Anim/PlayAnim.h"

#include "State/StateManager.h"
#include "MMD/Anim/PlayAnimQueue.h"
#include "Define.h"
#include "WindowGravity.h"

#include <string>

class DrawMMD
{
public:
    DrawMMD(const std::string& animPath, const std::string& modelPath);
    void afterInitialize();
    int mainProcess();

    void LoadModel();

    inline void SetStateManager(std::shared_ptr<StateManager<EState>> stateManager)
    {
        m_StateManager = stateManager;
    }

    inline int GetModelHandle() { return model; }

    inline void SetCharactorPos(const DxLib::VECTOR& pos)
    {
        charaPos = pos;
    }

    inline DxLib::VECTOR GetCharactorPos() { return charaPos; }

    void UpdatePosRot();

    inline void SetZoom(float zoom)
    {
        m_Zoom = zoom;
        cameraPos = VScale(VNorm(cameraPos), zoom);
    }

    float GetZoom() { return m_Zoom; };

    inline void SetAnimQueue(std::shared_ptr<PlayAnimQueue> queue)
    {
        m_AnimQueue = queue;
    }

    inline VECTOR GetRayVec()
    {
        return VSub(cameraPos, cameraViewPos);
    }

    float RotateY = 0;
    DxLib::VECTOR cameraPos = VGet(0.f, 0.f, 0.f);

    bool canViewCamera = false;

private:
    int model = 0;//モデルハンドル
    DxLib::VECTOR charaPos = VGet(0.f, 0.f, 1.f);//キャラクタの座標
    VECTOR cameraViewPos = VGet(.0f, .0f, .0f);
    VECTOR cameraViewOffset = VGet(0.f, 0.f, 1.f);//カメラ注視点
    float m_Zoom = 10.f;

    LONG dispWidth = 640;
    LONG dispHeight = 480;

    PlayAnim blink;

    std::shared_ptr<PlayAnimQueue> m_AnimQueue;
    std::shared_ptr<StateManager<EState>> m_StateManager;

    bool isDraw = true;

    int boneHead = -1;
    MATRIX defHeadLocalRot = MGetIdent();

    WindowGravity gravity;

    std::string m_AnimPath;
    std::string m_ModelPath;
};