#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

// argc is the number of command line arguments, argv is the array of command line arguments
// ie argv[0] is ./wish, argv[1] is the batch file, only for batch mode

void call_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

int main(int argc, char *argv[]) {
    string command;
    ifstream input_file;

    // read commands from either input file or standard input
    // allow 0 or 1 arguments after ./wish
    while (true) {
        // if shell is invoked with more than one file or bad batch file -> exit(1)
        if (argc > 2) {
            call_error();
            exit(1);
        } else if (argc == 2) {
            input_file.open(argv[1]);
            if (!input_file.is_open()) {
                call_error();
                exit(1);
            }
            // end of file reached ---- MAYBE NEED TO EXIT(0)
            if (!getline(input_file, command) ){
                break;
            }
        } else { // start interactive mode
            cout << "wish> ";
            getline(cin,command);
            // ignore empty lines
            if (command.empty()) {
                continue;
            }
        }

        // execute_command;

    }
    if (input_file.is_open()) {
        input_file.close();
    }

    return 0;
}