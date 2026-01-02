#ifndef VIEW_H
#define VIEW_H

#include "data.h"

typedef struct
{
    char *file_name;
    char *title;
    char *artist;
    char *album;
    char *year;
    char *track;
    char *genre;
    char *comment;

    int version_major;
    int version_revision;
    int size;

    FILE *mp3_fp;
} Mp3;

Result read_and_validation_view(int argc, char *argv[], Mp3 *data); // Function to read and validate view operation arguments
Result operation_view(Mp3 *data);                                   // Function to perform view operation
Result open_file(Mp3 *data);                                        // Function to open mp3 file
Result header_data(Mp3 *data);                                      // Function to read and validate MP3 header
Result store_data(Mp3 *data);                                       // Function to store mp3 tag data
Result print_view(Mp3 *data);                                       // Function to print mp3 tag data

#endif
