#include <stdio.h>
#include <stdlib.h>
#include "waveform.h"
#include "io.h"



// checks a file name was provided, loads teh csv and runs the analysis //

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("usage: %s <csv_filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    printf("============================================================\n");
    printf(" Power Quality Waveform Analyser\n");
    printf(" UGMFGT-15-1 Programming for Engineers\n");
    printf("============================================================\n\n");

    int sample_count = 0;
    WaveformSample *samples = load_csv(filename, &sample_count);

    if (samples == NULL) {
        return 1;
    }

    PhaseResult results[3];

    printf("analysing waveforms...\n");
    analyse_all_phases(samples, sample_count, results);

    double mean_freq = compute_mean_frequency(samples, sample_count);
    double mean_pf   = compute_mean_powerfactor(samples, sample_count);
    double mean_thd  = compute_mean_thd(samples, sample_count);

    printf("\n--- results ---\n\n");

    int phase;
    for (phase = 0; phase < 3; phase++) {
        PhaseResult *r = results + phase;
        printf("%s:\n", r->name);
        printf("  RMS             = %.4f V  (%s)\n",
               r->rms, r->compliant ? "COMPLIANT" : "OUT OF TOLERANCE");
        printf("  Peak-to-Peak    = %.4f V\n", r->peak_to_peak);
        printf("  DC Offset       = %.6f V\n", r->dc_offset);
        printf("  Clipped Samples = %d\n\n", r->clipped_count);
    }
// free memory alocated with malloc //
    printf("Mean Frequency   : %.4f Hz\n", mean_freq);
    printf("Mean Power Factor: %.4f\n", mean_pf);
    printf("Mean THD         : %.4f %%\n\n", mean_thd);

    write_report(results, mean_freq, mean_pf, mean_thd, sample_count);

    free(samples);
    samples = NULL;

    printf("\ndone.\n");
    return 0;
}