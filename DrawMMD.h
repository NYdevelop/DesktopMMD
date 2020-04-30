#pragma once

#include "DxLib.h"

#include "PlayAnim.h"

#include "StateManager.h"
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

    float Zoom;
    float RotateY;

private:
    int model;//モデルハンドル
    DxLib::VECTOR charaPos;//キャラクタの座標

    LONG dispWidth;
    LONG dispHeight;

    PlayAnim blink;

    std::shared_ptr<StateManager<EState>> m_StateManager;

    bool isDraw = true;
};