#pragma once

#include "DxLib.h"

#include "PlayAnim.h"

#include "State/StateManager.h"
#include "Define.h"

class DrawMMD
{
public:
    void preInitialize();
    void afterInitialize();
    int mainProcess();

    void LoadModel();

    inline void SetStateManager(std::shared_ptr<StateManager<EState>> stateManager)
    {
        m_StateManager = stateManager;
    }

    inline int GetModelHandle()
    {
        return model;
    }

    inline void SetCharactorPos(const DxLib::VECTOR& pos)
    {
        charaPos = pos;
    }

    inline DxLib::VECTOR GetCharactorPos()
    {
        return charaPos;
    }

    void UpdatePosRot();

    float Zoom = 0;
    float RotateY = 0;
    DxLib::VECTOR cameraPos = VGet(0.f, 0.f, -10.f);

private:
    int model = 0;//モデルハンドル
    DxLib::VECTOR charaPos = VGet(0.f, 0.f, 1.f);//キャラクタの座標

    LONG dispWidth = 0;
    LONG dispHeight = 0;

    PlayAnim blink;

    std::shared_ptr<StateManager<EState>> m_StateManager;

    bool isDraw = true;
};