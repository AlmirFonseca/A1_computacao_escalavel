#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

int main() {
    const char* dirPath = "../mock/mock_files/csv"; // Path to the directory you want to list
    struct dirent *entry;
    struct stat statbuf;
    DIR *dir = opendir(dirPath);

    if (dir == NULL) {
        perror("Failed to open directory");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

        // Use stat to check if it's a regular file
        if (stat(fullPath, &statbuf) == 0) {
            if (S_ISREG(statbuf.st_mode)) {
                printf("%s\n", entry->d_name);
            }
        }
    }

    closedir(dir);
    return 0;
}
