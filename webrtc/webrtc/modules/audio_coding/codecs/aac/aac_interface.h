

#ifndef WEBRTC_MODULES_AUDIO_CODING_CODECS_AAC_AAC_INTERFACE_H_
#define WEBRTC_MODULES_AUDIO_CODING_CODECS_AAC_AAC_INTERFACE_H_

#include "webrtc/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

// Opaque wrapper types for the codec state.
typedef struct WebRtcAACEncInst AACEncInst;
typedef struct WebRtcAACDecInst AACDecInst;

/**
* Audio Object Type definitions.
*/
typedef enum
{
	AAC_AOT_NONE = -1,
	AAC_AOT_LC = 0,
	AAC_AOT_HE = 1,
	AAC_AOT_HE_V2 = 2,
	AAC_AOT_LD = 3,
	AAC_AOT_ELD = 4,
} AAC_AOT;

typedef enum
{
	AAC_TT_UNKNOWN = -1, /**< Unknown format.            */
	AAC_TT_RAW = 0,  /**< "as is" access units (packet based since there is obviously no sync layer) */
	AAC_TT_ADIF = 1,  /**< ADIF bitstream format.     */
	AAC_TT_ADTS = 2,  /**< ADTS bitstream format.     */

	AAC_TT_LATM_MCP1 = 3,  /**< Audio Mux Elements with muxConfigPresent = 1 */
	AAC_TT_LATM_MCP0 = 4,  /**< Audio Mux Elements with muxConfigPresent = 0, out of band StreamMuxConfig */

	AAC_TT_LOAS = 5, /**< Audio Sync Stream. */
} AAC_TRANSPORT_TYPE;

int16_t WebRtcAAC_EncoderCreate(AACEncInst** inst, AAC_AOT aot, int32_t samplerate, int32_t channels);
int16_t WebRtcAAC_EncoderFree(AACEncInst* inst);

/****************************************************************************
 * WebRtcAAC_Encode(...)
 *
 * This function encodes audio as a series of AAC frames and inserts
 * it into a packet. Input buffer can be any length.
 *
 * Input:
 *      - inst                  : Encoder context
 *      - audio_in              : Input speech data buffer
 *      - samples               : Samples per channel in audio_in
 *      - length_encoded_buffer : Output buffer size
 *
 * Output:
 *      - encoded               : Output compressed data buffer
 *
 * Return value                 : >0 - Length (in bytes) of coded data
 *                                -1 - Error
 */
int16_t WebRtcAAC_Encode(AACEncInst* inst, int16_t* audio_in, int16_t samples,
                          int16_t length_encoded_buffer, uint8_t* encoded);

/****************************************************************************
 * WebRtcAAC_SetBitRate(...)
 *
 * This function adjusts the target bitrate of the encoder.
 *
 * Input:
 *      - inst               : Encoder context
 *      - rate               : New target bitrate
 *
 * Return value              :  0 - Success
 *                             -1 - Error
 */
int16_t WebRtcAAC_SetBitRate(AACEncInst* inst, int32_t rate);

/****************************************************************************
 * WebRtcAAC_SetPacketLossRate(...)
 *
 * This function configures the encoder's expected packet loss percentage.
 *
 * Input:
 *      - inst               : Encoder context
 *      - loss_rate          : loss percentage in the range 0-100, inclusive.
 * Return value              :  0 - Success
 *                             -1 - Error
 */
int16_t WebRtcAAC_SetPacketLossRate(AACEncInst* inst, int32_t loss_rate);

/* TODO(minyue): Check whether an API to check the FEC and the packet loss rate
 * is needed. It might not be very useful since there are not many use cases and
 * the caller can always maintain the states. */

/****************************************************************************
 * WebRtcAAC_EnableFec()
 *
 * This function enables FEC for encoding.
 *
 * Input:
 *      - inst               : Encoder context
 *
 * Return value              :  0 - Success
 *                             -1 - Error
 */
int16_t WebRtcAAC_EnableFec(AACEncInst* inst);

/****************************************************************************
 * WebRtcAAC_DisableFec()
 *
 * This function disables FEC for encoding.
 *
 * Input:
 *      - inst               : Encoder context
 *
 * Return value              :  0 - Success
 *                             -1 - Error
 */
int16_t WebRtcAAC_DisableFec(AACEncInst* inst);

/*
 * WebRtcAAC_SetComplexity(...)
 *
 * This function adjusts the computational complexity. The effect is the same as
 * calling the complexity setting of AAC as an AAC encoder related CTL.
 *
 * Input:
 *      - inst               : Encoder context
 *      - complexity         : New target complexity (0-10, inclusive)
 *
 * Return value              :  0 - Success
 *                             -1 - Error
 */
int16_t WebRtcAAC_SetComplexity(AACEncInst* inst, int32_t complexity);






int16_t WebRtcAAC_DecoderCreate(AACDecInst** inst, AAC_TRANSPORT_TYPE tt);
int16_t WebRtcAAC_DecoderFree(AACDecInst* inst);

