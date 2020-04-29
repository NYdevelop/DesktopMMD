#ifndef	__WAVEFILE_H__
#define	__WAVEFILE_H__

//BOOL			ReadOpenWaveFile( LPCTSTR file_name );
//int				SaveOpenWaveFile( LPCTSTR file_name, WAVEFORMATEX* wf, BOOL over_write );
//void			CloseWaveFile( void );
//DWORD			ReadWaveFile( void* data, DWORD size );
//DWORD			WritedWaveFile( void* data, DWORD size );
//BOOL			IsOpenWaveFile( void );
//WAVEFORMATEX*	GetAudioFormat( WAVEFORMATEX* wf );


//! WAVE�t�@�C���w�b�_�\����.
typedef struct {
    DWORD		RiffId;						//!< RIFF�t�@�C�����ʎq (RIFF).
    DWORD		FileSize;					//!< �t�@�C���T�C�Y - 8.
    DWORD		FileType;					//!< �t�@�C���^�C�v ("WAVE").
    DWORD		FormatId;					//!< �t�H�[�}�b�g���ʎq ("fmt ").
    DWORD		FormatSize;					//!< �t�H�[�}�b�g�E�T�C�Y - 8.
    WORD		FormatType;					//!< �t�H�[�}�b�g�^�C�v.
    WORD		Channels;					//!< �`�����l����.
    DWORD		SamplesPerSec;				//!< �T���v�����O����.
    DWORD		AvgBytesPerSec;				//!< 1�b������̃o�C�g��.
    WORD		BlockAlign;					//!< 1�`�����l���̃o�C�g��.
    WORD		BitsPerSample;				//!< 1�f�[�^������̃r�b�g��.
} WAVFILEHEADER;

//! �t�@�N�g���`�����N�E�w�b�_�\����.
typedef struct {
    DWORD		Id;							//!< �f�[�^���ʎq("fact").
    DWORD		Size;						//!< �`�����N�T�C�Y - 8.
    DWORD		Samples;					//!< �S�T���v����.
} WAVEFACTCHUNK;

//! �f�[�^�`�����N�E�w�b�_�\����.
typedef struct {
    DWORD		Id;							//!< �f�[�^���ʎq("data").
    DWORD		Size;						//!< �f�[�^�E�T�C�Y.
} WAVEDATACHUNK;

#endif	// __WAVEFILE_H__
