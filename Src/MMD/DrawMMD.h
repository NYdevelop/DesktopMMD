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

    float Zoom = 0;
    float RotateY = 0;

private:
    int model = 0;//���f���n���h��
    DxLib::VECTOR charaPos = VGet(0, 0, 0);//�L�����N�^�̍��W

    LONG dispWidth = 0;
    LONG dispHeight = 0;

    PlayAnim blink;

    std::shared_ptr<StateManager<EState>> m_StateManager;

    bool isDraw = true;
};