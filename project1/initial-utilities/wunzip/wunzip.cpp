    #include <fcntl.h>
    #include <unistd.h>
    #include <iostream>

    // argc is the number of command line arguments, argv is the array of command line arguments
    // ie argv[0] is the function call, argv[1] is the first file parameter

    using namespace std;

    int main(int argc, char *argv[]) {
        if (argc == 1) {
            // no file arguments -> return 1
            cout << "wunzip: file1 [file2 ...]\n";
            return 1;
        }
        int count;
        char currentByte;
        for (int i = 1; i < argc; i++) {
            int fileDescriptor = open(argv[i], O_RDONLY);
            if (fileDescriptor == -1) { 
                cout << "wunzip: cannot open file\n"; 
                return 1; 
            }
            while (read(fileDescriptor, &count, 4) > 0) {
                if (read(fileDescriptor, &currentByte, 1) == 0) {
                    cout << "wunzip: unexpected end of file \n";
                    return 1;
                }
                for (int i = 0; i < count; i++) {
                    write(STDOUT_FILENO, &currentByte, 1);
                }
            }
            close(fileDescriptor);
        }
        return 0;
    }