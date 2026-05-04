

#ifndef WAVEFORM_H
#define WAVEFORM_H


// EN 50160 compliance limits //
#define NOMINAL_VOLTAGE  230.0
#define TOLERANCE_UPPER  253.0
#define TOLERANCE_LOWER  207.0
#define CLIP_THRESHOLD   324.9

// pase index constants //
#define PHASE_A  0
#define PHASE_B  1
#define PHASE_C  2

//waveformsample holds all 8 values from a row of the CSV. 1000 rows total //

typedef struct {
    double timestamp;
    double phase_A_voltage;
    double phase_B_voltage;
    double phase_C_voltage;
    double line_current;
    double frequency;
    double power_factor;
    double thd_percent;
} WaveformSample;

// stores results for each calculation //

typedef struct {
    char   name[10];
    double rms;
    double peak_to_peak;
    double dc_offset;
    int    clipped_count;
    int    compliant;
    double std_dev;
} PhaseResult;

// function prototypes //
double get_phase_voltage(WaveformSample *sample, int phase);
double compute_rms(WaveformSample *samples, int n, int phase);
double compute_peak_to_peak(WaveformSample *samples, int n,  int phase);
double compute_dc_offset(WaveformSample *samples, int n, int phase);
int count_clipped(WaveformSample *samples, int n, int phase);
int check_compliance(double rms);
double compute_mean_frequency(WaveformSample *samples, int n);
double compute_mean_powerfactor(WaveformSample *samples, int n);
double compute_mean_thd(WaveformSample *samples, int n);
double compute_std_dev(WaveformSample *samples, int n, int phase);
void analyse_all_phases(WaveformSample *samples, int n, PhaseResult *results);

#endif