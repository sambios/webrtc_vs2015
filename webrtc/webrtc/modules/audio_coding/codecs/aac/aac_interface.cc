

#include "webrtc/modules/audio_coding/codecs/aac/aac_interface.h"
#include "webrtc/base/logging.h"
#include "aacdecoder_lib.h"
#include "aacenc_lib.h"

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  /* Maximum supported frame size in WebRTC is 60 ms. */
  kWebRtcAACMaxEncodeFrameSizeMs = 60,

  /* The format allows up to 120 ms frames. Since we don't control the other
   * side, we must allow for packets of that size. NetEq is currently limited
   * to 60 ms on the receive side. */
   //TODO: media-av
  kWebRtcAACMaxDecodeFrameSizeMs = 60,

  /* Maximum sample count per channel is 48 kHz * maximum frame size in
   * milliseconds. */
  kWebRtcAACMaxFrameSizePerChannel = 48 * kWebRtcAACMaxDecodeFrameSizeMs,

  /* Maximum sample count per frame is 48 kHz * maximum frame size in
   * milliseconds * maximum number of channels. */
  kWebRtcAACMaxFrameSize = kWebRtcAACMaxFrameSizePerChannel * 2,

  /* Maximum sample count per channel for output resampled to 32 kHz,
   * 32 kHz * maximum frame size in milliseconds. */
  kWebRtcAACMaxFrameSizePerChannel32kHz = 32 * kWebRtcAACMaxDecodeFrameSizeMs,

  /* Number of samples in resampler state. */
  kWebRtcAACStateSize = 7,

  /* Default frame size, 20 ms @ 48 kHz, in samples (for one channel). */
  kWebRtcAACDefaultFrameSize = 960,
};

static AUDIO_OBJECT_TYPE GetInternalAOT(AAC_AOT aot) {
	AUDIO_OBJECT_TYPE ret;
	switch (aot) {
	case AAC_AOT_LC:
		ret = AOT_AAC_LC;
		break;
	case AAC_AOT_HE:
		ret = AOT_SBR;
		break;
	case AAC_AOT_HE_V2:
		ret = AOT_PS;
		break;
	case AAC_AOT_LD:
		ret = AOT_ER_AAC_LD;
		break;
	case AAC_AOT_ELD:
		ret = AOT_ER_AAC_ELD;
		break;
	default:
		ret = AOT_AAC_LC;
		break;
	}
	return ret;
}

static TRANSPORT_TYPE GetInternalTT(AAC_TRANSPORT_TYPE aot) {
	TRANSPORT_TYPE ret;
	switch (aot) {
	case AAC_TT_RAW:
		ret = TT_MP4_RAW;
		break;
	case AAC_TT_ADIF:
		ret = TT_MP4_ADIF;
		break;
	case AAC_TT_ADTS:
		ret = TT_MP4_ADTS;
		break;
	case AAC_TT_LATM_MCP1:
		ret = TT_MP4_LATM_MCP1;
		break;
	case AAC_TT_LATM_MCP0:
		ret = TT_MP4_LATM_MCP0;
		break;
	case AAC_TT_LOAS:
		ret = TT_MP4_LOAS;
		break;
	default:
		ret = TT_MP4_RAW;
		break;
	}
	return ret;
}

struct WebRtcAACEncInst {
	HANDLE_AACENCODER encoder;
	int32_t frame_samples;
	int32_t channels;
};

