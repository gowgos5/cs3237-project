#ifndef FFT_ANALYZER_H
#define FFT_ANALYZER_H

#include "kiss_fftr.h"

#define SECONDS_PER_MIN 60

#define SIGNAL_ARRAY_SIZE 6010
#define SPECTRUM_ARRAY_SIZE 3010

#define MAX_WINDOW_SIZE 6000    // 1bps resolution at fs = 400 Hz
#define DEFAULT_WINDOW_TIME 15  // 15 seconds of data (4bps res)

#define JUMP_THRESHOLD 100      // threshold to eliminate signal jumps

#define LARGE_BUFFER_SIZE 410
#define SMALL_BUFFER_SIZE 50
#define MAX_LARGE_MEAN_BUFFER_SIZE 401
#define MAX_SMALL_MEAN_BUFFER_SIZE 41

#define MIN_HEART_RATE 30       // minimum heart rate that can be calculated (bpm)
#define MAX_HEART_RATE 300      // maximum heart rate that can be calculated (bpm)

class FFTAnalyzer {
private:
    float fs;           // sampling frequency
    int w;              // window size
    float f_res;        // frequency resolution
    bool initialized;   // analyzer is successfully initialized
    bool ready;         // analyzer can start output heart rate
    
    // kiss fft alocated buffer
    kiss_fftr_cfg cfg;
    kiss_fft_scalar in_signal[SIGNAL_ARRAY_SIZE];
    int in_signal_size;
    kiss_fft_cpx out_spectrum[SPECTRUM_ARRAY_SIZE];
    int out_spectrum_size;
    int cutoff_spectrum_index;

    // variables to eliminate the jumps of the signal
    float jump;
    float last_raw_signal;
    bool have_prev_raw;

    // circular buffer of data after jump elimination
    float large_mean_window_buffer[MAX_LARGE_MEAN_BUFFER_SIZE];
    int large_mean_window_buffer_size;
    int large_mean_window_buffer_input_count;
    int large_mean_window_next_pointer_index;
    float large_mean_window_sum;

    // circular buffer of data after removing large mean line
    float small_mean_window_buffer[MAX_SMALL_MEAN_BUFFER_SIZE];
    int small_mean_window_buffer_size;
    int small_mean_window_buffer_input_count;
    int small_mean_window_next_pointer_index;
    float small_mean_window_sum;

    // circular buffer to store fully processed signal data
    float signal_buffer[SIGNAL_ARRAY_SIZE];
    int signal_buffer_size;
    int signal_buffer_input_count;
    int signal_next_pointer_index;

    // copy signal data to input buffer for kiss fft
    void fillInput();
    // find the frequency of the highest magnitude peak
    float freqMaxPeak();

public:
    /**
     * Initialize the analyzer.
     * 
     * If <window_time> is 0, then <DEFAULT_WINDOW_TIME> is used.
     * 
     * The higher the window time, the higher the accuracy will be,
     * but the slower it will be in noticing changed of the heart rate.
     * freq resolution (bpm) = 60 / <window_time>
     */
    FFTAnalyzer(float sampling_frequency, float window_time);

    /**
     * Return if the analyzer is successfully initialized.
     * 
     * If not successfully initialized, check the constructor call.
     */
    bool isInitialized();

    /**
     * Reset the analyzer without changing the
     * sampling frequency and the window size.
     */
    void reset();

    /**
     * Add new raw sampled data to the analyzer
     */
    void addSignalData(float x);

    /**
     * Return if analyzer is ready to return heart rate
     */
    bool isReady();

    /**
     * Return the current heart rate in bpm (-1 if not ready)
     */
    float getHeartRate();

    /**
     * Free the memory used by KissFFT
     */
    void freeFftMem();
};

#endif