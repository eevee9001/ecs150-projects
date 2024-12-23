#include <fcntl.h>
#include <unistd.h>
#include <iostream>

// argc is the number of command line arguments, argv is the array of command line arguments
// ie argv[0] is the function call, argv[1] is the first file parameter

using namespace std;

int main(int argc, char *argv[]) {
    int bytesRead = 0;
    char buffer[4096];
    if (argc == 1) {
        // no files on command line -> exit and return 0
        return 0; 
    } else {
        for (int i=0; i < argc-1; i++) {
            int fileDescriptor = open(argv[i+1], O_RDONLY);
            if (fileDescriptor == -1) {
                // fails to open a file -> print exact message and return 1
                cout << "wcat: cannot open file\n";
                return 1;
            }
            while ((bytesRead = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
                // don't care about total bytesRead, just need amount to write to stdout
                write(STDOUT_FILENO, buffer, bytesRead);
                if(fileDescriptor == STDIN_FILENO) {
                close(fileDescriptor);
                }
            }
        }
    }
    return 0;
}