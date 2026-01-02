#include "view.h"

Result read_and_validation_view(int argc, char *argv[], Mp3 *data) // Function to read and validate view operation arguments
{
    if (argc != 3)
    {
        fprintf(stderr, "ERROR : Invalid arguments for view operation\n");
        fprintf(stderr, "For view : ./a.out -v <filename.mp3>\n");
        return e_failure;
    }

    char *ext;
    if ((ext = strstr(argv[2], ".mp3")) != NULL)
    {
        if (strcmp(ext, ".mp3") == 0)
        {
            data->file_name = argv[2];
            return e_success;
        }
        else
        {
            fprintf(stderr, "ERROR : Invalid file extension\n");
            fprintf(stderr, "For view : ./a.out -v <filename.mp3>\n");
            return e_failure;
        }
    }
    else
    {
        fprintf(stderr, "ERROR : Invalid file name\n");
        fprintf(stderr, "For view : ./a.out -v <filename.mp3>\n");
        return e_failure;
    }
}

Result operation_view(Mp3 *data) // Function to perform view operation
{
    printf("INFO : Processing data from mp3 file\n");

    printf("INFO : Opening the %s file\n", data->file_name);
    if (open_file(data) == e_failure)
    {
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Checking the header of the %s file\n", data->file_name);
    if (header_data(data) == e_failure)
    {
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Storing the data from the %s file\n", data->file_name);
    if (store_data(data) == e_failure)
    {
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Printing the data from the %s file\n", data->file_name);
    printf("\n\n");

    if (print_view(data) == e_failure)
    {
        printf("ERROR : Failed to print mp3 file details\n");
        return e_failure;
    }

    free(data->title);
    free(data->artist);
    free(data->album);
    free(data->year);
    free(data->track);
    free(data->genre);
    free(data->comment);

    fclose(data->mp3_fp);

    return e_success;
}

Result open_file(Mp3 *file_data) // Function to open mp3 file
{
    if ((file_data->mp3_fp = fopen(file_data->file_name, "r")) == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR : Failed to open %s file\n", file_data->file_name);

        return e_failure;
    }
    return e_success;
}

Result header_data(Mp3 *data) // Function to read and validate MP3 header
{
    char buff[10];
    if (fread(buff, 10, 1, data->mp3_fp) != 1) // store 10 bytes in buff from mp3 file
    {
        printf("ERROR : Failed to read mp3 header data\n");
        return e_failure;
    }

    char str[4]; // store the mp3 standard id3v2
    strncpy(str, buff, 3);
    str[3] = '\0';

    if (strcmp(str, "ID3") != 0)
    {
        printf("Error: No,ID3 tag found in the file.\n");
        printf("The file is not a valid MP3\n");
        return e_failure;
    }

    data->version_major = buff[3]; // version
    data->version_revision = buff[4];

    data->size = (buff[6] & 0x7F) << 21 | (buff[7] & 0x7F) << 14 | (buff[8] & 0x7F) << 7 | (buff[9] & 0x7F); // size of metadata in big-endian
    printf("INFO : The total metadata size of this mp3 file is %d\n", data->size);
    return e_success;
}

Result store_data(Mp3 *data) // Function to store mp3 tag data
{

    while (ftell(data->mp3_fp) < data->size) // loop until the file pointer reaches the size of metadata
    {
        char buf[5];
        if (fread(buf, 4, 1, data->mp3_fp) != 1)
        {
            printf("ERROR : Failed to read mp3 data\n");
            return e_failure;
        }
        buf[4] = '\0';

        char *ext[8] = {"TIT2", "TRCK", "TPE1", "TALB", "TYER", "TCON", "COMM", "TDRC"};

        int index = -1;
        for (int j = 0; j < 8; j++) // check if the tag is one of the required tags
        {
            if (strcmp(buf, ext[j]) == 0)
            {
                index = j;
                break;
            }
        }

        unsigned char buff[4];
        if (fread(buff, 4, 1, data->mp3_fp) != 1) // size of the tag content
        {
            printf("ERROR : Failed to read mp3 data\n");
            return e_failure;
        }
        int size = buff[0] << 24 | buff[1] << 16 | buff[2] << 8 | buff[3]; // convert the size from big-endian to integer

        fseek(data->mp3_fp, 2, SEEK_CUR); // skip 2 flag

        char encode; // skip encoding flag
        if (fread(&encode, 1, 1, data->mp3_fp) != 1)
        {
            printf("ERROR : Failed to read mp3 data\n");
            return e_failure;
        }
        if (encode != 0x00 && encode != 0x01) // no encoding flag
            fseek(data->mp3_fp, -1, SEEK_CUR);
        else
            size = size - 1;

        if (index == -1) // if the tag is not one of the required tags, skip the content
        {
            fseek(data->mp3_fp, size, SEEK_CUR);
        }
        else
        {
            char str[size + 1]; // store the tag content
            if (fread(str, size, 1, data->mp3_fp) != 1)
            {
                printf("ERROR : Failed to read mp3 data\n");
                return e_failure;
            }

            for (int i = 0; i < size; i++)
            {
                if (str[i] == '\0' || !isprint((unsigned char)str[i])) // replace non-printable characters with '-'
                {
                    str[i] = '-';
                }
            }
            str[size] = '\0';

            switch (index) // store the tag content in the corresponding variable
            {
            case 0:
            {
                data->title = malloc(size);
                if (data->title == NULL)
                {
                    perror("malloc");
                    return e_failure;
                }
                strcpy(data->title, str);
            }
            break;
            case 1:
            {
                data->track = malloc(size);
                if (data->track == NULL)
                {
                    perror("malloc");
                    return e_failure;
                }
                strcpy(data->track, str);
            }
            break;
            case 2:
            {
                data->artist = malloc(size);
                if (data->artist == NULL)
                {
                    perror("malloc");
                    return e_failure;
                }
                strcpy(data->artist, str);
            }
            break;
            case 3:
            {
                data->album = malloc(size);
                if (data->album == NULL)
                {
                    perror("malloc");
                    return e_failure;
                }
                strcpy(data->album, str);
            }
            break;
            case 4:
            {
                data->year = malloc(size);
                if (data->year == NULL)
                {
                    perror("malloc");
                    return e_failure;
                }
                strcpy(data->year, str);
            }
            break;
            case 5:
            {
                data->genre = malloc(size);
                if (data->genre == NULL)
                {
                    perror("malloc");
                    return e_failure;
                }
                strcpy(data->genre, str);
            }
            break;
            case 6:
            {
                data->comment = malloc(size);
                if (data->comment == NULL)
                {
                    perror("malloc");
                    return e_failure;
                }
                strcpy(data->comment, str);
            }
            break;
            case 7:
            {
                data->year = malloc(size);
                if (data->year == NULL)
                {
                    perror("malloc");
                    return e_failure;
                }
                strcpy(data->year, str);
            }
            break;
            }
        }
    }
}

Result print_view(Mp3 *data) // Function to print mp3 tag data
{
    printf("---------------------------------------------------------------------------------------------------\n");
    printf("\t\t\t MP3 TAG READER FOR IDv2.%d.%d\n", data->version_major, data->version_revision);
    printf("---------------------------------------------------------------------------------------------------\n");
    printf("TITLE : %s\n", data->title);
    printf("ARTIST : %s\n", data->artist);
    printf("ALBUM : %s\n", data->album);
    printf("YEAR : %s\n", data->year);
    printf("TRACK : %s\n", data->track);
    printf("GENRE : %s\n", data->genre);
    printf("COMMENT : %s\n", data->comment);
    printf("---------------------------------------------------------------------------------------------------\n");
    printf("\n");

    printf("---------------------------------------------------------------------------------------------------\n");
    printf("\t\t\t DETAILS DISPLAYED SUCCESSFULLY\n");
    printf("---------------------------------------------------------------------------------------------------\n");
    printf("\n");

    return e_success;
}
