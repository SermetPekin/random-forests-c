#pragma once
#include <stddef.h>

// Structure to hold options parsed from options.txt

typedef struct Options {
    int n_estimators;
    int max_depth;
    int min_samples_leaf;
    char max_features[32];
    int include_columns[64]; // max 64 columns
    size_t num_include_columns;
    int target_column; // can be negative, resolve at runtime
} Options;

// Expands include_columns and target_column negative indices using total_cols.
// Call this after parsing options and after knowing the number of columns in the data.
void resolve_column_indices(Options *opts, int total_cols);

void print_options(const Options *opts);

// Parses the options file and fills the Options struct. Returns 0 on success, -1 on error.
int parse_options_file(const char *filename, Options *opts);
