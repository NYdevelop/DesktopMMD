#pragma once

#include <vector>
#include <Windows.h>
#include <functional>

class CalculateBPM
{
public:
    std::vector<double> CalcVolume(WAVEHDR wh, WAVEFORMATEX wf);
    std::vector<int> CalcBPM(std::vector<double> vol, WAVEFORMATEX wf);

    void Clear();

    void SetCalcVolume(const std::function<void(double)>& callback);

private:
    const int BPM_FRAME_LEN = 490;
    std::vector<double> vol;
    std::function<void(double)> calcVolumeCallback;
};