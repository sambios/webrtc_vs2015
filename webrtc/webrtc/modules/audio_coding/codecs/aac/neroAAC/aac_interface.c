/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/audio_coding/codecs/aac/aac_interface.h"

#include "faad.h"
//#include "faac.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

AacEncInst* WebRtcAac_EncoderOpen(unsigned char ucAudioChannel, unsigned int u32AudioSamplerate,
            unsigned int u32PCMBitSize, int is_mp4)
{
    return 0;
}

void WebRtcAac_EncoderClose(AacEncInst *pHandle)
{
    return;
}

int WebRtcAac_EncodeFrame(AacEncInst *pHandle, unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf, unsigned int* outlen)
{
    return 0;
}


AacDecInst* WebRtcAac_DecoderOpen(unsigned char* adts, unsigned int len, unsigned char* outChannels, unsigned int* outSampleHz)
{
    unsigned long samplerate;
    unsigned char channels;
    NeAACDecHandle decoder = NULL;

    //open decoder  
    decoder = NeAACDecOpen();
    //initialize decoder  
    if (NeAACDecInit(decoder, adts, len, &samplerate, &channels) < 0)
    {
        NeAACDecClose(decoder);
        decoder = NULL;
    }
    *outChannels = channels;
    *outSampleHz = samplerate;
    return decoder;

}

void WebRtcAac_DecodeClose(AacDecInst *pHandle)
{
    if (pHandle != NULL) {
        NeAACDecClose(pHandle);
        pHandle = NULL;
    }
}

int  WebRtcAac_DecodeFrame(AacDecInst *pHandle, unsigned char* inbuf, unsigned int inlen, unsigned char* outbuf, unsigned int* outlen)
{
    NeAACDecFrameInfo frame_info;
    unsigned char* pcm_data = NULL;
    if (pHandle != NULL) {
        //decode ADTS frame  
        pcm_data = (unsigned char*)NeAACDecDecode(pHandle, &frame_info, inbuf, inlen);

        if (frame_info.error > 0)
        {
            printf("%s\n", NeAACDecGetErrorMessage(frame_info.error));
            return 0;
        }
        else if (pcm_data && frame_info.samples > 0)
        {
            *outlen = frame_info.samples * frame_info.channels;
            memcpy(outbuf, pcm_data, frame_info.samples * frame_info.channels);
        }
    }
    return *outlen;
}

