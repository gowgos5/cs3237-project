package com.example.android.bluetoothlegatt;

import com.paramsen.noise.Noise;

import java.lang.Math;
import java.util.LinkedList;
import java.util.Queue;

public class FftAnalyzer {
    private static final int SECONDS_PER_MIN = 60;

    private static final int MAX_WINDOW_SIZE = 8192;        // 1bps resolution at fs = 400 Hz
    private static final float DEFAULT_WINDOW_TIME = 15f;    // 15 seconds of data (4bps res)

    private static final int MIN_HEART_RATE = 30;   // min heart rate that can be calculated (bpm)
    private static final int MAX_HEART_RATE = 300;  // max heart rate that can be calculated (bpm)

    private static final float MIN_PEAK_VALUE = 450f;

    // Flags
    private boolean isInitialized;          // analyzer is successfully initialized
    private boolean isReady;                // analyzer is ready to calculate heart rate

    // Frequency
    private float fs;
    private float f_res;                    // frequency resolution

    // FFT
    private int w;
    private Noise noise;                    // Kiss FFT wrapper for Java
    private float[] input_signal;
    private float[] output_spectrum;
    private float[] fft;                    // Pointer to output of FFT
    private int cutoff_index;

    // Long term mean line elimination
    private Queue<Float> left_large_buffer = new LinkedList<>();
    private Queue<Float> right_large_buffer = new LinkedList<>();
    private float mid_large_buffer;
    private boolean has_mid_value;
    private int large_buffer_size;
    private int half_large_buffer_size;
    private float large_buffer_sum;

    // Short term mean line extraction
    private Queue<Float> small_buffer = new LinkedList<>();
    private int small_buffer_size;
    private float small_buffer_sum;

    // Processed values
    private Queue<Float> signal_buffer = new LinkedList<>();

    private float peakValue;

    public FftAnalyzer(float sampling_freq, float window_time) {
        isInitialized = true;
        fs = sampling_freq;
        if (fs <= 0) {
            isInitialized = false;
            return;
        }
        setW(window_time);
        if (!isInitialized) {
            return;
        }
        f_res = fs / w;
        noise = Noise.real(w);
        input_signal = new float[w];
        output_spectrum = new float[w + 2];
        cutoff_index = (int) ((MAX_HEART_RATE / SECONDS_PER_MIN) / f_res);
        if (cutoff_index > ((w + 2) / 2)) {
            cutoff_index = ((w + 2) / 2);
        }
        half_large_buffer_size = (int) (fs / 2);
        if (half_large_buffer_size <= 0) {
            half_large_buffer_size = 1;
        }
        large_buffer_size = 2 * half_large_buffer_size + 1;
        small_buffer_size = 2 * ((int) (fs / 20)) + 1;
        reset();
    }

    public boolean isInitialized() {
        return isInitialized;
    }

    public void reset() {
        if (!isInitialized) {
            return;
        }
        isReady = false;
        has_mid_value = false;
        large_buffer_sum = 0f;
        small_buffer_sum = 0f;
        left_large_buffer.clear();
        right_large_buffer.clear();
        small_buffer.clear();
        signal_buffer.clear();
    }

    public void addData(int data) {
        if (!isInitialized) {
            return;
        }
        addDataLargeBuffer((float) data);
    }

    public boolean isReady() {
        return isInitialized && isReady;
    }

    public float getHeartRate() {
        if (!isReady()) {
            return -1;
        }
        fillInput();
        fft = noise.fft(input_signal, output_spectrum);
        return freqMaxPeak();
    }

    private void setW(float window_time) {
        if (!isInitialized) {
            return;
        }
        int desired_w;
        if (window_time <= 0) {
            desired_w = (int) (fs * DEFAULT_WINDOW_TIME);
        } else {
            desired_w = (int) (fs * window_time);
        }
        if (desired_w < 2) {
            isInitialized = false;
            return;
        }
        w = 2;
        while ((w < desired_w) && (w < MAX_WINDOW_SIZE)) {
            w *= 2;
        }
        if (w > MAX_WINDOW_SIZE) {
            w = MAX_WINDOW_SIZE;
        }
    }

    private void addDataLargeBuffer(float data) {
        right_large_buffer.add(data);
        large_buffer_sum += data;
        while (right_large_buffer.size() > half_large_buffer_size) {
            float value = right_large_buffer.remove();
            if (has_mid_value) {
                left_large_buffer.add(mid_large_buffer);
            }
            mid_large_buffer = value;
            has_mid_value = true;
        }
        while (left_large_buffer.size() > half_large_buffer_size) {
            float value = left_large_buffer.remove();
            large_buffer_sum -= value;
        }
        if ((left_large_buffer.size() == half_large_buffer_size)
                && (right_large_buffer.size() == half_large_buffer_size)) {
            float result = mid_large_buffer - large_buffer_sum / large_buffer_size;
            addDataSmallBuffer(result);
        }
    }

    private void addDataSmallBuffer(float data) {
        small_buffer.add(data);
        small_buffer_sum += data;
        while (small_buffer.size() > small_buffer_size) {
            float removed_value = small_buffer.remove();
            small_buffer_sum -= removed_value;
        }
        if (small_buffer.size() == small_buffer_size) {
            float result = small_buffer_sum / small_buffer_size;
            addDataSignalBuffer(result);
        }
    }

    private void addDataSignalBuffer(float data) {
        signal_buffer.add(data);
        while (signal_buffer.size() > w) {
            signal_buffer.remove();
        }
        if (signal_buffer.size() == w) {
            isReady = true;
        }
    }

    private void fillInput() {
        int i = 0;
        for (Float value: signal_buffer) {
            if (i < w) {
                input_signal[i] = value;
            }
            i++;
        }
    }

    private float freqMaxPeak() {
        int max_peak_index = 0;
        float max_peak_power = 0;
        for (int i = 0; i < cutoff_index; i++) {
            float power = fft[2*i] * fft[2*i] + fft[2*i+1] * fft[2*i+1];
            if (power > max_peak_power) {
                max_peak_index = i;
                max_peak_power = power;
            }
        }
        float curr = (float) Math.sqrt(max_peak_power);
        peakValue = curr;
        if (curr < MIN_PEAK_VALUE) {
            return 0;
        }
        float prev = 0;
        float next = 0;
        if (max_peak_index > 0) {
            int i = max_peak_index - 1;
            float power = fft[2*i] * fft[2*i] + fft[2*i+1] * fft[2*i+1];
            prev = (float) Math.sqrt(power);
        }
        if (max_peak_index < w) {
            int i = max_peak_index + 1;
            float power = fft[2*i] * fft[2*i] + fft[2*i+1] * fft[2*i+1];
            next = (float) Math.sqrt(power);
        }
        float hr;
        if (prev > next) {
            prev -= next;
            curr -= next;
            hr = (max_peak_index - (prev / (curr + prev))) * f_res * SECONDS_PER_MIN;
        } else if (prev < next) {
            next -= prev;
            curr -= prev;
            hr = (max_peak_index + (next / (curr + next))) * f_res * SECONDS_PER_MIN;
        } else {
            hr = max_peak_index * f_res * SECONDS_PER_MIN;
        }
        if (hr < MIN_HEART_RATE) {
            hr = 0;
        }
        return hr;
    }

    public float getPeakValue(){
        return peakValue;
    }
}
