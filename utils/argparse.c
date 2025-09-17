
/*
 * Modifications by Sermet Pekin , 19.09.2025 :
 */ 

#include "argparse.h"

 

 
 void parse_args(int argc, char **argv, struct arguments *arguments) {
    // Defaults
    arguments->rows = 0;
    arguments->cols = 0;
    arguments->log_level = 1;
    arguments->random_seed = 0;
    arguments->args[0] = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], ARG_KEY_ROWS) == 0 && i + 1 < argc) {
            arguments->rows = atol(argv[++i]);
        } else if (strcmp(argv[i], ARG_KEY_COLS) == 0 && i + 1 < argc) {
            arguments->cols = atol(argv[++i]);
        } else if (strncmp(argv[i], ARG_KEY_LOG_LEVEL "=", strlen(ARG_KEY_LOG_LEVEL) + 1) == 0) {
            // Support --log_level=2 style
            arguments->log_level = atoi(argv[i] + strlen(ARG_KEY_LOG_LEVEL) + 1);
        } else if (strcmp(argv[i], ARG_KEY_LOG_LEVEL) == 0 && i + 1 < argc) {
            // Support --log_level 2 style
            arguments->log_level = atoi(argv[++i]);
        } else if (strcmp(argv[i], ARG_KEY_SEED) == 0 && i + 1 < argc) {
            arguments->random_seed = atoi(argv[++i]);
        } else if (arguments->args[0] == NULL) {
            arguments->args[0] = argv[i]; // CSV file
        }
    }
}

