/*
 * Modifications by Sermet Pekin , 19.09.2025 :
 * - Improved CSV parsing to handle long lines.
 * - Added debug information for CSV parsing.
 *
 * Original code copyright (c) andrii dobroshynski /2023
 * Licensed under the Apache License, Version 2.0
 */


#include "data.h"

// Set to 1 if your CSV has a header row, 0 otherwise
#define CSV_HAS_HEADER 1

// Debug macro for easier debug output
#define DEBUG_PRINT(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

int DEBUG = 0;


// Returns the buffer size for reading lines
int get_buf_size()
{
    return 8192;
    // return BUFSIZ;
}

// Parses the CSV file to determine its dimensions (rows and columns)
struct dim parse_csv_dims(const char *file_name)
{
    FILE *csv_file;
    csv_file = fopen(file_name, "r");
    int buf_size = get_buf_size();

    if (csv_file == NULL)
    {
        printf("Error: can't open file: %s\n", file_name);
        exit(-1);
    }

    const char *delimiter = ",";


    char *buffer = (char *)malloc(buf_size); // BUFSIZ
    if (!buffer) {
        printf("Error: failed to allocate buffer of size %d\n", buf_size);
        fclose(csv_file);
        exit(-1);
    }
    char *token;

    // Keeping track of how many rows and columns there are.
    int rows = 0;
    int cols = 0;

    // Reach each line of the file into the buffer.

    while (fgets(buffer, buf_size, csv_file) != NULL)
    {
        ++rows;

        // We use a second counter for columns in order to count the number of columns
        // for each row we read and will trigger an assert if there is a mismatch in the
        // size of the cols, i.e. not all rows have the same number of columns which would
        // cause undefined behavior.
        int curr_cols = 0;

        
        token = strtok(buffer, delimiter);
        while (token != NULL)
        {
            ++curr_cols;
            DEBUG_PRINT( " R: %d Col: %zu %s\n", rows, curr_cols, token );

            token = strtok(NULL, delimiter);
        }
        if (cols == 0)
        {
            cols = curr_cols;
        }
        else
        {
            assert(curr_cols == cols && "Error: every row must have the same amount of columns");
        }
    }

    // Adjust for header row if present
    if (CSV_HAS_HEADER) {
        --rows;
    }


    if (fclose(csv_file) != 0) {
        printf("Warning: error closing file %s\n", file_name);
    }
    free(buffer);

    // Make sure that the dimensions are valid.

    assert(rows > 0 && "# of rows in csv must be > 0");
    assert(cols > 0 && "# of cols in csv must be > 0");


    return (struct dim){rows : rows, cols : cols};
}

// Parses the CSV file and fills the data array
void parse_csv(const char *file_name, double **data_p, const struct dim csv_dim)
{

    FILE *csv_file;
    const char *delimiter = ",";
    int buf_size;
    char *buffer;
    char *token;
    int row = 0;
    int idx = 0;

    csv_file = fopen(file_name, "r");
    if (csv_file == NULL) {
        printf("Error: can't open file: %s\n", file_name);
        exit(-1);
    }

    buf_size = get_buf_size();
    buffer = (char *)malloc(buf_size);
    if (!buffer) {
        printf("Error: failed to allocate buffer of size %d\n", buf_size);
        fclose(csv_file);
        exit(-1);
    }

    while (row <= csv_dim.rows && fgets(buffer, buf_size, csv_file) != NULL) {
        // Skip header row if present
        if (CSV_HAS_HEADER && ++row == 1) continue;
        if (!CSV_HAS_HEADER) ++row;

        token = strtok(buffer, delimiter);
        while (token != NULL) {
            DEBUG_PRINT("Parsing row %d col %d: %s\n", row, (idx % csv_dim.cols) + 1, token);
            (*data_p)[idx] = atof(token);
            ++idx;
            token = strtok(NULL, delimiter);
        }
    }

    if (log_level > 1)
        printf("read %d rows from file %s\n", row - (CSV_HAS_HEADER ? 1 : 0), file_name);

    if (fclose(csv_file) != 0) {
        printf("Warning: error closing file %s\n", file_name);
    }
    free(buffer);
}

 

// Pivots and transforms the data array into a 2D array
void pivot_data(const double *data, const struct dim csv_dim, double ***pivoted_data_p)
{
    (*pivoted_data_p) = _2d_calloc(csv_dim.rows, csv_dim.cols);

    for (size_t i = 0; i < csv_dim.rows; ++i)
        for (size_t j = 0; j < csv_dim.cols; ++j)
            (*pivoted_data_p)[i][j] = data[(i * csv_dim.cols) + j];
}
