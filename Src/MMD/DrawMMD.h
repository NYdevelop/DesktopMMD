#pragma once

#include "DxLib.h"

#include "MMD/Anim/PlayAnim.h"

#include "State/StateManager.h"
#include "MMD/Anim/ActionManager.h"
#include "Define.h"
#include "WindowGravity.h"

#include <string>

class DrawMMD
{
public:
    DrawMMD(const std::string& animPath, const std::string& modelPath,
        const float charaX, const float charaY, const float charaZ,
        const float charaDirect,
        const float cameraX, const float cameraY, const float cameraZ);

    void afterInitialize();

    int mainProcess();

    void Exit();

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
        std::cout << "zoom: " << m_Zoom << std::endl;
        cameraPos = VScale(VNorm(cameraPos), zoom);
    }

    float GetZoom() { return m_Zoom; };

    inline VECTOR GetRayVec()
    {
        return VSub(cameraPos, cameraViewPos);
    }

    float RotateY = 0;
    DxLib::VECTOR cameraPos = VGet(0.f, 0.f, 0.f);

    bool canViewCamera = false;

private:
    int model = 0;//���f���n���h��
    DxLib::VECTOR charaPos = VGet(0.f, 0.f, 1.f);//�L�����N�^�̍��W
    VECTOR cameraViewPos = VGet(.0f, .0f, .0f);
    VECTOR cameraViewOffset = VGet(0.f, 0.f, 1.f);//�J���������_
    float m_Zoom = 10.f;

    LONG dispWidth = 640;
    LONG dispHeight = 480;

    std::shared_ptr<StateManager<EState>> m_StateManager;

    bool isDraw = true;

    int boneHead = -1;
    MATRIX defHeadLocalRot = MGetIdent();

    WindowGravity gravity;

    std::string m_AnimPath;
    std::string m_ModelPath;
};