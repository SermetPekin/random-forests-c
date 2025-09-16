/*
@author andrii dobroshynski
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "eval/eval.h"
#include "utils/argparse.h"
#include "utils/data.h"
#include "utils/utils.h"
#include "utils/options.h"

int main(int argc, char **argv)
{

    struct arguments arguments;
    parse_args(argc, argv, &arguments);

    set_log_level(arguments.log_level);

    // Load options from options.txt
    Options opts;

    if (parse_options_file("options.txt", &opts) != 0)
    {
        printf("Failed to read options.txt.\n");
        return 1;
    }



    // Read the csv file from args which must be parsed now.
    const char *file_name = arguments.args[0];
    if (!file_name)
    {
        printf("Usage: %s <CSV_FILE> [--num_rows N] [--num_cols N] [--log_level N] [--seed N]\n", argv[0]);
        return 1;
    }

    // If the values for rows and cols were provided as arguments, then use them for the
    // 'dim' struct, otherwise call 'parse_csv_dims()' to parse the csv file provided to
    // compute the size of the csv file.
    dim csv_dim;
    if (arguments.rows && arguments.cols)
        csv_dim = (dim){.rows = arguments.rows, .cols = arguments.cols};
    else
        csv_dim = parse_csv_dims(file_name);

    // Resolve negative indices in options now that we know total columns
    resolve_column_indices(&opts, csv_dim.cols);
    print_options(&opts);

    // Optionally set the random seed if a specific random seed was provided via an argument.
    if (arguments.random_seed)
        srand(arguments.random_seed);
    else
        srand((unsigned int)time(NULL));



    if (log_level > 0)
        printf("using:\n  verbose log level: %d\n  rows: %ld, cols: %ld\nreading from csv file:\n  \"%s\"\n",
               log_level,
               csv_dim.rows,
               csv_dim.cols,
               file_name);

    // Allocate memory for the data coming from the .csv and read in the data.
    double *data = malloc(sizeof(double) * csv_dim.rows * csv_dim.cols);
    parse_csv(file_name, &data, csv_dim);

    // Compute a checksum of the data to verify that loaded correctly.
    if (log_level > 1)
        printf("data checksum = %f\n", _1d_checksum(data, csv_dim.rows * csv_dim.cols));

    const int k_folds = 5;

    if (log_level > 0)
        printf("using:\n  k_folds: %d\n", k_folds);

    // Use options for random forest parameters
    const RandomForestParameters params = {
        .n_estimators = opts.n_estimators,
        .max_depth = opts.max_depth,
        .min_samples_leaf = opts.min_samples_leaf,
        .max_features = (strcmp(opts.max_features, "auto") == 0) ? opts.num_include_columns : atoi(opts.max_features)
    };
    if (log_level > 0)
        print_params(&params);

    // Pivot the csv file data into a two dimensional array.
    double **pivoted_data;
    pivot_data(data, csv_dim, &pivoted_data);

    if (log_level > 1)
        printf("checksum of pivoted 2d array: %f\n", _2d_checksum(pivoted_data, csv_dim.rows, csv_dim.cols));

    // Start the clock for timing.
    clock_t begin_clock = clock();

    // TODO: Use opts.include_columns and opts.target_column in your data/model pipeline
    // For now, just print them (already done in print_options)

    double cv_accuracy = cross_validate(pivoted_data, &params, &csv_dim, k_folds);
    printf("cross validation accuracy: %f%% (%ld%%)\n",
           (cv_accuracy * 100),
           (long)(cv_accuracy * 100));

    // Record and output the time taken to run.
    clock_t end_clock = clock();
    printf("(time taken: %fs)\n", (double)(end_clock - begin_clock) / CLOCKS_PER_SEC);

    // Free loaded csv file data.
    free(data);
    free(pivoted_data);
}
