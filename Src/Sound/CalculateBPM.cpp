#include "CalculateBPM.h"

#include "fft.h"
#include <numeric>

using namespace std;

std::vector<double> CalculateBPM::CalcVolume(WAVEHDR wh, WAVEFORMATEX wf)
{
    vector<double> frame(BPM_FRAME_LEN);

    int datasize = wh.dwBytesRecorded;
    int N = datasize / sizeof(short) / 2 / BPM_FRAME_LEN;

    vector<double> currentVolume(N);

    unsigned long i = 0;
    int j = 0;
    int m = 0;
    short* data = (short*)wh.lpData;
    while (i <= datasize / sizeof(short) && m < N)
    {
        frame[j] = data[i];
        //frame[j] = data[i] * han_window(j, FRAME_LEN);
        j++;

        if (j == BPM_FRAME_LEN)
        {
            // 音量(実効値)=sqrt(1/FRAME_LEN * Σframe[n]^2)
            double sum = 0;
            for (int n = 0; n < BPM_FRAME_LEN; n++)
            {
                sum += frame[n] * frame[n];
            }

            // 音量(FFT係数の合計)
            /*rdft(FRAME_LEN, 1, frame, ip, w);
            for (int n = 1; n < FRAME_LEN/2; n++)
            {
            sum += power(frame[2*n], frame[2*n+1]);
            }*/

            currentVolume[m] = sqrt(sum / BPM_FRAME_LEN);
            vol.emplace_back(currentVolume[m]);
            m++;

            j = 0; // 次フレーム
        }

        i += 2;
    }

    if (calcVolumeCallback != nullptr)
        calcVolumeCallback(std::accumulate(currentVolume.begin(), currentVolume.end(), 0.0) / currentVolume.size());

    return vol;
}


void find_peak3(const double* a, size_t size, int *max_idx)
{
    max_idx[0] = -1;
    max_idx[1] = -1;
    max_idx[2] = -1;
    double dy = 0;
    for (size_t i = 1; i < size; ++i)
    {
        double dy_pre = dy;
        dy = a[i] - a[i - 1];
        if (dy_pre > 0 && dy <= 0)
        {
            if (max_idx[0] < 0 || a[i - 1] > a[max_idx[0]])
            {
                max_idx[2] = max_idx[1];
                max_idx[1] = max_idx[0];
                max_idx[0] = i - 1;
            }
            else if (max_idx[1] < 0 || a[i - 1] > a[max_idx[1]])
            {
                max_idx[2] = max_idx[1];
                max_idx[1] = i - 1;
            }
            else if (max_idx[2] < 0 || a[i - 1] > a[max_idx[2]])
            {
                max_idx[2] = i - 1;
            }
        }
    }
}


inline double power(const double re, const double im)
{
    return re*re + im*im;
}


#define _USE_MATH_DEFINES
#include <cmath>
#include "CalculateBPM.h"
#define M_PI       3.14159265358979323846

std::vector<int> CalculateBPM::CalcBPM(std::vector<double> vol, WAVEFORMATEX wf)
{
    const int N = vol.size();
    if (N == 0) return vector<int>();
    // 音量差分(増加のみ)
    double *diff = new double[N]; // 音量差分

    diff[0] = vol[0];
    for (int i = 1; i < N; i++)
    {
        if (vol[i] - vol[i - 1] > 0)
        {
            diff[i] = vol[i] - vol[i - 1];
        }
        else {
            diff[i] = 0;
        }
    }

    const double s = double(wf.nSamplesPerSec*2) / double(BPM_FRAME_LEN); // サンプリング周波数

    // テスト用
    // cos
    /*for (int i = 0; i < N; i++)
    {
        diff[i] = cos(2.0 * M_PI * 2.0 * i / s)*1000;
    }*/
    // 擬似インパルス
    /*for (int i = 0; i < N; i++)
    {
        if (cos(2.0 * M_PI * 2.0 * i / s) > 0.999)
        {
            diff[i] = 1000;
        } else {
            diff[i] = 0;
        }
    }*/

    // テンポ解析
    double *a = new double[240 - 60 + 1];
    double *b = new double[240 - 60 + 1];
    double *r = new double[240 - 60 + 1];
    for (int bpm = 60; bpm <= 240; bpm++)
    {
        double a_sum = 0;
        double b_sum = 0;
        double f = double(bpm) / 60;
        for (int n = 0; n < N; n++)
        {
            double win = Humming(n, N);
            a_sum += diff[n] * cos(2.0 * M_PI * f * n / s) * win;
            b_sum += diff[n] * sin(2.0 * M_PI * f * n / s) * win;
            // 注意：窓関数を使用しないと端の影響で誤差が出る
        }
        double a_tmp = a_sum / N;
        double b_tmp = b_sum / N;
        a[bpm - 60] = a_tmp;
        b[bpm - 60] = b_tmp;
        r[bpm - 60] = sqrt(power(a_tmp, b_tmp));
    }
    delete[] diff;

    // ピーク解析
    int peak_x[3];
    find_peak3(r, 240 - 60 + 1, peak_x);
    vector<int> ret(2);
    ret[0] = peak_x[0] + 60;
    ret[1] = peak_x[1] + 60;
    // cout << ret[0] << ", " << ret[1] << endl;
    delete[] a;
    delete[] b;
    delete[] r;

    return ret;
}

void CalculateBPM::Clear()
{
    vol.clear();
}

void CalculateBPM::SetCalcVolume(const std::function<void(double)>& callback)
{
    calcVolumeCallback = callback;
}
