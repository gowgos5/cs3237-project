#include "kiss_fftr.h"
#include "fft_analyzer.h"

FFTAnalyzer::FFTAnalyzer(float sampling_frequency, float window_time) {
    initialized = false;
    
    fs = sampling_frequency;
    if (fs <= 0) {
        return;
    }

    if (window_time == 0) {
        w = (int) (fs * DEFAULT_WINDOW_TIME);
    } else {
        w = (int) (fs * window_time);
    }
    if (w % 2 == 1) {
        w -= 1;
    }
    if (w <= 0) {
        return;
    } else if (w > MAX_WINDOW_SIZE) {
        w = MAX_WINDOW_SIZE;
    }

    f_res = fs / w;

    cfg = kiss_fftr_alloc(w, 0, NULL, NULL);
    in_signal_size = w;
    out_spectrum_size = w / 2 + 1;

    cutoff_spectrum_index = ((int) ((((float) MAX_HEART_RATE) / SECONDS_PER_MIN) / f_res)) + 1;
    if (cutoff_spectrum_index > out_spectrum_size) {
        cutoff_spectrum_index = out_spectrum_size;
    }

    large_mean_window_buffer_size = ((int) (fs / 2)) * 2 + 1;
    if (large_mean_window_buffer_size > MAX_LARGE_MEAN_BUFFER_SIZE) {
        large_mean_window_buffer_size = MAX_LARGE_MEAN_BUFFER_SIZE;
    }

    small_mean_window_buffer_size = ((int) (fs / 20)) * 2 + 1;
    if (small_mean_window_buffer_size > MAX_SMALL_MEAN_BUFFER_SIZE) {
        small_mean_window_buffer_size = MAX_SMALL_MEAN_BUFFER_SIZE;
    }

    signal_buffer_size = w;

    reset();
    initialized = true;
}

bool FFTAnalyzer::isInitialized() {
    return initialized;
}

void FFTAnalyzer::reset() {
    if (!initialized) {
        return;
    }

    ready = false;

    jump = 0;
    last_raw_signal = 0;
    have_prev_raw = false;

    large_mean_window_buffer_input_count = 0;
    large_mean_window_next_pointer_index = 0;
    large_mean_window_sum = 0;

    small_mean_window_buffer_input_count = 0;
    small_mean_window_next_pointer_index = 0;
    small_mean_window_sum = 0;

    signal_buffer_input_count = 0;
    signal_next_pointer_index = 0;
}

void FFTAnalyzer::addSignalData(float x) {
    if (!initialized) {
        return;
    }

    // remove jump
    float x1;
    if (have_prev_raw) {
        float diff = x - last_raw_signal;
        if ((diff > JUMP_THRESHOLD) || (diff < -JUMP_THRESHOLD)) {
            jump += diff;
        }
        x1 = x - jump;
    } else {
        x1 = x;
        have_prev_raw = true;
        // re-initialize large mean buffer
        large_mean_window_buffer_input_count = 0;
        large_mean_window_next_pointer_index = 0;
        large_mean_window_sum = 0;
    }
    last_raw_signal = x;

    // remove large mean line
    float x2;
    bool x2_returned = false;
    if (large_mean_window_buffer_input_count < large_mean_window_buffer_size) {
        large_mean_window_buffer[large_mean_window_next_pointer_index] = x1;
        large_mean_window_sum += x1;
        large_mean_window_buffer_input_count += 1;
    } else {
        large_mean_window_sum -= large_mean_window_buffer[large_mean_window_next_pointer_index];
        large_mean_window_buffer[large_mean_window_next_pointer_index] = x1;
        large_mean_window_sum += x1;
    }
    large_mean_window_next_pointer_index += 1;
    if (large_mean_window_next_pointer_index >= large_mean_window_buffer_size) {
        large_mean_window_next_pointer_index -= large_mean_window_buffer_size;
    }
    if (large_mean_window_buffer_input_count >= large_mean_window_buffer_size) {
        int large_mid_index = large_mean_window_next_pointer_index + (large_mean_window_buffer_size - 1) / 2;
        if (large_mid_index >= large_mean_window_buffer_size) {
            large_mid_index -= large_mean_window_buffer_size;
        }
        x2 = large_mean_window_buffer[large_mid_index] - large_mean_window_sum / large_mean_window_buffer_size;
        x2_returned = true;
    }
    if (!x2_returned) {
        // re-initialize small mean buffer
        small_mean_window_buffer_input_count = 0;
        small_mean_window_next_pointer_index = 0;
        small_mean_window_sum = 0;
        return;
    }

    // extract small mean line
    float x3;
    bool x3_returned = false;
    if (small_mean_window_buffer_input_count < small_mean_window_buffer_size) {
        small_mean_window_buffer[small_mean_window_next_pointer_index] = x2;
        small_mean_window_sum += x2;
        small_mean_window_buffer_input_count += 1;
    } else {
        small_mean_window_sum -= small_mean_window_buffer[small_mean_window_next_pointer_index];
        small_mean_window_buffer[small_mean_window_next_pointer_index] = x2;
        small_mean_window_sum += x2;
    }
    small_mean_window_next_pointer_index += 1;
    if (small_mean_window_next_pointer_index >= small_mean_window_buffer_size) {
        small_mean_window_next_pointer_index -= small_mean_window_buffer_size;
    }
    if (small_mean_window_buffer_input_count >= small_mean_window_buffer_size) {
        x3 = small_mean_window_sum / small_mean_window_buffer_size;
        x3_returned = true;
    }
    if (!x3_returned) {
        // re-initialize signal buffer
        signal_buffer_input_count = 0;
        signal_next_pointer_index = 0;
        return;
    }

    // add processed data to circular buffer
    signal_buffer[signal_next_pointer_index] = x3;
    signal_next_pointer_index += 1;
    if (signal_next_pointer_index >= signal_buffer_size) {
        signal_next_pointer_index -= signal_buffer_size;
    }
    if (signal_buffer_input_count < signal_buffer_size) {
        signal_buffer_input_count += 1;
        if (signal_buffer_input_count >= signal_buffer_size) {
            ready = true;
        }
    }
}

