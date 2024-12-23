#include <fcntl.h>
#include <unistd.h>
#include <iostream>

// argc is the number of command line arguments, argv is the array of command line arguments
// ie argv[0] is the function call "./wzip", argv[1] is the first input file

using namespace std;

int main(int argc, char *argv[]) {
    int count = 0;
    char currentByte;
    char nextByte;
    int bytesRead;
    int bufferSize = 5000;
    char buffer[bufferSize];

    if (argc == 1) {
        // no file arguments -> return 1
        cout << "wzip: file1 [file2 ...]\n";
        return 1;
    }

    for (int j = 1; j < argc; j++) {
        // open files
        int fileDescriptor = open(argv[j], O_RDONLY);
            if (fileDescriptor == -1) {
                // fails to open a file -> print exact message and return 1
                cout << "wzip: cannot open file\n";
                return 1;
            }
        while ((bytesRead = read(fileDescriptor, buffer, bufferSize)) > 0) {
            for (int i = 0; i < bytesRead; i++) {
                //initialize nextByte
                nextByte = buffer[i];
                if (count == 0) {
                    // if first iteration, increment both currentByte and count
                    currentByte = nextByte;
                    count = 1;
                } else if (currentByte == nextByte) {
                    count++;
                } else {
                    write(STDOUT_FILENO, &count, 4);
                    write(STDOUT_FILENO, &currentByte, 1);
                    // update state for potential next file
                    currentByte = nextByte;
                    count = 1;
                }
            }
        }
        close(fileDescriptor);
    }
    if (count > 0) {
        write(STDOUT_FILENO, &count, 4);
        write(STDOUT_FILENO, &currentByte, 1);
    }
    return 0;
}