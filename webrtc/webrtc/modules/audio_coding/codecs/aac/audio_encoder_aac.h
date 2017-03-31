/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_CODING_CODECS_AAC_AUDIO_ENCODER_AAC_H_
#define WEBRTC_MODULES_AUDIO_CODING_CODECS_AAC_AUDIO_ENCODER_AAC_H_

#include <vector>

#include "webrtc/base/constructormagic.h"
#include "webrtc/modules/audio_coding/codecs/aac/aac_interface.h"
#include "webrtc/modules/audio_coding/codecs/audio_encoder.h"

namespace webrtc {

struct CodecInst;

class AudioEncoderAAC final : public AudioEncoder {
 public:
     enum ApplicationMode {
         kVoip = 0,
         kAudio = 1,
     };
  struct Config {
    bool IsOk() const;
    int frame_size_ms = 20;
    size_t num_channels = 1;
    int payload_type = 121;
    ApplicationMode application = kVoip;
    int bitrate_bps = 64000;
    bool fec_enabled = false;
    int max_playback_rate_hz = 48000;
    int complexity = 0;
    bool dtx_enabled = false;

  };

  explicit AudioEncoderAAC(const Config& config);
  explicit AudioEncoderAAC(const CodecInst& codec_inst);
  ~AudioEncoderAAC() override;

  int SampleRateHz() const override;
  size_t NumChannels() const override;
  size_t Num10MsFramesInNextPacket() const override;
  size_t Max10MsFramesInAPacket() const override;
  int GetTargetBitrate() const override;

  void Reset() override;
  bool SetFec(bool enable) override;

  // Set Opus DTX. Once enabled, Opus stops transmission, when it detects voice
  // being inactive. During that, it still sends 2 packets (one for content, one
  // for signaling) about every 400 ms.
  bool SetDtx(bool enable) override;

  bool SetApplication(Application application) override;
  void SetMaxPlaybackRate(int frequency_hz) override;
  void SetProjectedPacketLossRate(double fraction) override;
  void SetTargetBitrate(int target_bps) override;

  // Getters for testing.
  double packet_loss_rate() const { return packet_loss_rate_; }
  //ApplicationMode application() const { return config_.application; }
  bool dtx_enabled() const { return config_.dtx_enabled; }

 protected:
  EncodedInfo EncodeImpl(uint32_t rtp_timestamp,
                         rtc::ArrayView<const int16_t> audio,
                         rtc::Buffer* encoded) override;

 private:
  size_t Num10msFramesPerPacket() const;
  size_t SamplesPer10msFrame() const;
  size_t SufficientOutputBufferSize() const;
  bool RecreateEncoderInstance(const Config& config);

  Config config_;
  double packet_loss_rate_;
  std::vector<int16_t> input_buffer_;
  AACEncInst* inst_;
  uint32_t first_timestamp_in_buffer_;
  RTC_DISALLOW_COPY_AND_ASSIGN(AudioEncoderAAC);
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_AUDIO_CODING_CODECS_OPUS_AUDIO_ENCODER_OPUS_H_
