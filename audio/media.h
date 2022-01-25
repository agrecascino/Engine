#ifndef MEDIA_H
#define MEDIA_H

#include <libopenmpt/libopenmpt.h>
#include <libopenmpt/libopenmpt_stream_callbacks_file.h>
#include <AL/al.h>
#include <AL/alut.h>
#include "deps/dr_wav.h"
#include <chrono>
#include <thread>
#include <vector>

enum MediaType {
    MODULE,
    WAVE,
    UNLOADED
};


struct MediaFile {
    MediaFile(void) { type = UNLOADED; }

    MediaFile(openmpt_module *mod) {
        type = MODULE;
        repmod = mod;
    }

    MediaFile(drwav loaded) {
        type = MODULE;
        repwav = loaded;
    }

    MediaType type;
    openmpt_module *repmod;
    drwav repwav;
};

enum MediaStreamerError {
    OKAY,
    NO_MORE_POLYPHONY,
    NO_SOUND_FOUND
};

class MediaStreamer {
public:
    MediaStreamer() {
        alutInit(NULL, NULL);
        alGenSources(32, sources);
        memset(&sourcestate, 0, sizeof(bool) * 32);
    }
    void songPlayerWorker(openmpt_module *mod) {
        int status;
        std::vector<ALuint> buffer;
        int buffers = 0;
        while (true) {
            int16_t samples[11025 * 2];
            alGetSourcei(sources[0], AL_BUFFERS_PROCESSED, &status);
            alGetSourcei(sources[0], AL_BUFFERS_QUEUED, &buffers);
            if ((status + 1) < (buffers)) {
                continue;
            }
            if (status > 0 && status <= buffer.size())
            {
                alSourceUnqueueBuffers(sources[0], status, buffer.data());
                alDeleteBuffers(status, &buffer[0]);
                buffer.erase(buffer.begin(), buffer.begin() + status);
            }
            buffer.push_back(0);
            alGenBuffers(1, &buffer.back());
            std::this_thread::sleep_for(std::chrono::milliseconds((11025 / 44100) * 1000));
            openmpt_module_read_interleaved_stereo(mod, 44100, 11025, samples);
            alGetSourcei(sources[0], AL_SOURCE_STATE, &status);
            alBufferData(buffer.back(), AL_FORMAT_STEREO16, samples, 22050 * 2, 44100);
            alSourceQueueBuffers(sources[0], 1, &buffer.back());
            alGetSourcei(sources[0], AL_SOURCE_STATE, &status);
            if (status != AL_PLAYING) {
                alSourcePlay(sources[0]);
            }
            if(end) {
                goto end;
            }
        }
end:
        openmpt_module_destroy(mod);
    }

    void sfxPlayerWorker(int sid, ALint format, int16_t *samples, uint32_t len) {
        int status;

        ALuint buffer;
        alGenBuffers(1, &buffer);
        alBufferData(buffer, format, samples, len*((format == AL_FORMAT_STEREO16) ? 2 : 1), 44100);
        alSourcei(sources[sid], AL_BUFFER, buffer);
        int buffers = 0;
        alSourcePlay(sources[sid]);
        alGetSourcei(sources[sid], AL_SOURCE_STATE, &status);
        while (status == AL_PLAYING) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            alGetSourcei(sources[sid], AL_SOURCE_STATE, &status);
        }
        delete[] samples;
        alDeleteBuffers(1, &buffer);
        sourcestate[sid] = false;
    }

    int playModule(openmpt_module *mod) {
        t[0] = std::thread(&MediaStreamer::songPlayerWorker, this, mod);
        t[0].detach();
        return true;
    }

    MediaStreamerError sfxQueue(unsigned long id) {

    }

    int QueueNewSound(int16_t *ptr, int size, int stereo) {
        ALint state;
        for (int i = 1; i < 32; i++) {
            alGetSourcei(sources[i], AL_SOURCE_STATE, &state);
            if (!sourcestate[i]) {
                sourcestate[i] = true;
                int16_t *copy = new int16_t[size];
                memcpy(copy, ptr, size * 2);
                ALint format = stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
                t[i] = std::thread(&MediaStreamer::sfxPlayerWorker, this, i, format, copy, size);
                t[i].detach();
                return true;
            }
        }
        return false;
    }
    ~MediaStreamer() {
        end = true;
        if(t[0].joinable()) {
            t[0].join();
        }
    }

private:

    bool end = false;
    bool sourcestate[32];
    std::thread t[32];
    ALuint sources[32];
};


#endif // MEDIA_H