int16_t WebRtcAAC_EncoderCreate(AACEncInst** inst, AAC_AOT aot, int32_t samplerate, int32_t channels) {
	AACEncInst* state = NULL;
    int ret = 0;
    if (inst != NULL) {
        state = (AACEncInst*)calloc(1, sizeof(AACEncInst));
        if (state) {
            state->encoder = NULL;
            if ( (ret = aacEncOpen(&state->encoder, 0, 2)) != AACENC_OK) {  // channels
                goto INIT_FAIL;
            }

            int getaot = GetInternalAOT(aot);
            printf("====================================%s-aot:%d, getaot:%d\n", __FUNCTION__, aot, getaot );
            if (aacEncoder_SetParam(state->encoder, AACENC_AOT, getaot) != AACENC_OK) { // 2-is-ok
                goto INIT_FAIL;
            }
            if (aot == AAC_AOT_ELD) {
                //ELD: use default SBR.
                if ((ret = aacEncoder_SetParam(state->encoder, AACENC_SBR_MODE, -1)) != AACENC_OK) {
                    goto INIT_FAIL;
                }
                //ELD: 480 or 512.
                if ((ret = aacEncoder_SetParam(state->encoder, AACENC_GRANULE_LENGTH, 480)) != AACENC_OK) {
                    goto INIT_FAIL;
                }
            }

            printf("====================================%s-samplerate:%d\n", __FUNCTION__, samplerate );
            if ((ret = aacEncoder_SetParam(state->encoder, AACENC_SAMPLERATE, samplerate)) != AACENC_OK) { // 32000-is-ok
                goto INIT_FAIL;
            }

            printf("====================================%s-channels:%d\n", __FUNCTION__, channels );
            if ((ret = aacEncoder_SetParam(state->encoder, AACENC_CHANNELMODE, channels)) != AACENC_OK) { // 2-is-ok
                goto INIT_FAIL;
            }

            if ((ret = aacEncoder_SetParam(state->encoder, AACENC_CHANNELORDER, 1)) != AACENC_OK) {
                goto INIT_FAIL;
            }

            int32_t vbr = 0; // 8;
            int32_t bitrate_def = 32000;
            if (aot == AAC_AOT_ELD)
            {
                vbr = 8;
                bitrate_def = 24000;
            }
            printf("====================================%s-vbr:%d\n", __FUNCTION__, vbr );

            if (vbr) {
                if ((ret = aacEncoder_SetParam(state->encoder, AACENC_BITRATEMODE, vbr)) != AACENC_OK) {
                    goto INIT_FAIL;
                }
            }

            printf("====================================%s-bitrate_def:%d\n", __FUNCTION__, bitrate_def );
            if ((ret = aacEncoder_SetParam(state->encoder, AACENC_BITRATE, bitrate_def)) != AACENC_OK) { // 32000
                goto INIT_FAIL;
            }

            // rtp or udp or rudp must use adts, TT_MP4_LOAS=10,  TT_MP4_ADTS=2, TT_MP4_RAW=0
            int tt_type = TT_MP4_ADTS;
            if (aot == AAC_AOT_ELD)
            {
                tt_type = TT_MP4_LOAS;
            }
            //LOAS // 0-is-ok
            printf("====================================%s-tt_type:%d\n", __FUNCTION__, tt_type );
            if ((ret = aacEncoder_SetParam(state->encoder, AACENC_TRANSMUX, tt_type)) != AACENC_OK) {
                goto INIT_FAIL;
            }

            if (aot == AAC_AOT_ELD)
            {
#if defined(WEBRTC_ANDROID) || defined(WEBRTC_IOS) || defined(WEBRTC_ARCH_ARM)
                int32_t afterburner = 0;
#else
                int32_t afterburner = 1;
#endif
                if ((ret = aacEncoder_SetParam(state->encoder, AACENC_AFTERBURNER, afterburner)) != AACENC_OK) {
                    goto INIT_FAIL;
                }
            }

            if ((ret = aacEncoder_SetParam(state->encoder, AACENC_AFTERBURNER, 1)) != AACENC_OK) {
                goto INIT_FAIL;
            }

            if ((ret = aacEncEncode(state->encoder, NULL, NULL, NULL, NULL)) != AACENC_OK) {
                goto INIT_FAIL;
            }
            AACENC_InfoStruct info = { 0 };
            if ((ret = aacEncInfo(state->encoder, &info)) != AACENC_OK) {
                goto INIT_FAIL;
            }
            
            printf("====================================%s-maxOutBufBytes:%d, maxAncBytes:%d, inBufFillLevel:%d, inputChannels:%d, frameLength:%d, encoderDelay:%d, confBuf:%02x-%02x-%02x-%02x, confSize:%d\n",
                __FUNCTION__,
                info.maxOutBufBytes, 
                info.maxAncBytes, 
                info.inBufFillLevel, 
                info.inputChannels, 
                info.frameLength, 
                info.encoderDelay,
                info.confBuf[0],
                info.confBuf[1],
                info.confBuf[2],
                info.confBuf[3],
                info.confSize                );
            state->frame_samples = info.frameLength;
            state->channels = channels;
            *inst = state;
            return 0;
        }
    }

INIT_FAIL:
    if (state) {
        if (state->encoder) {
            aacEncClose(&state->encoder);
        }
        free(state);
    }
	return -1;
}

