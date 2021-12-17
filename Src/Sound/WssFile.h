#pragma once

struct WSSHEADER {
    char				FileSignature[4];
    unsigned int		CompressionType;
    unsigned short		Format;						//!< フォーマット
    unsigned short		Channels;					//!< チャンネル数
    unsigned int		SamplesPerSec;				//!< サンプリング周期.
    unsigned int		BytesPerSecond;				//!< 1秒あたりのバイト数.
    unsigned short		BlockAlign;					//!< 1チャンネルのバイト数.
    unsigned int		BitsPerSecond;				//!< 1データあたりのビット数.
    unsigned long		Size;
};
