/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_CODING_CODECS_OPUS_OPUS_INTERFACE_H_
#define WEBRTC_MODULES_AUDIO_CODING_CODECS_OPUS_OPUS_INTERFACE_H_

#include <stddef.h>

#include "webrtc/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

// Opaque wrapper types for the codec state.
typedef struct WebRtcAacEncInst AacEncInst;
typedef struct WebRtcAacDecInst AacDecInst;

AacEncInst* WebRtcAac_EncoderOpen(unsigned char ucAudioChannel, unsigned int u32AudioSamplerate, unsigned int u32PCMBitSize, int is_mp4);
 void WebRtcAac_EncoderClose(AacEncInst *pHandle);
 int WebRtcAac_EncodeFrame(AacEncInst *pHandle, unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf, unsigned int* outlen);

 
AacDecInst* WebRtcAac_DecoderOpen(unsigned char* adts, unsigned int len, unsigned char* outChannels, unsigned int* outSampleHz);
void WebRtcAac_DecodeClose(AacDecInst *pHandle);
int  WebRtcAac_DecodeFrame(AacDecInst *pHandle, unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf, unsigned int* outlen);




#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // WEBRTC_MODULES_AUDIO_CODING_CODECS_OPUS_OPUS_INTERFACE_H_
