// Custom File Archiver in C

/*
Available commands =>
    -- pack output.pack file1.txt file2.png etc,...
    -- unpack output.pack 
    -- help
    -- exit
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

//--------------------------

typedef struct {
    char fileName[100];
    uint32_t fileSize;
} Header;

//--------------------------

char* getInput(void);
uint32_t getFileSize(char* fileName);
void writeFileToArchive(char* fileName, FILE* archive);
void operation(char* command, bool* isRunning);
void pack(char* archiveName);
void unpack(char* archiveName);

//--------------------------


int main(void) {
    bool isRunning = true;

    // Welcome Text
    printf("Pack Archiver\n");
    printf("Version v1.0.2931. MIT License\n");
    printf("Use 'help' command to view all available commands\n\n");

    // Main Loop
    while (isRunning) {
        char* command = getInput();
        operation(command, &isRunning);
    }

    return 0;
}

char* getInput(void) {
    static char command[256];

    printf("packArchiver> ");
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0';

    return command;
}

uint32_t getFileSize(char* fileName) {
    FILE* file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("Error: %s failed to load\n\n", fileName);
        return 0;
    }

    // Calculating File Size
    fseek(file, 0, SEEK_END);
    uint32_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fclose(file);
    
    return fileSize;
}

void writeFileToArchive(char* fileName, FILE* archive) {
    uint8_t buffer[512];
    size_t bytesRead;

    FILE* file = fopen(fileName, "rb");

    if (file == NULL) {
        printf("Error: %s does not exist\n\n", fileName);
        return;
    }

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        fwrite(buffer, 1, bytesRead, archive);
    }

    fclose(file);

    // deleting the standalone file
    remove(fileName);
} 

void operation(char* command, bool* isRunning) {
    char* kwd = strtok(command, " "); 

    if (kwd == NULL) return;

    if (strcmp(kwd, "pack") == 0) {
       char* archiveName = strtok(NULL, " ");
       if (archiveName == NULL) {
            printf("Error: Missing Archive Name\n");
            printf("Use 'help' command to view all available commands\n\n");
            return;
       }

       // packing multiple files into archive
       pack(archiveName);
    }

    else if (strcmp(kwd, "unpack") == 0) {
        char* archiveName = strtok(NULL, " ");
        if (archiveName == NULL) {
            printf("Error: Missing Archive Name\n");
            printf("Use 'help' command to view all available commands\n\n");
            return;
        }

        // unpacking multiple files from archives
        unpack(archiveName);
    }

    else if (strcmp(kwd, "help") == 0) {
        printf("Available Commands => \n");
        printf("-- pack output.pack file1.txt file2.png etc,...\n");
        printf("-- unpack output.pack\n");
        printf("-- help\n");
        printf("-- exit\n\n");
    }

    else if (strcmp(kwd, "exit") == 0) {
        *isRunning = false;
    } 

}

void pack(char* archiveName) {
    char* fileName = strtok(NULL, " ");
    if (fileName == NULL) {
        printf("Error: Missing Files to Pack\n");
        printf("Use 'help' command to view all available commands\n\n");
        return;
    }
    
    FILE* archive = fopen(archiveName, "ab");
    if (archive == NULL) {
        printf("Error: File failed to load\n\n");
        return;
    }

    while (fileName != NULL) {

        // Preparing Header
        Header header;
        memset(&header, 0, sizeof(Header)); // clears the garbage
        strncpy(header.fileName, fileName, sizeof(header.fileName) - 1);
        header.fileSize = getFileSize(fileName);

        // Writing the header to archive
        fwrite(&header, sizeof(Header), 1, archive);

        // Writing the contents of file to our archive
        writeFileToArchive(fileName, archive);

        // moving to next file
        fileName = strtok(NULL, " ");
    }

    fclose(archive);
}

void unpack(char* archiveName) {
    Header header;
    uint8_t buffer[512];

    FILE* archive = fopen(archiveName, "rb");
    if (archive == NULL) {
        printf("Error: Failed to load %s. Try Again\n\n", archiveName);
        return;
    }

    while (fread(&header, sizeof(Header), 1, archive) == 1) {
        FILE* file = fopen(header.fileName, "wb");
        if (file == NULL) {
            printf("Error: Failed to extract %s, skipping %s...\n\n", header.fileName, header.fileName);
            fseek(archive, header.fileSize, SEEK_CUR);
            continue;
        }

        uint32_t bytesRemaining = header.fileSize;

        while (bytesRemaining > 0) {
            size_t bytesToRead = (bytesRemaining < sizeof(buffer)) ? bytesRemaining : sizeof(buffer);
            size_t bytesRead = fread(buffer, 1, bytesToRead, archive);

            fwrite(buffer, 1, bytesRead, file);
            bytesRemaining -= bytesRead;
        }
        
        fclose(file);
    }

    fclose(archive);

    // deleting the archive
    remove(archiveName);
}   




