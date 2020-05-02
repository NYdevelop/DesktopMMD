#include "WaitState.h"

#include <random>
#include "Util/UtilMMD.h"

static std::mt19937 mt;
void WaitState::Doing()
{
    if (m_RandomMove == false) return;
    if (walkManager->IsMove() == true) return;

    if (mt() % 500 != 0) return;

    // �����_���ȃX�N���[�����W�ֈړ��J�n
    WalkStart((float)(mt() % dispWidth), (float)(mt() % dispHeight), m_mmd.get(), walkManager);
}

void WaitState::OnceInitial()
{
    RECT rc;
    GetWindowRect(GetDesktopWindow(), &rc);
    dispWidth = rc.right - rc.left;
    dispHeight = rc.bottom - rc.top;

    std::random_device rnd;     // �񌈒�I�ȗ���������
    mt = move(std::mt19937(rnd()));            // �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h
}