bool FFTAnalyzer::isReady() {
    return (initialized && ready);
}

void FFTAnalyzer::fillInput() {
    int i = 0;
    for (int j = signal_next_pointer_index; j < signal_buffer_size; j++) {
        in_signal[i] = (kiss_fft_scalar) (signal_buffer[j]);
        i++;
    }
    for (int j = 0; j < signal_next_pointer_index; j++) {
        in_signal[i] = (kiss_fft_scalar) (signal_buffer[j]);
        i++;
    }
}

float FFTAnalyzer::freqMaxPeak() {
    // find the highest peak
    int max_peak_index = 0;
    float max_peak_value = 0;
    for (int j = 0; j < cutoff_spectrum_index; j++) {
        float r_part = (float) (out_spectrum[j].r);
        float i_part = (float) (out_spectrum[j].i);
        float value = r_part * r_part + i_part * i_part;
        if (value > max_peak_value) {
            max_peak_value = value;
            max_peak_index = j;
        }
    }
    if (max_peak_value == 0) {
        return 0;
    }

    // find the hr between the highest peak and its neighbors
    float curr = max_peak_index * f_res * SECONDS_PER_MIN;
    float curr_value = max_peak_value;
    float prev = (max_peak_index - 1) * f_res * SECONDS_PER_MIN;
    float prev_value = 0;
    if (max_peak_index - 1 >= 0) {
        float r_part = (float) (out_spectrum[max_peak_index - 1].r);
        float i_part = (float) (out_spectrum[max_peak_index - 1].i);
        prev_value = r_part * r_part + i_part * i_part;
    }
    float next = (max_peak_index + 1) * f_res * SECONDS_PER_MIN;
    float next_value = 0;
    if (max_peak_index + 1 < out_spectrum_size) {
        float r_part = (float) (out_spectrum[max_peak_index + 1].r);
        float i_part = (float) (out_spectrum[max_peak_index + 1].i);
        next_value = r_part * r_part + i_part * i_part;
    }
    float hr;
    if (prev_value > next_value) {
        prev_value -= next_value;
        curr_value -= next_value;
        hr = curr - (curr - prev) * prev_value / (prev_value + curr_value);
    } else if (prev_value < next_value) {
        next_value -= prev_value;
        curr_value -= prev_value;
        hr = curr + (next - curr) * next_value / (next_value + curr_value);
    } else {
        hr = curr;
    }

    // ignore small value of heartrate
    if (hr < MIN_HEART_RATE) {
        hr = 0;
    }
    return hr;
}

float FFTAnalyzer::getHeartRate() {
    if (!isReady()) {
        return -1.0;
    }
    fillInput();
    kiss_fftr(cfg, in_signal, out_spectrum);
    return freqMaxPeak();
}

void FFTAnalyzer::freeFftMem() {
    initialized = false;
    kiss_fftr_free(cfg);
    free(in_signal);
    free(out_spectrum);
    free(large_mean_window_buffer);
    free(small_mean_window_buffer);
    free(signal_buffer);
}
