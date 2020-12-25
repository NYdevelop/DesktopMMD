#include "WaitState.h"

#include <random>
#include "Util/UtilMMD.h"

static std::mt19937 mt;

void WaitState::Initialize()
{
    std::cout << "state: wait" << std::endl;

    /// �J��������������
    m_mmd->canViewCamera = true;
}

void WaitState::Doing()
{
    if (m_RandomMove == false) return;
    if (walkManager->IsMove() == true) return;
    if (!animQueue->Empty()) return;

    if (mt() % 500 != 0) return;

    if (mt() % 2 == 0)
    {
        auto anim = m_WaitAnimMap[mt() % 3];
        std::cout << "wait anim : " << anim->GetAnimIndex() << std::endl;
        anim->ResetAnimTime();
        animQueue->AddTransrate(-1, anim->GetAnimIndex(), 10);
        animQueue->AddAnim(anim);
        animQueue->AddTransrate(anim->GetAnimIndex(), -1, 10, true);
        return;
    }

    // �����_���ȃX�N���[�����W�ֈړ��J�n
    std::uniform_real_distribution<> randRange(0.967598021, 0.99);
    auto z = (float)randRange(mt);
    WalkStart(
        VGet((float)(mt() % dispWidth), (float)(mt() % dispHeight), z),
        m_mmd.get(),
        walkManager);
}

void WaitState::End()
{
    m_mmd->canViewCamera = false;
}

void WaitState::OnceInitial()
{
    RECT rc;
    GetWindowRect(GetDesktopWindow(), &rc);
    dispWidth = rc.right - rc.left;
    dispHeight = rc.bottom - rc.top;

    std::random_device rnd;     // �񌈒�I�ȗ���������
    mt = move(std::mt19937(rnd()));            // �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h

    SetAnim(EAnimIndex::ANIM_THINK2);
    SetAnim(EAnimIndex::ANIM_LOOK_SELF1);
    SetAnim(EAnimIndex::ANIM_LOOK_SELF2);
}

void WaitState::SetAnim(EAnimIndex index)
{
    int mapIndex = m_WaitAnimMap.size();
    m_WaitAnimMap[mapIndex] = std::shared_ptr<PlayAnim>(new PlayAnim);
    m_WaitAnimMap[mapIndex]->AttachAnime(model, (int)index);
    m_WaitAnimMap[mapIndex]->IsLoop(false);
    MV1SetAttachAnimBlendRate(model, m_WaitAnimMap[mapIndex]->GetAnimIndex(), 0);
}
