/**************************************************************************************************************************************************************
       NAME    : ANURAJ R
       ROLL NO : 25005_010
       BATCH   : 25005B
       PROJECT : MP3 TAG READER AND EDITOR
 ***************************************************************************************************************************************************************/
#include "edit.h"
#include "help.h"
#include "view.h"
#include "data.h"

Mode operation_type(char *argv[]);

int main(int argc, char *argv[])
{
    if (argc < 2) // Check for minimum arguments
    {
        fprintf(stderr, "Error : ./a.out : INVALID ARGUMENTS\n");
        fprintf(stderr, "USAGE :\nFor help : ./a.out -help\n");
        fprintf(stderr, "To exit : ./a.out -c\n");
        fprintf(stderr, "To view : ./a.out -v <filename.mp3>\n");
        fprintf(stderr, "To edit : ./a.out -e  <-t/-T/-a/-A/-g/-y/-c> <new_content> <filename.mp3>\n");
        return 1;
    }

    switch (operation_type(argv)) // Determine operation type
    {
    case Help:
    {
        print_help(argc);
    }
    break;

    case View:
    {
        Mp3 file_data = {NULL};
        if (read_and_validation_view(argc, argv, &file_data) == e_success)
        {
            if (operation_view(&file_data) == e_failure)
            {
                printf("ERROR : Failed to display mp3 file details\n");
            }
        }
    }
    break;

    case Edit:
    {
        edit data = {NULL};
        if (read_and_validate_edit(argc, argv, &data) == e_success)
        {
            if (operation_edit(&data) == e_failure)
            {
                printf("ERROR : Failed to edit mp3 file details\n");
            }
        }
    }
    break;

    case Close:
    {
        printf("INFO : Closing the program\n");
        return 0;
    }
    break;

    default:
        printf("Error :Invalid Operation.\nINFO : Please use -h for Help, -v for View, -e for editing and -c for closing the program\n"); // Handle unsupported operations
    }
}

Mode operation_type(char *argv[]) // function to determine operation type
{
    if (strcmp(argv[1], "-h") == 0)
    {
        return Help;
    }
    else if (strcmp(argv[1], "-v") == 0)
    {
        return View;
    }
    else if (strcmp(argv[1], "-e") == 0)
    {
        return Edit;
    }
    else if (strcmp(argv[1], "-c") == 0)
    {
        return Close;
    }
    else
    {
        return Invalid;
    }
}