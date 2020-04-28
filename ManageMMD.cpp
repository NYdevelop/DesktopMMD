#include "ManageMMD.h"

#include "WaveFile.h"
#include "fft.h"
#include "EstimateBPM.h"
#include "CalculateBPM.h"
#include "CheckNoSound.h"


using namespace std;

#pragma region debug
#pragma pack(push, 1)
struct RiffHeader {
    char          id[4];
    unsigned long size;
    char          fmt[4];
};

struct FormatChunk {
    char           chunkID[4];
    long           chunkSize;
    short          wFormatTag;
    unsigned short wChannels;
    unsigned long  dwSamplesPerSec;
    unsigned long  dwAvgBytesPerSec;
    unsigned short wBlockAlign;
    unsigned short wBitsPerSample;
    /* Note: there may be additional fields here, depending upon wFormatTag. */
};

struct DataChunk {
    char           chunkID[4];
    long           chunkSize;
};
#pragma pack(pop)

void ManageMMD::Debug()
{
    wchar_t *fname_in = L"test1.wav";

    // wavファイルオープン
    HANDLE hFile = CreateFile(fname_in, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("file open error.\n");
        return;
    }
    HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hFileMap == NULL)
    {
        printf("CreateFileMapping error.\n");
        return;
    }
    const void* pBuf = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
    if (pBuf == NULL)
    {
        printf("MapViewOfFile error.\n");
        return;
    }

    // RIFFヘッダー
    struct RiffHeader *header = (struct RiffHeader*)pBuf;
    if (memcmp(header->id, "RIFF", 4) != 0)
    {
        printf("RIFF check error.\n");
        return;
    }
    if (memcmp(header->fmt, "WAVE", 4) != 0)
    {
        printf("WAVE check error.\n");
        return;
    }
    printf("file size : %d\n", header->size);

    // fmtチャンク
    struct FormatChunk *fmt = (struct FormatChunk*)((unsigned char*)pBuf + sizeof(struct RiffHeader));
    if (memcmp(fmt->chunkID, "fmt", 3) != 0)
    {
        printf("fmt check error.\n");
        return;
    }
    printf("FormatTag : %d\n", fmt->wFormatTag);
    printf("Channels : %d\n", fmt->wChannels);
    printf("SamplesPerSec : %d\n", fmt->dwSamplesPerSec);
    printf("BitsPerSample : %d\n", fmt->wBitsPerSample);

    // dataチャンク
    struct DataChunk *dchunk = (struct DataChunk*)((unsigned char*)pBuf + sizeof(struct RiffHeader) + 8 + fmt->chunkSize);
    if (memcmp(dchunk->chunkID, "data", 4) != 0)
    {
        printf("data chunk check error.\n");
        return;
    }
    printf("data size : %d\n", dchunk->chunkSize);

    // サイズチェック
    unsigned long datasize = dchunk->chunkSize;

    BY_HANDLE_FILE_INFORMATION fi;
    GetFileInformationByHandle(hFile, &fi);
    DWORD filesize = fi.nFileSizeLow;
    if (filesize < sizeof(struct RiffHeader) + 8 + fmt->chunkSize + sizeof(struct DataChunk) + datasize)
    {
        printf("data size error.\n");
        return;
    }

    // 16bitのみ対応
    if (fmt->wBitsPerSample != 16)
    {
        printf("16bit only.\n");
        return;
    }
    // stereoのみ対応
    if (fmt->wChannels != 2)
    {
        printf("stereo only.\n");
        return;
    }

    WAVEFORMATEX wf;
    wf.wFormatTag = WAVE_FORMAT_PCM;       // PCM形式
    wf.nChannels = 1;                      // ステレオかモノラルか
    wf.nSamplesPerSec = 22050;             // サンプリングレート 22050KHz
    wf.wBitsPerSample = 16;                // 量子化レベル
    wf.nBlockAlign = wf.wBitsPerSample * wf.nChannels / 8;      // バイトあたりのビット数[PCMの仕様]
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;    // 1 秒あたりのバイト数
                                                                // モノラル*サンプル22050*量子化16bit=1byteなので 一秒間に44100byteのデータが発生する
                                                                // サンプル * nBlockAlign = 一秒間のデータ量なので nBlockAlignは2

    //WAVEHDR wh;
    //wh.lpData = new char[datasize];
    //wh.dwBufferLength = datasize;
    //wh.dwBytesRecorded = datasize;
    //memcpy(wh.lpData, (unsigned char*)pBuf + sizeof(struct RiffHeader) + 8 + fmt->chunkSize + sizeof(struct DataChunk), datasize);

    //{
    //    auto volume = CalcVolume(wh, wf);
    //    auto bpms = CalcBPM(volume, wf);
    //}

}

#pragma endregion

