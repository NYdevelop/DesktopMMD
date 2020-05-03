#pragma once

class AnimInterface
{
public:
    virtual int AttachAnime(int modHandle, int animIndex)
    {
        m_AnimIndex = animIndex;
        return 0;
    }
    virtual bool PlayAnimation() = 0;

    int GetAnimIndex() { return m_AnimIndex; }

protected:
    int m_AnimIndex = 0;
};