/****************************************************************************
 * WebRtcAAC_DecoderChannels(...)
 *
 * This function returns the number of channels created for AAC decoder.
 */
int WebRtcAAC_DecoderChannels(AACDecInst* inst);

/****************************************************************************
 * WebRtcAAC_DecoderInit(...)
 *
 * This function resets state of the decoder.
 *
 * Input:
 *      - inst               : Decoder context
 *
 * Return value              :  0 - Success
 *                             -1 - Error
 */
int16_t WebRtcAAC_DecoderInit(AACDecInst* inst);

/****************************************************************************
 * WebRtcAAC_Decode(...)
 *
 * This function decodes an AAC packet into one or more audio frames at the
 * ACM interface's sampling rate (32 kHz).
 *
 * Input:
 *      - inst               : Decoder context
 *      - encoded            : Encoded data
 *      - encoded_bytes      : Bytes in encoded vector
 *
 * Output:
 *      - decoded            : The decoded vector
 *      - audio_type         : 1 normal, 2 CNG (for AAC it should
 *                             always return 1 since we're not using AAC's
 *                             built-in DTX/CNG scheme)
 *
 * Return value              : >0 - Samples per channel in decoded vector
 *                             -1 - Error
 */
int16_t WebRtcAAC_Decode(AACDecInst* inst, const uint8_t* encoded,
                          int16_t encoded_bytes, int16_t* decoded,
                          int16_t* audio_type);

/****************************************************************************
 * WebRtcAAC_DecodePlc(...)
 * TODO(tlegrand): Remove master and slave functions when NetEq4 is in place.
 * WebRtcAAC_DecodePlcMaster(...)
 * WebRtcAAC_DecodePlcSlave(...)
 *
 * This function processes PLC for AAC frame(s).
 * Input:
 *        - inst                  : Decoder context
 *        - max_decoded_shorts    : max size of |decoded| in shorts
 *        - number_of_lost_frames : Number of PLC frames to produce
 *
 * Output:
 *        - decoded               : The decoded vector
 *
 * Return value                   : >0 - number of samples in decoded PLC vector
 *                                  -1 - Error
 */
int16_t WebRtcAAC_DecodePlc(AACDecInst* inst, int16_t* decoded, const int32_t max_decoded_shorts,
                             int16_t number_of_lost_frames);

/****************************************************************************
 * WebRtcAAC_DecodeFec(...)
 *
 * This function decodes the FEC data from an AAC packet into one or more audio
 * frames at the ACM interface's sampling rate (32 kHz).
 *
 * Input:
 *      - inst               : Decoder context
 *      - encoded            : Encoded data
 *      - encoded_bytes      : Bytes in encoded vector
 *
 * Output:
 *      - decoded            : The decoded vector (previous frame)
 *
 * Return value              : >0 - Samples per channel in decoded vector
 *                              0 - No FEC data in the packet
 *                             -1 - Error
 */
int16_t WebRtcAAC_DecodeFec(AACDecInst* inst, const uint8_t* encoded,
                             int16_t encoded_bytes, int16_t* decoded,
                             int16_t* audio_type);

/****************************************************************************
 * WebRtcAAC_DurationEst(...)
 *
 * This function calculates the duration of an AAC packet.
 * Input:
 *        - inst                 : Decoder context
 *        - payload              : Encoded data pointer
 *        - payload_length_bytes : Bytes of encoded data
 *
 * Return value                  : The duration of the packet, in samples.
 */
int WebRtcAAC_DurationEst(AACDecInst* inst,
                           const uint8_t* payload,
                           int payload_length_bytes);

/* TODO(minyue): Check whether it is needed to add a decoder context to the
 * arguments, like WebRtcAAC_DurationEst(...). In fact, the packet itself tells
 * the duration. The decoder context in WebRtcAAC_DurationEst(...) is not used.
 * So it may be advisable to remove it from WebRtcAAC_DurationEst(...). */

/****************************************************************************
 * WebRtcAAC_FecDurationEst(...)
 *
 * This function calculates the duration of the FEC data within an AAC packet.
 * Input:
 *        - payload              : Encoded data pointer
 *        - payload_length_bytes : Bytes of encoded data
 *
 * Return value                  : >0 - The duration of the FEC data in the
 *                                 packet in samples.
 *                                  0 - No FEC data in the packet.
 */
int WebRtcAAC_FecDurationEst(const uint8_t* payload,
                              int payload_length_bytes);

/****************************************************************************
 * WebRtcAAC_PacketHasFec(...)
 *
 * This function detects if an AAC packet has FEC.
 * Input:
 *        - payload              : Encoded data pointer
 *        - payload_length_bytes : Bytes of encoded data
 *
 * Return value                  : 0 - the packet does NOT contain FEC.
 *                                 1 - the packet contains FEC.
 */
int WebRtcAAC_PacketHasFec(const uint8_t* payload,
                            int payload_length_bytes);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // WEBRTC_MODULES_AUDIO_CODING_CODECS_AAC_INTERFACE_AAC_INTERFACE_H_
