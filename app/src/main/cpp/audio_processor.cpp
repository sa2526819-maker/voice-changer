#include <jni.h>
#include <oboe/Oboe.h>
#include <vector>
#include <cmath>

using namespace oboe;

class HindiVoiceEngine : public AudioStreamDataCallback {
public:
    AudioStream *recordingStream = nullptr;
    
    // Hindi Female Tone Calibration
    const float PITCH_FACTOR = 1.34f;
    const float FORMANT_WARP = 1.18f;

    void start() {
        AudioStreamBuilder inBuilder;
        inBuilder.setDirection(Direction::Input)
                 ->setPerformanceMode(PerformanceMode::LowLatency)
                 ->setSharingMode(SharingMode::Exclusive)
                 ->setFormat(AudioFormat::Float)
                 ->setChannelCount(1)
                 ->setSampleRate(48000)
                 ->setDataCallback(this)
                 ->openStream(&recordingStream);

        recordingStream->requestStart();
    }

    DataCallbackResult onAudioReady(AudioStream *stream, void *audioData, int32_t numFrames) override {
        float *buffer = static_cast<float *>(audioData);

        for (int i = 0; i < numFrames; i++) {
            float sample = buffer[i];

            // Formant & Resonance balance
            float warped = sample * FORMANT_WARP;
            float processed = std::sin(warped * PITCH_FACTOR * 1.5707963f);

            if (processed > 0.95f) processed = 0.95f;
            if (processed < -0.95f) processed = -0.95f;

            buffer[i] = processed;
        }

        return DataCallbackResult::Continue;
    }

    void stop() {
        if (recordingStream) {
            recordingStream->stop();
            recordingStream->close();
        }
    }
};

static HindiVoiceEngine engine;

extern "C" {
    JNIEXPORT void JNICALL Java_com_game_voicechanger_VoiceService_startVoiceEngine(JNIEnv *env, jobject thiz) {
        engine.start();
    }

    JNIEXPORT void JNICALL Java_com_game_voicechanger_VoiceService_stopVoiceEngine(JNIEnv *env, jobject thiz) {
        engine.stop();
    }
}
