#pragma once

#include <iostream>
#include <vector>
#include <complex>

// fft header

// fft 計算前に一度だけ本関数を使いID並び列情報、ids, n_levelを計算しておきます。
// ここで得られた ids, n_level を、fftの計算で使用します。
// 配列サイズが変わらない限りids, n_levelは使い回し可能。
//【入力】N:入力信号配列の大きさ（２のべき乗）
//【出力】pids:ID並び（呼出時は空を前提）
//      戻り値: n_level
int lc_fft_calc_ids(const int N, std::vector< int >* pids);

// fft
// 入力信号 a 数値列の、高速フーリエ変換を行います。
//【入力】a: 数値列としての離散入力信号、
//         (a.size()は２のべき乗数であることが大前提)
//     ids/n_level: 内部使用する情報(lc_fft_calc_iid()の出力)
//【出力】pout: 数値列としての計算結果。（呼び出し時は空を前提）
//【オプション】is_inverse：直接は使わないで下さい。
//      逆変換には、lc_fft_inverse()を使用します。
void lc_fft(
    const std::vector< std::complex<double> >& a,
    const std::vector< int >& ids, const int n_level,
    std::vector< std::complex< double > >* pout, bool is_inverse = 0);

void lc_inverse_fft(
    const std::vector< std::complex<double> >& a,
    const std::vector< int >& ids, const int n_level,
    std::vector< std::complex< double > >* pout);

// ハミング関数
double Humming(double i, unsigned long size);