#ifndef	__WAVEFILE_H__
#define	__WAVEFILE_H__

//BOOL			ReadOpenWaveFile( LPCTSTR file_name );
//int				SaveOpenWaveFile( LPCTSTR file_name, WAVEFORMATEX* wf, BOOL over_write );
//void			CloseWaveFile( void );
//DWORD			ReadWaveFile( void* data, DWORD size );
//DWORD			WritedWaveFile( void* data, DWORD size );
//BOOL			IsOpenWaveFile( void );
//WAVEFORMATEX*	GetAudioFormat( WAVEFORMATEX* wf );


//! WAVEファイルヘッダ構造体.
typedef struct {
    DWORD		RiffId;						//!< RIFFファイル識別子 (RIFF).
    DWORD		FileSize;					//!< ファイルサイズ - 8.
    DWORD		FileType;					//!< ファイルタイプ ("WAVE").
    DWORD		FormatId;					//!< フォーマット識別子 ("fmt ").
    DWORD		FormatSize;					//!< フォーマット・サイズ - 8.
    WORD		FormatType;					//!< フォーマットタイプ.
    WORD		Channels;					//!< チャンネル数.
    DWORD		SamplesPerSec;				//!< サンプリング周期.
    DWORD		AvgBytesPerSec;				//!< 1秒あたりのバイト数.
    WORD		BlockAlign;					//!< 1チャンネルのバイト数.
    WORD		BitsPerSample;				//!< 1データあたりのビット数.
} WAVFILEHEADER;

//! ファクトリチャンク・ヘッダ構造体.
typedef struct {
    DWORD		Id;							//!< データ識別子("fact").
    DWORD		Size;						//!< チャンクサイズ - 8.
    DWORD		Samples;					//!< 全サンプル数.
} WAVEFACTCHUNK;

//! データチャンク・ヘッダ構造体.
typedef struct {
    DWORD		Id;							//!< データ識別子("data").
    DWORD		Size;						//!< データ・サイズ.
} WAVEDATACHUNK;

#endif	// __WAVEFILE_H__
