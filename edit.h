#ifndef EDIT_H
#define EDIT_H

#include "data.h"
#include "view.h"
#define TAG_NOT_FOUND 2

typedef struct
{
    char *file_name;
    char *dup_file;
    char edit_option;
    char *content;
    char *tag;

    int version_major;
    int version_revision;
    int size;

    FILE *mp3_fp;
    FILE *dup_fp;
} edit;

Result read_and_validate_edit(int argc, char *argv[], edit *data); // Function to read and validate edit operation arguments
Result operation_edit(edit *data);                                 // Function to perform edit operation
Result set_tag(edit *data);                                        // Function to set which tag to edit
Result read_header(edit *data);                                    // Function to read and validate MP3 header
Result open_files(edit *data);                                     // Function to open original and duplicate files
Result update_file(edit *data);                                    // Function to edit mp3 file content and copy content to a duplicate file
Result update_mp3(edit *data);                                     // Function to update contents in main mp3 file

#endif
