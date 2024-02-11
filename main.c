#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int createArchive(const char* archiveName, const char* files[], int numFiles) {
    FILE* archive = fopen(archiveName, "wb");
    if (archive == NULL) {
        return -1;
    }
    for (int i = 0; i < numFiles; i++) {
        FILE* file = fopen(files[i], "rb");
        if (file == NULL) {
            fprintf(stderr,"Error opening file %s\n", files[i]);
            continue;
        }
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file,0,SEEK_SET);
        fwrite(&fileSize, sizeof(long), 1, archive);
        int fileNameLength = strlen(files[i]);
        fwrite(&fileNameLength, sizeof(int), 1, archive);
        fwrite(files[i], sizeof(char), fileNameLength, archive);
        char* buffer = (char*)malloc(fileSize);
        fread(buffer, 1, fileSize, file);
        fwrite(buffer, 1, fileSize, archive);
        free(buffer);
        fclose(file);
    }
    fclose(archive);
    return 0;
}

int extractArchive(const char* archiveName) {
    FILE* archive = fopen(archiveName, "rb");
    if (archive==NULL) {
        return -2;
    }
    while (!feof(archive)) {
        long fileSize;
        fread(&fileSize, sizeof(long), 1, archive);
        int fileNameLength;
        fread(&fileNameLength, sizeof(int), 1, archive);
        char* filename = (char*)malloc(fileNameLength + 1);
        if(filename == NULL){
            return -3;
        }
        fread(filename, sizeof(char), fileNameLength, archive);
        filename[fileNameLength] = '\0';
        char* buffer = (char*)malloc(fileSize);
        if(buffer == NULL){
            return -1;
        }
        fread(buffer, 1, fileSize, archive);
        FILE* file = fopen(filename, "wb");
        fwrite(buffer, 1, fileSize, file);
        free(buffer);
        free(filename);
        fclose(file);

    }
    fclose(archive);
    return 0;
}

int listFiles(const char* archiveName) {
    FILE* archive = fopen(archiveName, "rb");
    if (archive == NULL) {
        return -1;
    }
    int fileNum = 1;
    while (!feof(archive)) {
        long fileSize;
        fread(&fileSize, sizeof(long), 1, archive);
        if(feof(archive)){
            break;
        }
        int fileNameLength;
        fread(&fileNameLength, sizeof(int), 1, archive);
        char* filename = (char*)malloc(fileNameLength+1);
        if(filename == NULL){
            return -2;
        }
        fread(filename, sizeof(char), fileNameLength, archive);
        filename[fileNameLength] = '\0';
        printf("File %d: %s, size %ld bytes\n", fileNum, filename, fileSize);
        fileNum++;
        fseek(archive, fileSize, SEEK_CUR);
        free(filename);
    }
    fclose(archive);
    return 0;
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Not enough command line arguments");
        return 1;
    }
    const char* archiveName = NULL;
    const char* files[100];
    int numFiles = 0;
    int extractingArchive = 0;
    int listingFiles = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--file") == 0) {
            archiveName = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "--create") == 0) {
            for (int j = i+1; j < argc; j++) {
                files[numFiles] = argv[j];
                numFiles++;
            }
        } else if (strcmp(argv[i], "--extract") == 0) {
            extractingArchive = 1;
        } else if (strcmp(argv[i], "--list") == 0) {
            listingFiles = 1;
        }
    }
    if (archiveName!=NULL) {
        if(extractingArchive==1) {
            int k = extractArchive(archiveName);
            if((k == -1)|| (k == -3)){
                fprintf(stderr, "Memory allocation error\n");
            }
            else if(k == -2){
                fprintf(stderr, "Error opening archive\n");
            }
            else{
                fprintf(stdout,"Files were successfully unpacked\n");
            }
        }
        else if(listingFiles == 1){
            int l = listFiles(archiveName);
            if(l == -1){
                fprintf(stderr, "Error opening archive\n");
            }
            else if(l == -2){
                fprintf(stderr, "Memory allocation error\n");
            }
            else{
                fprintf(stdout,"List of files in the archive\n");
            }
        }
        else if(numFiles > 0){
            int p = createArchive(archiveName,files,numFiles);
            if(p == -1){
                fprintf(stderr, "Error creating archive\n");
            }
            else{
                fprintf(stdout,"Archive created successfully\n");

            }
        }
        else{
            fprintf(stderr, "There are not enough files to create an archive\n");
            return 0;
        }
    }
    else{
        fprintf(stderr,"No archive name\n");

    }
    return 0;

}
