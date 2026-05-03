#ifndef IO_H
#define IO_H

#include "waveform.h"

WaveformSample *load_csv(const char *filename, int *row_count);

void write_report(PhaseResult *results, double mean_freq, double mean_pf, double mean_thd, int sample_count);

#endif
