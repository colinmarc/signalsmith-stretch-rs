#ifndef SIGNALSMITH_STRETCH_WRAPPER_H
#define SIGNALSMITH_STRETCH_WRAPPER_H

#include <stddef.h>
#include <stdbool.h>

#define EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
extern "C" {
#endif

struct signalsmith_stretch;
typedef struct signalsmith_stretch signalsmith_stretch_t;

// Creates a new SignalsmithStretch instance.
EXPORT signalsmith_stretch_t *signalsmith_stretch_create(int channel_count, size_t block_length, size_t interval);
EXPORT signalsmith_stretch_t *signalsmith_stretch_create_preset_default(int channel_count, float sample_rate);
EXPORT signalsmith_stretch_t *signalsmith_stretch_create_preset_cheaper(int channel_count, float sample_rate);

// Destroys a SignalsmithStretch instance.
EXPORT void signalsmith_stretch_destroy(signalsmith_stretch_t *handle);

// Resets the instance.
EXPORT void signalsmith_stretch_reset(signalsmith_stretch_t *handle);

// Gets the current input latency in frames.
EXPORT size_t signalsmith_stretch_input_latency(signalsmith_stretch_t *handle);

// Gets the current output latency in frames.
EXPORT size_t signalsmith_stretch_output_latency(signalsmith_stretch_t *handle);

// Provides input, or "pre-roll", without affecting the stream position.
EXPORT void signalsmith_stretch_seek(signalsmith_stretch_t *handle, float *input, size_t input_length, double playback_rate);

// Set the frequency multiplier and an optional tonality limit (pass zero for
// no limit).
EXPORT void signalsmith_stretch_set_transpose_factor(signalsmith_stretch_t *handle, float multiplier, float tonality_limit);

// Set the frequency multiplier in semitones, as well as an optional tonality
// limit (pass zero for no limit).
EXPORT void signalsmith_stretch_set_transpose_factor_semitones(signalsmith_stretch_t *handle, float multiplier, float tonality_limit);

// Set formant shift factor, with an option to compensate for pitch.
EXPORT void signalsmith_stretch_set_formant_factor(signalsmith_stretch_t *handle, float multiplier, int compensate_pitch);

// Set formant shift in semitones, with an option to compensate for pitch.
EXPORT void signalsmith_stretch_set_formant_factor_semitones(signalsmith_stretch_t *handle, float semitones, int compensate_pitch);

// Rough guesstimate of the fundamental frequency, used for formant analysis.
// 0 means attempting to detect the pitch.
EXPORT void signalsmith_stretch_set_formant_base(signalsmith_stretch_t *handle, float frequency);

// Processes interleaved input samples and stores the result in the output buffer.
// The input and output buffers must interleaved, with the correct number of
// channels. The length arguments refer to the number of frames, i.e. the number
// of samples per channel.
EXPORT void signalsmith_stretch_process(signalsmith_stretch_t *handle,
                                        float *input, size_t input_length,
                                        float *output, size_t output_length);


// Process a complete audio buffer all in one go.
EXPORT bool signalsmith_stretch_exact(signalsmith_stretch_t *handle,
                                      float *input, size_t input_length,
                                      float *output, size_t output_length);

// Read the remaining output. `signalsmith_stretch_output_latency` will return
// the correct size for the output buffer.
EXPORT void signalsmith_stretch_flush(signalsmith_stretch_t *handle,
                                      float *output, size_t output_length);

#ifdef __cplusplus
}
#endif

#endif // SIGNALSMITH_STRETCH_WRAPPER_H
