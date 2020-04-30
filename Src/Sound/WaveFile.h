#ifndef	__WAVEFILE_H__
#define	__WAVEFILE_H__

//BOOL			ReadOpenWaveFile( LPCTSTR file_name );
//int				SaveOpenWaveFile( LPCTSTR file_name, WAVEFORMATEX* wf, BOOL over_write );
//void			CloseWaveFile( void );
//DWORD			ReadWaveFile( void* data, DWORD size );
//DWORD			WritedWaveFile( void* data, DWORD size );
//BOOL			IsOpenWaveFile( void );
//WAVEFORMATEX*	GetAudioFormat( WAVEFORMATEX* wf );

//! RIFF のヘッダー
struct RIFFHEADER
{
    DWORD	ID;
    DWORD	Size;
    DWORD	FileType;
};

//! RIFF のチャンク
struct RIFFCHUNCK
{
    DWORD	ID;
    DWORD	Size;
};

//! フォーマットチャンク
struct RIFFFORMATCHUNK {
    WORD		FormatType;					//!< フォーマットタイプ.
    WORD		Channels;					//!< チャンネル数.
    DWORD		SamplesPerSec;				//!< サンプリング周期.
    DWORD		AvgBytesPerSec;				//!< 1秒あたりのバイト数.
    WORD		BlockAlign;					//!< 1チャンネルのバイト数.
    WORD		BitsPerSample;				//!< 1データあたりのビット数.
} ;

//! ファクトリチャンク・ヘッダ
struct WAVEFACTCHUNK
{
    DWORD		Id;							//!< データ識別子("fact").
    DWORD		Size;						//!< チャンクサイズ - 8.
    DWORD		Samples;					//!< 全サンプル数.
};

#endif	// __WAVEFILE_H__