int16_t WebRtcAAC_EncoderFree(AACEncInst* inst) {
	if (inst) {
		aacEncClose(&inst->encoder);
		free(inst);
		return 0;
	} else {
		return -1;
	}
}

int16_t WebRtcAAC_Encode(AACEncInst* inst, int16_t* audio_in, int16_t samples,
	int16_t length_encoded_buffer, uint8_t* encoded) {
	AACENC_BufDesc in_buf = { 0 }, out_buf = { 0 };
	AACENC_InArgs in_args = { 0 };
	AACENC_OutArgs out_args = { 0 };
	int in_identifier = IN_AUDIO_DATA;
	int in_size = 0;
    int in_elem_size = 0;
	int out_identifier = OUT_BITSTREAM_DATA;
	int out_size = 20480;
    int out_elem_size = 0;
	void *in_ptr, *out_ptr;
	AACENC_ERROR err;

	if (audio_in == NULL) {
		in_args.numInSamples = -1;
	} else {
		in_ptr = audio_in;
		in_size = samples * 2 * inst->channels;
		in_elem_size = 2;

		in_args.numInSamples = samples * inst->channels;
		in_buf.numBufs = 1;
		in_buf.bufs = &in_ptr;
		in_buf.bufferIdentifiers = &in_identifier;
		in_buf.bufSizes = &in_size;
		in_buf.bufElSizes = &in_elem_size;
	}
	out_ptr = encoded;
	out_size = length_encoded_buffer;
	out_elem_size = 1;
	out_buf.numBufs = 1;
	out_buf.bufs = &out_ptr;
	out_buf.bufferIdentifiers = &out_identifier;
	out_buf.bufSizes = &out_size;
	out_buf.bufElSizes = &out_elem_size;

    if ((err = aacEncEncode(inst->encoder, &in_buf, &out_buf, &in_args, &out_args)) != AACENC_OK)
	{
        if (err == AACENC_ENCODE_EOF)
        {
            return 0;
        }
        return -1;
    }
	return out_args.numOutBytes;
}

int16_t WebRtcAAC_SetBitRate(AACEncInst* inst, int32_t rate) {
	if (inst) {
		if (aacEncoder_SetParam(inst->encoder, AACENC_BITRATE, rate) == AACENC_OK) {
			return 0;
		}
	}
	return -1;
}

int16_t WebRtcAAC_SetPacketLossRate(AACEncInst* inst, int32_t loss_rate) {
	return 0;
}

int16_t WebRtcAAC_EnableFec(AACEncInst* inst) {
	return 0;
}

int16_t WebRtcAAC_DisableFec(AACEncInst* inst) {
	return 0;
}

int16_t WebRtcAAC_SetComplexity(AACEncInst* inst, int32_t complexity) {
	return 0;
}

struct WebRtcAACDecInst {
	HANDLE_AACDECODER decoder;
	CStreamInfo* info;
	int prev_decoded_samples;
};

//TODO:transport_type
int16_t WebRtcAAC_DecoderCreate(AACDecInst** inst, AAC_TRANSPORT_TYPE tt) {
	AACDecInst* state = NULL;
	AAC_DECODER_ERROR err = AAC_DEC_OK;

	if (inst) {
		/* Create AAC decoder state. */
		state = (AACDecInst*)calloc(1, sizeof(AACDecInst));
		if (state == NULL) {
			goto INIT_FAIL;
		}
		state->decoder = NULL;
		state->info = NULL;
		state->prev_decoded_samples = 0;

		state->decoder = aacDecoder_Open(GetInternalTT(tt), 1);
		if (!state->decoder) {
			goto INIT_FAIL;
		}

		if ((err = aacDecoder_SetParam(state->decoder, AAC_CONCEAL_METHOD, 2)) != AAC_DEC_OK) {
			goto INIT_FAIL;
		}

		*inst = state;
		return 0;
	}

INIT_FAIL:
	if (state) {
		if (state->decoder) {
			aacDecoder_Close(state->decoder);
		}
		free(state);
	}
	return -1;
}

int16_t WebRtcAAC_DecoderFree(AACDecInst* inst) {
	if (inst) {
		aacDecoder_Close(inst->decoder);
		free(inst);
		return 0;
	} else {
		return -1;
	}
}

int WebRtcAAC_DecoderChannels(AACDecInst* inst) {
	if (inst->info) {
		return inst->info->numChannels;
	}
	return -1;
}

int16_t WebRtcAAC_DecoderInit(AACDecInst* inst) {
	return 0;
}

