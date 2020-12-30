#pragma once

#include "StateMMD.h"

#include <memory>
#include <map>
#include <tuple>
#include "WalkStateManager.h"
#include "MMD/DrawMMD.h"

class WaitState : public StateMMD
{
public:
    void Initialize();
    void Doing();
    void End();
    inline int ModelInitial() { return 0; };

    void OnceInitial();

    inline void SetRandomMove(bool flag) { m_RandomMove = flag; };

    inline void SetDrawMMD(std::shared_ptr<DrawMMD> mmd)
    {
        m_mmd = mmd;
    }

    inline void SetWalkStateManager(WalkStateManager* m)
    {
        walkManager = m;
    }

private:
    void DoWaitAnim();
    void SetAnim(EAnimIndex index, bool isBlink = true, bool isBreath = true, int transFrame = 10);
    void loadConfig();

    bool m_RandomMove = false;
    bool m_IsWaitAnim = false;
    std::shared_ptr<DrawMMD> m_mmd;
    WalkStateManager* walkManager = nullptr;

    enum EAnimMap : int
    {
        ITEM_ANIM,
        ITEM_INDEX,
        ITEM_BLINK,
        ITEM_BREATH,
        ITEM_TRANSE_FRAME
    };

    std::map<int, std::tuple<std::shared_ptr < PlayAnim >, EAnimIndex, bool, bool, int> > m_WaitAnimMap;

    int dispWidth = 640;
    int dispHeight = 480;
};