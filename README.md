# MP3 Tag Reader and Editor

## Overview

This is a command-line application written in C that allows users to view and edit the metadata (ID3v2 tags) of MP3 files.
It provides a simple interface to read existing tags like title, artist, album, etc., and to modify them directly from the terminal.

---

## Features

- View Tags : Read and display the primary metadata tags from an MP3 file.
- Edit Tags : Modify specific tags (Title, Artist, Album, Year, Track, Genre, Comment) with new content.
- Command-Line Interface : All operations are performed via command-line arguments.
- Error Handling : Includes validation for command-line arguments, file extensions, and file operations.

---

## How to Compile and Run

### Compilation

You can compile the project using a C compiler like GCC. Navigate to the project directory and run:

```sh
gcc main.c view.c edit.c help.c -o mp3_tag_editor
```

### Usage

All commands are run from the terminal.

**1. View MP3 Tags**

To display the metadata of an MP3 file:

```sh
./mp3_tag_editor -v <filename.mp3>
```
*Example:*
```sh
./mp3_tag_editor -v "my_song.mp3"
```

**2. Edit an MP3 Tag**

To modify a specific tag in an MP3 file. It's recommended to put the new content in quotes if it contains spaces.

```sh
./mp3_tag_editor -e <option> "<new_content>" <filename.mp3>
```

Edit Options:
- "-t" : Edit Title
- "-T" : Edit Track
- "-a" : Edit Artist
- "-A" : Edit Album
- "-y" : Edit Year
- "-g" : Edit Genre
- "-c" : Edit Comment

*Example (editing the artist):*
```sh
./mp3_tag_editor -e -a "New Artist Name" "my_song.mp3"
```

**3. Display Help Menu**

To see the help menu with all available commands and options:

```sh
./mp3_tag_editor -help
```

---

## File Structure

- `main.c` : The entry point of the application. It parses command-line arguments and directs the flow to the appropriate module (view, edit, or help).
- `view.h` / `view.c` : Contains all the logic for the "view" functionality. This includes reading and validating arguments, opening the MP3 file, parsing the ID3v2 header and frames, and printing the tag information to the console.
- `edit.h` / `edit.c` : Implements the "edit" functionality. It handles argument validation, creates a temporary file, copies the MP3 data while replacing the specified tag with new content, and then overwrites the original file with the updated data.
- `help.h` / `help.c` : A simple module to print the help and usage information.
- `data.h` : A common header file that defines shared data types, such as enumerations for operation modes (`Mode`) and success/failure results ('Result').

---

## Implementation Notes

- The application is designed to work with MP3 files that use the **ID3v2** tag format.
- The edit operation is performed safely by first writing all changes to a temporary file ('dup_file.mp3'). If the operation is successful, the original file is replaced, and the temporary file is deleted.
- The program handles basic errors like invalid arguments, incorrect file types, and file-not-found issues, providing feedback to the user via 'stderr'.

## Limitations
- Focused on common ID3v2 frames; not a full-featured tag library.
- Limited encoding and error recovery; unexpected or exotic frames may be skipped.
- No automatic backup created; edits overwrite the original file after processing.

## Contributing
Feel free to open issues or submit pull requests to improve parsing, add backup options, or extend supported frames.

## Sample Execution

|      | Terminal Output |
|------|-----------------|
| **Help** | <img width="1920" height="1080" alt="Screenshot 2026-01-02 152202" src="https://github.com/user-attachments/assets/ee7812f4-c3da-41db-865e-c89ecc69e249" /> |
| **View** | <img width="1920" height="1080" alt="Screenshot 2026-01-02 152021" src="https://github.com/user-attachments/assets/18ca7820-8b0e-4385-a4e7-918bf28f987c" /> |
| **Edit** | <img width="1920" height="1080" alt="Screenshot 2026-01-02 152341" src="https://github.com/user-attachments/assets/0e15a753-ec04-4c07-9d66-09f735a57e90" /> |