int16_t WebRtcAAC_Decode(AACDecInst* inst, const uint8_t* encoded,
	int16_t encoded_bytes, int16_t* decoded,
	int16_t* audio_type) {

	if (inst == NULL) {
		return  -1;
	}

	AAC_DECODER_ERROR err;
	UCHAR* coded = (UCHAR*)encoded;
	UINT codedBytes = (UINT)encoded_bytes;
	UINT valid = (UINT)encoded_bytes;
	// suker-2015, for aac lc, 1024--2048--4096, (aac eld:960-1920-3840)
	// 2880 * 2 * 2;  // 60 ms @ 48 kHz //TODO: maybe 2880*2. 
	const int kMaxFrameSize = 2048*2*8;
	int16_t *buf = decoded;
	int buf_size = kMaxFrameSize;
	int16_t decodedSamples = 0;

	do {
		if (codedBytes > 0) {
			err = aacDecoder_Fill(inst->decoder, &coded, &codedBytes, &valid);
			if (err != AAC_DEC_OK) {
				return -1;
			}
			coded += (codedBytes - valid);
			codedBytes = valid;
		}

		err = aacDecoder_DecodeFrame(inst->decoder, (INT_PCM *)buf, buf_size, 0);
		if (err == AAC_DEC_NOT_ENOUGH_BITS) {
			break;
		}
		if (err != AAC_DEC_OK) {
			return -1;
		}

		if (inst->info ==  NULL) {
			inst->info = aacDecoder_GetStreamInfo(inst->decoder);
			if (inst->info == NULL) {
				return -1;
			}
		}

		int frameShorts = inst->info->frameSize * inst->info->numChannels;
		buf += frameShorts;
		buf_size -= frameShorts * 2;

		if (buf_size < frameShorts * 2) {
			break;
		}

		decodedSamples += inst->info->frameSize;
	} while (buf_size > 0);

	if (audio_type) {
		*audio_type = 1;
	}
	inst->prev_decoded_samples = decodedSamples;
	return decodedSamples;
}

int16_t WebRtcAAC_DecodePlc(AACDecInst* inst, int16_t* decoded, const int32_t max_decoded_shorts,
	int16_t number_of_lost_frames) {

	AAC_DECODER_ERROR err;
	int decodedSamples = 0;
	int plc_samples;
	int16_t *buf;
	int buf_samples, buf_bytes;

	if (inst == NULL || inst->info == NULL) {
		return -1;
	}

	buf = decoded;
    buf_samples = max_decoded_shorts / ((inst->info->numChannels == 0) ? 1 : inst->info->numChannels);
    buf_bytes = max_decoded_shorts * 2;



	/* Decode to a temporary buffer. The number of samples we ask for is
	 * |number_of_lost_frames| times |prev_decoded_samples_|. Limit the number
	 * of samples to maximum |kWebRtcAACMaxFrameSizePerChannel|. */
	plc_samples = number_of_lost_frames * inst->prev_decoded_samples;
    plc_samples = (plc_samples <= buf_samples) ? plc_samples : buf_samples;

	while (decodedSamples < plc_samples) {
        err = aacDecoder_DecodeFrame(inst->decoder, (INT_PCM *)buf, buf_bytes, AACDEC_CONCEAL);
		if (err != AAC_DEC_OK) {
            if (decodedSamples > 0) {
                return decodedSamples;
            }
			return -1;
		}

		decodedSamples += inst->info->frameSize;

		int frameShorts = inst->info->frameSize * inst->info->numChannels;
		buf += frameShorts;
        buf_bytes -= frameShorts * 2;
        if (buf_bytes <= 0) {
            break;
        }
	}

	return decodedSamples;
}

int16_t WebRtcAAC_DecodeFec(AACDecInst* inst, const uint8_t* encoded,
	int16_t encoded_bytes, int16_t* decoded,
	int16_t* audio_type) {
	return -1;
}

int WebRtcAAC_DurationEst(AACDecInst* inst,
	const uint8_t* payload,
	int payload_length_bytes) {
	return -1;
}

int WebRtcAAC_FecDurationEst(const uint8_t* payload,
	int payload_length_bytes) {
	return -1;
}

int WebRtcAAC_PacketHasFec(const uint8_t* payload,
	int payload_length_bytes) {
	return 0;
}

#ifdef __cplusplus
}  // extern "C"
#endif
