#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256


//opens the file and counts how many rows are in it //

static int count_rows(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return -1;

    int count = 0;
    char line[MAX_LINE_LENGTH];

    fgets(line, MAX_LINE_LENGTH, fp);

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        count++;
    }
    fclose(fp);
    return count;
}

// loads the csv file (power_quality_log.csv), allocates memory for all the rows, reads each line into a struct //

WaveformSample *load_csv(const char *filename, int *row_count) {
    int n = count_rows(filename);
    if (n <= 0) {
        printf("ERROR: could not open file '%s' or file is empty.\n", filename);
        *row_count = 0;
        return NULL;
    }

    WaveformSample *samples = (WaveformSample *)malloc(n * sizeof(WaveformSample));
    if (samples == NULL) {
        printf("ERROR: memory allocation failed.\n");
        *row_count = 0;
        return NULL;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("ERROR: could not reopen file '%s'.\n", filename);
        free(samples);
        *row_count = 0;
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    fgets(line, MAX_LINE_LENGTH, fp);

    int i = 0;
    while (i < n && fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        WaveformSample *current = samples + i;

        int fields_read = sscanf(line,
                                 "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
                                 &current->timestamp,
                                 &current->phase_A_voltage,
                                 &current->phase_B_voltage,
                                 &current->phase_C_voltage,
                                 &current->line_current,   // FIXED
                                 &current->frequency,
                                 &current->power_factor,
                                 &current->thd_percent);

        if (fields_read == 8) {
            i++;
        }
    }
    fclose(fp);
    *row_count = i;
    printf("loaded %d samples from '%s'.\n", i, filename);
    return samples;
}

//writes all the coputed results out to results.txt //

void write_report(PhaseResult *results, double mean_freq, double mean_pf, double mean_thd, int sample_count) {
    FILE *fp = fopen("results.txt", "w");
    if (fp == NULL) {
        printf("ERROR: could not create results.txt\n");
        return;
    }

    fprintf(fp, "============================================================\n");
    fprintf(fp, " POWER QUALITY WAVEFORM ANALYSER - RESULTS\n");
    fprintf(fp, " Module: UGMFGT-15-1 Programming for Engineers\n");
    fprintf(fp, "============================================================\n\n");
    fprintf(fp, "Samples analysed : %d\n", sample_count);
    fprintf(fp, "Nominal voltage  : %.1f V  (EN 50160: %.0f - %.0f V)\n\n",
            NOMINAL_VOLTAGE, TOLERANCE_LOWER, TOLERANCE_UPPER);

    int phase;
    for (phase = 0; phase < 3; phase++) {
        PhaseResult *r = results + phase;
        fprintf(fp, "------------------------------------------------------------\n");
        fprintf(fp, " %s\n", r->name);
        fprintf(fp, "------------------------------------------------------------\n");
        fprintf(fp, "  RMS Voltage      : %.4f V\n", r->rms);
        fprintf(fp, "  Peak-to-Peak     : %.4f V\n", r->peak_to_peak);
        fprintf(fp, "  DC Offset        : %.6f V\n", r->dc_offset);
        fprintf(fp, "  Clipped Samples  : %d\n", r->clipped_count);
        fprintf(fp, "  EN 50160 Status  : %s\n\n",
                r->compliant ? "COMPLIANT" : "OUT OF TOLERANCE");
    }

    fprintf(fp, "------------------------------------------------------------\n");
    fprintf(fp, " SYSTEM METRICS\n");
    fprintf(fp, "------------------------------------------------------------\n");
    fprintf(fp, "  Mean Frequency   : %.4f Hz\n", mean_freq);
    fprintf(fp, "  Mean Power Factor: %.4f\n", mean_pf);
    fprintf(fp, "  Mean THD         : %.4f %%\n", mean_thd);
    fprintf(fp, "\n============================================================\n");
    fprintf(fp, " END OF REPORT\n");
    fprintf(fp, "============================================================\n");

    fclose(fp);
    printf("report written to results.txt\n");
}