#include "waveform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

// COMMIT 6 - cleaner   //

double get_phase_voltage(WaveformSample *sample, int phase) {
    if (phase == PHASE_A) {
        return sample->phase_A_voltage;
    }
    else if (phase == PHASE_B) {
        return sample->phase_B_voltage;
    }
    else {
        return sample->phase_C_voltage;
    }
}

// RMS function ------ squares every voltage sample, add tem all up and divide by how many there are, then square root it. //

double compute_rms(WaveformSample *samples, int n, int phase) {
    double sum_sq = 0.0;
    double voltage = 0.0;
    int i;

    for (i = 0; i < n; i++) {
        voltage = get_phase_voltage(samples + i, phase);
        sum_sq += voltage * voltage;
    }
    return sqrt(sum_sq / n);
}

// PtP -- finds highest and lowest voltage sample in the whole dataset for that phase then subtracts them //

double compute_peak_to_peak(WaveformSample *samples, int n, int phase) {
    double voltage_max = get_phase_voltage(samples, phase);
    double voltage_min = voltage_max;
    double voltage = 0.0;
    int i;

    for (i = 1; i < n; i++) {
        voltage = get_phase_voltage(samples + i, phase);
        if (voltage > voltage_max) voltage_max = voltage;
        if (voltage < voltage_min) voltage_min = voltage;
    }
    return voltage_max - voltage_min;
}

// DC offset - takes average of all the voltage samples. //

double compute_dc_offset(WaveformSample *samples, int n, int phase) {
    double sum = 0.0;
    int i;

    for (i = 0; i < n; i++) {
        sum += get_phase_voltage(samples + i, phase);
    }
    return sum / n;
}



// clipping - counts samples where absolute voltage hits or exceeds the sensor limit //

int count_clipped(WaveformSample *samples, int n, int phase) {
    int clipped_count = 0;
    double voltage = 0.0;
    int i;

    for (i = 0; i < n; i++) {
        voltage = get_phase_voltage(samples + i, phase);
        if (fabs(voltage) >= CLIP_THRESHOLD) {
            clipped_count++;
        }
    }
    return clipped_count;
}

 // check -- returns 1 if RMS is within EN 50160 tolerance band //

int check_compliance(double rms) {
    if (rms >= TOLERANCE_LOWER && rms <= TOLERANCE_UPPER) {
        return 1;
    }
    return 0;
}

// loop through all samples and average the relevent collumn //

double compute_mean_frequency(WaveformSample *samples, int n) {
    // COMMIT 7 - input validation cmf //
    if (n <= 0) return 0.0;
    double sum = 0.0;
    int i;
    for (i = 0; i < n; i++) {
        sum += (samples + i)->frequency;
    }
    return sum / n;
}

double compute_mean_powerfactor(WaveformSample *samples, int n) {
    // COMMIT 7 - input validation cmp //
    if (n <= 0) return 0.0;
    double sum = 0.0;
    int i;
    for (i = 0; i < n; i++) {
        sum += (samples + i)->power_factor;
    }
    return sum / n;
}

double compute_mean_thd(WaveformSample *samples, int n) {
    // COMMIT 7 - input validation tbh //
    if (n <= 0) return 0.0;
    double sum = 0.0;
    int i;
    for (i = 0; i < n; i++) {
        sum += (samples + i)->thd_percent;
    }
    return sum / n;
}

// runs all calculation fr pahse a,b and c in one loop //

void analyse_all_phases(WaveformSample *samples, int n, PhaseResult *results) {
    int phase;

    for (phase = 0; phase < 3; phase++) {
        if (phase == PHASE_A) strcpy((results + phase)->name, "Phase A");
        if (phase == PHASE_B) strcpy((results + phase)->name, "Phase B");
        if (phase == PHASE_C) strcpy((results + phase)->name, "Phase C");

        (results + phase)->rms           = compute_rms(samples, n, phase);
        (results + phase)->peak_to_peak  = compute_peak_to_peak(samples, n, phase);
        (results + phase)->dc_offset     = compute_dc_offset(samples, n, phase);
        (results + phase)->clipped_count = count_clipped(samples, n, phase);
        (results + phase)->compliant     = check_compliance((results + phase)->rms);
        (results + phase)->std_dev = compute_std_dev(samples, n, phase);
    }
}

// Standard deviation function - measures how much the voltage varies from the average //

double compute_std_dev(WaveformSample *samples, int n, int phase) {
    double mean = compute_dc_offset(samples, n, phase);
    double sum = 0.0;

    for (int i = 0; i < n; i++) {
        double v = get_phase_voltage(samples + i, phase);
        double diff = v - mean;
        sum += diff * diff;
    }

    return sqrt(sum / n);
}