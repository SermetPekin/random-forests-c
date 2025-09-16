#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


void print_options(const Options *opts) {
    printf("Options:\n");
    printf("  n_estimators: %d\n", opts->n_estimators);
    printf("  max_depth: %d\n", opts->max_depth);
    printf("  min_samples_leaf: %d\n", opts->min_samples_leaf);
    printf("  max_features: %s\n", opts->max_features);
    printf("  include_columns: ");
    for (size_t i = 0; i < opts->num_include_columns; i++) {
        printf("%d ", opts->include_columns[i]);
    }
    printf("\n");
    printf("  target_column: %d\n", opts->target_column);
}

static void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r'))
        str[len-1] = '\0';
}


// Helper: parse a single int, possibly negative
static int parse_index(const char *s) {
    return (int)strtol(s, NULL, 10);
}

// Helper: parse a range string (e.g. "0:5" or "-3:-1") and fill indices
static void parse_range(const char *range, int *out, size_t *out_count, size_t max_count) {
    char *colon = strchr(range, ':');
    if (!colon) {
        // Single index
        if (*out_count < max_count) {
            out[(*out_count)++] = parse_index(range);
        }
        return;
    }
    int start = parse_index(range);
    int end = parse_index(colon + 1);
    int step = (start <= end) ? 1 : -1;
    for (int i = start; (step > 0) ? (i <= end) : (i >= end); i += step) {
        if (*out_count < max_count) {
            out[(*out_count)++] = i;
        }
    }
}

int parse_options_file(const char *filename, Options *opts) {
    FILE *f = fopen(filename, "r");
    if (!f) return -1;
    char line[256];
    opts->num_include_columns = 0;
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (line[0] == '#' || line[0] == '\0') continue;
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *val = eq + 1;
        if (strcmp(key, "n_estimators") == 0) opts->n_estimators = atoi(val);
        else if (strcmp(key, "max_depth") == 0) opts->max_depth = atoi(val);
        else if (strcmp(key, "min_samples_leaf") == 0) opts->min_samples_leaf = atoi(val);
        else if (strcmp(key, "max_features") == 0) strncpy(opts->max_features, val, sizeof(opts->max_features)-1);
        else if (strcmp(key, "include_columns") == 0) {
            char *saveptr = NULL;
            char *tok = strtok_r(val, ",", &saveptr);
            while (tok && opts->num_include_columns < 64) {
                while (*tok == ' ' || *tok == '\t') tok++; // skip leading spaces
                parse_range(tok, opts->include_columns, &opts->num_include_columns, 64);
                tok = strtok_r(NULL, ",", &saveptr);
            }
        } else if (strcmp(key, "target_column") == 0) {
            opts->target_column = parse_index(val);
        }
    }
    fclose(f);
    return 0;
}

void resolve_column_indices(Options *opts, int total_cols) {
    // Resolve negative indices for include_columns
    for (size_t i = 0; i < opts->num_include_columns; ++i) {
        int idx = opts->include_columns[i];
        if (idx < 0) idx = total_cols + idx;
        opts->include_columns[i] = idx;
    }
    // Resolve negative index for target_column
    if (opts->target_column < 0) {
        opts->target_column = total_cols + opts->target_column;
    }
}
