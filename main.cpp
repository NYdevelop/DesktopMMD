#include <thread>
#include <iostream>

//#include "window.h"
//#include "DrawMMD.h"
#include "ManageMMD.h"

using namespace std;

// �v���O�����̎��s: Ctrl + F5 �܂��� [�f�o�b�O] > [�f�o�b�O�Ȃ��ŊJ�n] ���j���[
// �v���O�����̃f�o�b�O: F5 �܂��� [�f�o�b�O] > [�f�o�b�O�̊J�n] ���j���[
int main()
{
    //thread t([]()
    //{

    //});
    ManageMMD manager;
    manager.Process();
    manager.Exit();

    _CrtDumpMemoryLeaks();
    //t.join();

    return 0;
}


//int main()
//{
//    DrawMMD mmd;
//
//    mmd.preInitialize();
//    if (DxLib::DxLib_Init() == -1)return -1;
//    mmd.afterInitialize();
//
//    // ���C���X���b�h�ł̃��[�v�łȂ��Ɠ��͎擾�ł��Ȃ�
//    const std::chrono::microseconds INTERVAL(1000 * 1000 / 30);
//    while (DxLib::ProcessMessage() == 0)
//    {
//        auto start = std::chrono::system_clock::now();
//
//        if (mmd.mainProcess() != 0)
//        {
//            break;
//        }
//
//        auto end = std::chrono::system_clock::now();
//        auto elapsed = end - start;
//
//        // ����A�C���^�[�o�����Ԃ̃X���[�v��ۏ�
//        if (elapsed < INTERVAL) {
//            std::this_thread::sleep_for(INTERVAL - elapsed);
//        }
//    }
//
//    DxLib::DxLib_End(); // �c�w���C�u�����g�p�̏I������
//    return 0; // �\�t�g�̏I�� 
//}