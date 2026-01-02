#include "data.h"
#include "edit.h"

Result read_and_validate_edit(int argc, char *argv[], edit *data) // Function to read and validate edit operation arguments
{
    if (argc != 5)
    {
        fprintf(stderr, "ERROR : Invalid arguments for edit operation\n");
        fprintf(stderr, "Usage: ./a.out -e <-t/-T/-a/-A/-g/-y/-c> <new_content> <filename.mp3>\n");
        return e_failure;
    }

    char *valid_opts[7] = {"-t", "-T", "-a", "-A", "-g", "-y", "-c"};
    int valid = 0;
    for (int i = 0; i < 7; i++) // Check if the edit option is valid
    {
        if (strcmp(argv[2], valid_opts[i]) == 0)
        {
            data->edit_option = argv[2][1];
            valid = 1;
            break;
        }
    }

    if (!valid)
    {
        fprintf(stderr, "ERROR : Invalid Operation type for edit\n");
        fprintf(stderr, "Usage: ./a.out -e <-t/-T/-a/-A/-g/-y/-c> <new_content> <filename.mp3>\n");
        return e_failure;
    }

    data->content = argv[3]; // Assign new content

    char *ext = strstr(argv[4], ".mp3"); // Check if the file extension is .mp3
    if (!ext || strcmp(ext, ".mp3") != 0)
    {
        fprintf(stderr, "ERROR : Invalid file extension\n");
        fprintf(stderr, "Usage: ./a.out -e <-t/-T/-a/-A/-g/-y/-c> <new_content> <filename.mp3>\n");
        return e_failure;
    }

    data->file_name = argv[4]; // Assign file name
    data->dup_file = "dup_file.mp3";

    return e_success;
}

