#pragma once

#include <iostream>
#include <vector>
#include <complex>

// fft header

// fft �v�Z�O�Ɉ�x�����{�֐����g��ID���ї���Aids, n_level���v�Z���Ă����܂��B
// �����œ���ꂽ ids, n_level ���Afft�̌v�Z�Ŏg�p���܂��B
// �z��T�C�Y���ς��Ȃ�����ids, n_level�͎g���񂵉\�B
//�y���́zN:���͐M���z��̑傫���i�Q�ׂ̂���j
//�y�o�́zpids:ID���сi�ďo���͋��O��j
//      �߂�l: n_level
int lc_fft_calc_ids(const int N, std::vector< int >* pids);

// fft
// ���͐M�� a ���l��́A�����t�[���G�ϊ����s���܂��B
//�y���́za: ���l��Ƃ��Ă̗��U���͐M���A
//         (a.size()�͂Q�ׂ̂��搔�ł��邱�Ƃ���O��)
//     ids/n_level: �����g�p������(lc_fft_calc_iid()�̏o��)
//�y�o�́zpout: ���l��Ƃ��Ă̌v�Z���ʁB�i�Ăяo�����͋��O��j
//�y�I�v�V�����zis_inverse�F���ڂ͎g��Ȃ��ŉ������B
//      �t�ϊ��ɂ́Alc_fft_inverse()���g�p���܂��B
void lc_fft(
    const std::vector< std::complex<double> >& a,
    const std::vector< int >& ids, const int n_level,
    std::vector< std::complex< double > >* pout, bool is_inverse = 0);

void lc_inverse_fft(
    const std::vector< std::complex<double> >& a,
    const std::vector< int >& ids, const int n_level,
    std::vector< std::complex< double > >* pout);

// �n�~���O�֐�
double Humming(double i, unsigned long size);