HRESULT ManageMMD::Process()
{
    //Debug();

    m_Window.Init();

    SetUserWindow(m_Window.GetHWnd());
    m_mmd.preInitialize();
    if (DxLib::DxLib_Init() == -1) return E_FAIL;
    m_mmd.afterInitialize();

    m_Window.SetDrawFunc([&](HDC hdc)
    {
        m_mmd.mainProcess();
        //if (m_mmd.mainProcess() != 0)
        //{
        //    SendMessageA(m_Window.GetHWnd(), WM_CLOSE, 0, 0);
        //}
    });


    WAVEFORMATEX wf;
    wf.wFormatTag = WAVE_FORMAT_PCM;       // PCM形式
    wf.nChannels = 1;                      // ステレオかモノラルか
    wf.nSamplesPerSec = 22050;             // サンプリングレート 22050KHz
    wf.wBitsPerSample = 16;                // 量子化レベル
    wf.nBlockAlign = wf.wBitsPerSample * wf.nChannels / 8;      // バイトあたりのビット数[PCMの仕様]
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;    // 1 秒あたりのバイト数
                                                                // モノラル*サンプル22050*量子化16bit=1byteなので 一秒間に44100byteのデータが発生する
                                                                // サンプル * nBlockAlign = 一秒間のデータ量なので nBlockAlignは2

    //SaveOpenWaveFile(L"test.wav", &wf, TRUE);
    //m_Capture.SetCaptureCallback([&](WAVEHDR wh) 
    //{
    //    if (WritedWaveFile(wh.lpData, wh.dwBytesRecorded) == 0) {
    //        CloseWaveFile();
    //    }
    //});

    CalculateBPM calcBPM;
    CheckNoSound checker;
    calcBPM.SetCalcVolume([&checker, &calcBPM](double volume)
    {
        if (checker.CheckSound(volume) == true)
        {
            calcBPM.Clear();
            cout << "clear" << endl;
            return;
        }
    });

    EstimateBPM estimate;
    m_Capture.SetCaptureCallback([this, wf, &estimate, &calcBPM, &checker](WAVEHDR wh)
    {
        auto currentVolume = calcBPM.CalcVolume(wh, wf);
        auto bpms = calcBPM.CalcBPM(currentVolume, wf);
        if (bpms.size() == 0) return;
        auto ret = estimate.Estimate(bpms);
        cout << ret << endl;
        m_mmd.SetBPM(ret);
    });

    // DrawFFT();
    m_Capture.OpenDevice(0, wf, 4410*4);

    m_Capture.Start();

    m_Window.Process(60);
    m_Capture.Stop();

    return S_OK;
}

void ManageMMD::Exit()
{
    DxLib::DxLib_End(); // ＤＸライブラリ使用の終了処理
}

bool ManageMMD::IsRunning()
{
    return !m_Window.IsClose();
}


void ManageMMD::DrawFFT(WAVEFORMATEX wf)
{
    const int NUM_FREQUENCY_BUNDLES = 10;

    const int FFT_LENGTH = 2048;
    vector< int > ids;
    int n_level = lc_fft_calc_ids(FFT_LENGTH, &ids);

    const int devideNum = 11;
    chrono::milliseconds wait(3);
    float prev[22];
    memset(prev, 0, sizeof(float) * 22);
    auto Cr = GetColor(0, 0, 255);        // 青色の値を取得
    auto eachWidth = 1920 / devideNum;
    auto halfHeight = 1080 / 2;
    thread t([&prev, devideNum, wait, Cr, eachWidth, halfHeight]()
    {
        while (true)
        {
            DxLib::ClearDrawScreen();
            for (int i = 0; i < devideNum; i++)
            {
                DrawBox(i*eachWidth, 1080 - prev[i], i*eachWidth + eachWidth - 2, 1030, Cr, TRUE);    // 四角形を描画
            }
            DxLib::ScreenFlip();
            this_thread::sleep_for(wait);
        }
    });

    m_Capture.SetCaptureCallback([&](WAVEHDR wh)
    {
        vector< complex<double> > signal;
        // setup "signal" as your FFT input.
        for (unsigned int i = 0; i < wh.dwBytesRecorded; i += 2)
        {
            complex<double> val(((short)wh.lpData[i] + (wh.lpData[i + 1] << 8)) * Humming(i, FFT_LENGTH), 0);
            signal.emplace_back(val);
        }

        vector< complex<double> > proj;
        lc_fft(signal, ids, n_level, &proj);// FFT変換

                                            //auto result = new float[devideNum];
        for (int k = 0; k < devideNum; k++)
        {
            // 特徴的な周波数成分は低周波数に集まりやすいので、低周波数の音を中心に解像度を上げる
            auto indexFq = ((int)(round(FFT_LENGTH / 2 * pow(0.5, (double)(devideNum - 1 - k) * NUM_FREQUENCY_BUNDLES / devideNum)) - 1) << 1);
            //var indexFq = (int)(FFTLength / 2.0 / (devideNum * devideNum) * k * k);

            //複素数の大きさを計算
            double diagonal = sqrt(proj[indexFq].real() * proj[indexFq].real() + proj[indexFq].imag() * proj[indexFq].imag());

            //result[k] = (float)diagonal;
            prev[k] = max((float)diagonal / 800.f, prev[k] - 8);
        }
    });


    m_Capture.OpenDevice(0, wf, FFT_LENGTH * 2);
}