Result operation_edit(edit *data) // Function to perform edit operation
{
    printf("INFO : Opening the %s file\n", data->file_name);
    if (open_files(data) == e_failure)
    {
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Reading the header data from %s\n", data->file_name);
    if (read_header(data) == e_failure)
    {
        fclose(data->mp3_fp);
        fclose(data->dup_fp);
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Setting the tag based on edit option\n");
    if (set_tag(data) == e_failure)
    {
        fclose(data->mp3_fp);
        fclose(data->dup_fp);
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Updating the tag data in %s\n", data->file_name);
    if (update_file(data) == e_failure)
    {
        fclose(data->mp3_fp);
        fclose(data->dup_fp);
        fprintf(stderr, "ERROR : Updating tag failed\n");
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Writing changes back to original file\n");
    if (update_mp3(data) == e_failure)
    {
        fclose(data->mp3_fp);
        fclose(data->dup_fp);
        fprintf(stderr, "ERROR : Writing back to original file failed\n");
        return e_failure;
    }
    printf("INFO : Done\n");

    fclose(data->mp3_fp);
    fclose(data->dup_fp);
    remove(data->dup_file); // Remove duplicate file

    return e_success;
}

Result open_files(edit *data) // Function to open original and duplicate files
{
    data->mp3_fp = fopen(data->file_name, "r+");
    if (!data->mp3_fp)
    {
        perror("fopen");
        fprintf(stderr, "ERROR : Failed to open %s\n", data->file_name);
        return e_failure;
    }

    data->dup_fp = fopen(data->dup_file, "w+");
    if (!data->dup_fp)
    {
        perror("fopen");
        fprintf(stderr, "ERROR : Failed to open duplicate file\n");
        fclose(data->mp3_fp);
        return e_failure;
    }
    return e_success;
}

Result read_header(edit *data) // Function to read and validate MP3 header
{
    char buff[10];
    if (fread(buff, 1, 10, data->mp3_fp) != 10) // Read first 10 bytes
    {
        fprintf(stderr, "ERROR : Failed to read MP3 header\n");
        return e_failure;
    }

    if (strncmp(buff, "ID3", 3) != 0) // Validate ID3 tag
    {
        fprintf(stderr, "ERROR : No ID3 tag found\n");
        return e_failure;
    }

    data->version_major = buff[3];
    data->version_revision = buff[4];

    data->size = (buff[6] & 0x7F) << 21 | (buff[7] & 0x7F) << 14 | (buff[8] & 0x7F) << 7 | (buff[9] & 0x7F);

    if (fwrite(buff, 1, 10, data->dup_fp) != 10)
    {
        fprintf(stderr, "ERROR : Failed to write MP3 header to duplicate\n");
        return e_failure;
    }

    if (ftell(data->mp3_fp) != ftell(data->dup_fp))
    {
        fprintf(stderr, "ERROR : File position mismatch after header copy\n");
        return e_failure;
    }
    return e_success;
}

Result set_tag(edit *data) // Function to set which tag to edit
{
    switch (data->edit_option)
    {
    case 't':
        data->tag = "TIT2";
        break;
    case 'T':
        data->tag = "TRCK";
        break;
    case 'a':
        data->tag = "TPE1";
        break;
    case 'A':
        data->tag = "TALB";
        break;
    case 'y':
        data->tag = "TYER";
        break;
    case 'c':
        data->tag = "COMM";
        break;
    case 'g':
        data->tag = "TCON";
        break;
    default:
        fprintf(stderr, "ERROR : Invalid edit option\n");
        return e_failure;
    }
    return e_success;
}

Result update_file(edit *data) // Function to edit mp3 file content and copy content to a duplicate file
{
    char frame_id[5] = {0};
    int found_tag = 0;

    long tag_end_offset = 10 + data->size; // Tag header (10) + tag size

    while (ftell(data->mp3_fp) < tag_end_offset)
    {
        if (fread(frame_id, 1, 4, data->mp3_fp) != 4)
            break;
        if (frame_id[0] == 0)
            break; // padding or end of frames

        unsigned char size_bytes[4];
        if (fread(size_bytes, 1, 4, data->mp3_fp) != 4)
            return e_failure;

        int frame_size = size_bytes[0] << 24 | size_bytes[1] << 16 | size_bytes[2] << 8 | size_bytes[3]; // frame size(non-syncsafe)
        if (frame_size <= 0)                                                                             // Invalid frame size
            break;

        unsigned char flags[2];
        if (fread(flags, 1, 2, data->mp3_fp) != 2) // Read flags
            return e_failure;

        char *content = malloc(frame_size); // Allocate memory for frame content
        if (!content)
            return e_failure;
        if (fread(content, 1, frame_size, data->mp3_fp) != frame_size)
        {
            free(content);
            return e_failure;
        }

        if (strncmp(frame_id, data->tag, 4) == 0) // check both tag name are same
        {
            found_tag = 1;

            // Prepare new content: encoding byte + string
            int new_content_len = strlen(data->content) + 1;
            unsigned char new_size_bytes[4] = {(new_content_len >> 24) & 0xFF, (new_content_len >> 16) & 0xFF, (new_content_len >> 8) & 0xFF, new_content_len & 0xFF};

            // Write updated frame header + flags
            fwrite(frame_id, 1, 4, data->dup_fp);
            fwrite(new_size_bytes, 1, 4, data->dup_fp);
            fwrite(flags, 1, 2, data->dup_fp);

            // Write encoding byte (0x00) and new content string
            char encoding = 0x00;
            fwrite(&encoding, 1, 1, data->dup_fp);
            fwrite(data->content, 1, strlen(data->content), data->dup_fp);
        }
        else
        {
            // Write original frame header, size, flags
            fwrite(frame_id, 1, 4, data->dup_fp);
            fwrite(size_bytes, 1, 4, data->dup_fp);
            fwrite(flags, 1, 2, data->dup_fp);
            // Write original frame content
            fwrite(content, 1, frame_size, data->dup_fp);
        }
        free(content);
    }

    // Copy remaining tag/file data (after tag frames) to duplicate intact
    char buffer[1024];
    size_t read_size;
    while ((read_size = fread(buffer, 1, sizeof(buffer), data->mp3_fp)) > 0)
    {
        if (fwrite(buffer, 1, read_size, data->dup_fp) != read_size)
            return e_failure;
    }

    if (!found_tag)
    {
        fprintf(stderr, "ERROR : Tag not found\n");
        return e_failure;
    }

    return e_success;
}

Result update_mp3(edit *data) // Function to update contents in main mp3 file
{
    fseek(data->mp3_fp, 0, SEEK_SET);
    fseek(data->dup_fp, 0, SEEK_SET);

    char ch;
    while (fread(&ch, 1, 1, data->dup_fp) == 1)
    {
        if (fwrite(&ch, 1, 1, data->mp3_fp) != 1)
            return e_failure;
    }
    return e_success;
}
