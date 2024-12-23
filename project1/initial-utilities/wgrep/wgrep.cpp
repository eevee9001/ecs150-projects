#include <fcntl.h>
#include <unistd.h>
#include <iostream>

// argc is the number of command line arguments, argv is the array of command line arguments
// ie argv[0] is the function call, argv[1] is the search term, argv[2] is the first file

using namespace std;

ssize_t readLine(int fd, string& line, string& buffer, ssize_t& bufferPosition, ssize_t& bytesRead, size_t bufferSize) {
    line.clear();
    // read one char at a time until end of file or \n reached
    while(true) {
        if (bufferPosition >= bytesRead){
            // read more data into buffer
            bytesRead = read(fd, &buffer[0], bufferSize);
            if (bytesRead == -1) {
                cout << "wgrep: cannot open file\n";
                return 1;
            } else if (bytesRead == 0) {
                //end of file
                return line.empty() ? 0 : line.size();
            } 
            bufferPosition = 0;
        }
        while (bufferPosition < bytesRead) {
            char ch = buffer[bufferPosition++];
            line += ch;
            if (ch == '\n') {
                return line.size();
            }
        }
    }   
}

void searchInFileDescriptor(int fd, const string& searchTerm, size_t bufferSize) {
    string line;
    string buffer(bufferSize, '\0');
    ssize_t bufferPosition = bufferSize;
    ssize_t bytesRead = 0;
    while (true) {
        ssize_t result = readLine(fd, line, buffer, bufferPosition,bytesRead, bufferSize);
        if (result <= 0) {
            break;
        }
        
        if (line.find(searchTerm) != string::npos) {
            write(STDOUT_FILENO, line.c_str(), line.size());
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // no command line arguments -> return 1
        cout <<"wgrep: searchterm [file ...]\n";
        return 1; 
    }
    string searchTerm = argv[1];
    const size_t bufferSize = 4096;

    if (argc == 2) {
        // no files specified -> read from stdin
        searchInFileDescriptor(STDIN_FILENO, searchTerm, bufferSize);
    } else {
        // at least one file specified
        for (int i=2; i < argc; i++) {
            int fileDescriptor = open(argv[i], O_RDONLY);
            if (fileDescriptor == -1) {
                // fails to open a file -> print exact message and return 1
                cout << "wgrep: cannot open file\n";
                return 1;
            }
            searchInFileDescriptor(fileDescriptor, searchTerm, bufferSize);
            close(fileDescriptor);
        }
    }
    return 0;
}