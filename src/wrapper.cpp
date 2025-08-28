
#include "wrapper.h"
#include <signalsmith-stretch.h>

#include <stddef.h>
#include <vector>
#include <cstring>

// Allows channel-major indexing into interleaved buffers.
class InterleavedBuffer {
    float *data;
    int channelCount;

public:
    InterleavedBuffer(float *data, int channels)
        : data(data), channelCount(channels) {}

    class ChannelView {
        float *data;
        int channel;
        int stride;

    public:
        ChannelView(float *data, int channel, int stride)
            : data(data), channel(channel), stride(stride) {}

        float &operator[](size_t offset) {
            return data[(offset * stride) + channel];
        }

        const float &operator[](size_t offset) const {
            return data[(offset * stride) + channel];
        }
    };

    ChannelView operator[](size_t channel) {
        if (channel >= static_cast<size_t>(channelCount)) {
            throw std::out_of_range("Channel index out of range");
        }

        return ChannelView(data, channel, channelCount);
    }
};

struct signalsmith_stretch {
    signalsmith::stretch::SignalsmithStretch<float> instance;
    int channel_count;
};

signalsmith_stretch_t *signalsmith_stretch_create(int channel_count, size_t block_length, size_t interval) {
    auto handle = new signalsmith_stretch;
    handle->channel_count = channel_count;
    handle->instance.configure(channel_count, block_length, interval);

    return handle;
}

signalsmith_stretch_t *signalsmith_stretch_create_preset_default(int channel_count, float sample_rate) {
    auto handle = new signalsmith_stretch;
    handle->channel_count = channel_count;
    handle->instance.presetDefault(channel_count, sample_rate);

    return handle;
}

signalsmith_stretch_t *signalsmith_stretch_create_preset_cheaper(int channel_count, float sample_rate) {
    auto handle = new signalsmith_stretch;
    handle->channel_count = channel_count;
    handle->instance.presetCheaper(channel_count, sample_rate);

    return handle;
}

void signalsmith_stretch_destroy(signalsmith_stretch_t *handle) {
    delete handle;
}

void signalsmith_stretch_reset(signalsmith_stretch_t *handle) {
    return handle->instance.reset();
}

size_t signalsmith_stretch_input_latency(signalsmith_stretch_t *handle) {
    return static_cast<size_t>(handle->instance.inputLatency());
}

size_t signalsmith_stretch_output_latency(signalsmith_stretch_t *handle) {
    return static_cast<size_t>(handle->instance.outputLatency());
}

void signalsmith_stretch_set_transpose_factor(signalsmith_stretch_t *handle, float multiplier, float tonality_limit) {
    handle->instance.setTransposeFactor(multiplier, tonality_limit);
}

void signalsmith_stretch_set_transpose_factor_semitones(signalsmith_stretch_t *handle, float semitones, float tonality_limit) {
    handle->instance.setTransposeSemitones(semitones, tonality_limit);
}

void signalsmith_stretch_set_formant_factor(signalsmith_stretch_t *handle, float multiplier, int compensate_pitch) {
    handle->instance.setFormantFactor(multiplier, compensate_pitch);
}

void signalsmith_stretch_set_formant_factor_semitones(signalsmith_stretch_t *handle, float semitones, int compensate_pitch) {
    handle->instance.setFormantSemitones(semitones, compensate_pitch);
}

void signalsmith_stretch_set_formant_base(signalsmith_stretch_t *handle, float frequency) {
    handle->instance.setFormantSemitones(frequency);
}

void signalsmith_stretch_seek(signalsmith_stretch_t *handle, float *input, size_t input_length, double playback_rate) {
    InterleavedBuffer interleaved(input, handle->channel_count);
    handle->instance.seek(interleaved, input_length, playback_rate);
}

void signalsmith_stretch_process(signalsmith_stretch_t *handle,
                                 float *input, size_t input_length,
                                 float *output, size_t output_length) {
    InterleavedBuffer interleavedInput(input, handle->channel_count);
    InterleavedBuffer interleavedOutput(output, handle->channel_count);

    handle->instance.process(interleavedInput, input_length, interleavedOutput, output_length);
}

bool signalsmith_stretch_exact(signalsmith_stretch_t *handle,
                                 float *input, size_t input_length,
                                 float *output, size_t output_length) {
    InterleavedBuffer interleavedInput(input, handle->channel_count);
    InterleavedBuffer interleavedOutput(output, handle->channel_count);

    return handle->instance.exact(interleavedInput, input_length, interleavedOutput, output_length);
}

void signalsmith_stretch_flush(signalsmith_stretch_t *handle,
                               float *output, size_t output_length) {
    InterleavedBuffer interleavedOutput(output, handle->channel_count);

    handle->instance.flush(interleavedOutput, output_length);
}
