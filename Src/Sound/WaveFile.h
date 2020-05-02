#ifndef	__WAVEFILE_H__
#define	__WAVEFILE_H__

//! RIFF �̃w�b�_�[
struct RIFFHEADER
{
    DWORD	ID;
    DWORD	Size;
    DWORD	FileType;
};

//! RIFF �̃`�����N
struct RIFFCHUNCK
{
    DWORD	ID;
    DWORD	Size;
};

//! �t�H�[�}�b�g�`�����N
struct RIFFFORMATCHUNK {
    WORD		FormatType;					//!< �t�H�[�}�b�g�^�C�v.
    WORD		Channels;					//!< �`�����l����.
    DWORD		SamplesPerSec;				//!< �T���v�����O����.
    DWORD		AvgBytesPerSec;				//!< 1�b������̃o�C�g��.
    WORD		BlockAlign;					//!< 1�`�����l���̃o�C�g��.
    WORD		BitsPerSample;				//!< 1�f�[�^������̃r�b�g��.
} ;

//! �t�@�N�g���`�����N�E�w�b�_
struct WAVEFACTCHUNK
{
    DWORD		Id;							//!< �f�[�^���ʎq("fact").
    DWORD		Size;						//!< �`�����N�T�C�Y - 8.
    DWORD		Samples;					//!< �S�T���v����.
};

#endif	// __WAVEFILE_H__
