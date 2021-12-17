#pragma once

struct WSSHEADER {
    char				FileSignature[4];
    unsigned int		CompressionType;
    unsigned short		Format;						//!< �t�H�[�}�b�g
    unsigned short		Channels;					//!< �`�����l����
    unsigned int		SamplesPerSec;				//!< �T���v�����O����.
    unsigned int		BytesPerSecond;				//!< 1�b������̃o�C�g��.
    unsigned short		BlockAlign;					//!< 1�`�����l���̃o�C�g��.
    unsigned int		BitsPerSecond;				//!< 1�f�[�^������̃r�b�g��.
    unsigned long		Size;
};
