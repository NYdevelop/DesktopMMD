#pragma once

#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>

/**
 * �X���b�h�𗧂āA�e�L�X�g�t�@�C���̓��e���X�V����Ă��Ȃ����m�F���܂��B
 * �Q�� : �uc++�ň��Ԋu�Ŋ֐������s���������v (teratail) https://teratail.com/questions/27449
 * �Q�� : �ustd::thread::thread�v (C++����̓���T�C�g�ł�) http://kaworu.jpn.org/cpp/std::thread::thread
 * �Q�� : �ustd::this thread::sleep for�v (C++����̓���T�C�g�ł�) http://kaworu.jpn.org/cpp/std::this_thread::sleep_for
 * �Q�� : �uC++1z �����_���ł�*this�̃R�s�[�L���v�`���v (Faith and Brave - C++�ŗV�ڂ�) http://faithandbrave.hateblo.jp/entry/2016/04/27/191209
 * �Q�� : �uC++ �Ōp�������Ƃ��ɃT�u�N���X�̃����o�֐����ĂԂ��߂ɂ� virtual �����ĉ��z�֐��ɂ���v (sotarok�̂��׋�) http://d.hatena.ne.jp/strkpy/20100401/1270133069
 */
class UtilTimer
{
public:
    /**
     * �X���b�h���J�n���܂��B
     */
    void Start(std::function<bool(void)> func, unsigned long microsecond);
    /**
     * �X�^�[�g�������X���b�h�������Ă���Ԃ́A�������ɐi�܂��܂���B
     */
    void Block();
    /**
     * �����̓r���ł��A�����I�ɃX���b�h���I�����܂��B
     */
    void Break_Force();

    bool IsRunning();

    /**
     * �i���̃N���X�̎��X���b�h�ł͂Ȃ��j���̊֐����Ăяo�����X���b�h���A�w��~���b��~�����܂��B
     * �l�[���X�y�[�X��؂�̂��߂�ǂ������̂Ł@���̃N���X�ɒu���Ă���B
     */
    static void Sleep_Milliseconds(long milliseconds);

protected:
    std::thread m_thread1;
    /**
     * �X���b�h���~���Ă��A�����̃��[�v�͓��������Ă���悤�������̂ŁA
     * ���̃t���O���U�ɂ��邱�ƂŁA�X���b�h�̒��̃��[�v���甲�������܂�
     */
    bool m_alive = false;
    /**
     * �ʃX���b�h�Ŏ��s���������������������\�b�h�������Ă��܂��B
     */
    std::function<bool(void)> m_Func;
};