#include "help.h"
#include "data.h"

Result print_help(int argc) // Function to print help menu
{
    if (argc > 2)
    {
        fprintf(stderr, "ERROR : Too many arguments\n");
        fprintf(stderr, "For help : ./a.out -h\n");
        return e_failure;
    }
    printf("---------------------------------------------\n");
    printf("\t\tHelp MENU\n");
    printf("---------------------------------------------\n");
    printf("-v :  Displays mp3 file contents\n");
    printf("-e :  For Editing the mp3 file contents\n");
    printf("-c :  Closes the program\n");
    printf("---------------------------------------------\n");
    printf("\t\tEditing options\n");
    printf("---------------------------------------------\n");
    printf("-t :  To edit Title tag\n");
    printf("-T :  To edit Track tag\n");
    printf("-a :  To edit Artist tag\n");
    printf("-A :  To edit Album tag\n");
    printf("-y :  To edit Year tag\n");
    printf("-c :  To edit Comment tag\n");
    printf("-g :  Modifies a Genre tag\n");
    printf("---------------------------------------------\n");
}