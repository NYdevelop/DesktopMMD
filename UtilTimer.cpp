#include "UtilTimer.h"

#include <iostream>

void UtilTimer::Start(std::function<bool(void)> func, unsigned long microsecond)
{
    m_alive = true;
    m_Func = func;
    m_thread1 = std::thread([=] {
        // �����Ə����̊Ԃ� n �b�ȏ�󂯂�B�i���� n �b�Ԋu�ۏ�Ȃ̂ŁAn �b���ɒ���I�ɃX�^�[�g�����Ƃ����Ӗ��ł͂Ȃ��j
        const std::chrono::microseconds interval(microsecond);
        while (this->m_alive) {

            auto start = std::chrono::system_clock::now();

            // �����������ꂽ���\�b�h���Ăяo��
            if (m_Func() == false)
            {
                break;
            }

            auto end = std::chrono::system_clock::now();
            auto elapsed = end - start;
            // std::cout << "elapsed.count()=[" << elapsed.count() << "]" << std::endl;

            // ����A�C���^�[�o�����Ԃ̃X���[�v��ۏ�
            if (elapsed < interval)
            {
                // std::cout << (interval - elapsed).count()/ 10.0 / 1000 << std::endl;
                std::this_thread::sleep_for(interval - elapsed);
            }
        }
        this->m_alive = false;
        std::cout << "finished." << std::endl;
    });
}

void UtilTimer::Block()
{
    // �X�^�[�g�������X���b�h���I������܂ŁA����ȏ�i�܂Ȃ��B
    m_thread1.join();
    // �X���b�h���~�߂邩�A����Ɏ~�܂邩����Ɓ@��������ȉ��ɐi�ށB
}

void UtilTimer::Break_Force()
{
    if (m_alive == false)
    {
        return;
    }

    // �X���b�h���~�߂�
    m_thread1.detach();

    // �X���b�h�̒��̃��[�v���甲��������
    this->m_alive = false;
}

bool UtilTimer::IsRunning()
{
    return m_alive;
}

void UtilTimer::Sleep_Milliseconds(long milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}