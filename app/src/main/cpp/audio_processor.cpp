#include <jni.h>
#include <oboe/Oboe.h>
#include <vector>

using namespace oboe;

class VoiceEngine : public AudioStreamDataCallback {
public:
    AudioStream *recordingStream = nullptr;
    AudioStream *playbackStream = nullptr;
    
    float pitchShift = 1.32f; 

    void start() {
        AudioStreamBuilder inBuilder;

        inBuilder.setDirection(Direction::Input)
                 ->setPerformanceMode(PerformanceMode::LowLatency)
                 ->setSharingMode(SharingMode::Exclusive)
                 ->setFormat(AudioFormat::Float)
                 ->setChannelCount(1)
                 ->setDataCallback(this)
                 ->openStream(&recordingStream);

        recordingStream->requestStart();
    }

    DataCallbackResult onAudioReady(AudioStream *stream, void *audioData, int32_t numFrames) override {
        float *buffer = static_cast<float *>(audioData);

        for (int i = 0; i < numFrames; i++) {
            float sample = buffer[i] * pitchShift;
            if (sample > 1.0f) sample = 1.0f;
            if (sample < -1.0f) sample = -1.0f;
            buffer[i] = sample;
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

static VoiceEngine engine;

extern "C" {
    JNIEXPORT void JNICALL Java_com_game_voicechanger_VoiceService_startVoiceEngine(JNIEnv *env, jobject thiz) {
        engine.start();
    }

    JNIEXPORT void JNICALL Java_com_game_voicechanger_VoiceService_stopVoiceEngine(JNIEnv *env, jobject thiz) {
        engine.stop();
    }
}
