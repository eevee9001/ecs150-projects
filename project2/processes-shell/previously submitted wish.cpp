#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <fcntl.h>

using namespace std;
vector<string> paths = {"/bin"};

void set_path(const string &cmd_args) {
    // no arguments provided so do nothing.
    if (cmd_args.empty()) {
            return;
        }
    paths.clear(); // Always clear path settings
    
    // Tokenize arguments
    istringstream iss(cmd_args);
    string token;
    while (iss >> token) {
        paths.push_back(token);
    }
} 

string find_executable(const string &cmd_name) {
    if (paths.empty()) {
        return ""; // Path set to empty, only built ins should run
    }

    char full_path[1024];
    for (const auto &path : paths) {
        snprintf(full_path, sizeof(full_path), "%s/%s", path.c_str(), cmd_name.c_str());
        if (access(full_path, X_OK) == 0) {
            return string(full_path);
        }
    }
    return ""; // Command not found in any of the paths
}

void call_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

void execute_command(const string &command) {
    istringstream iss(command);
    string cmd_name;
    iss >> cmd_name;
    string cmd_args;
    getline(iss, cmd_args);
    cmd_args.erase(0, cmd_args.find_first_not_of(" ")); // Remove leading spaces

    // Handle built-in commands
    if (cmd_name == "exit") {
        if (!cmd_args.empty()) {
            call_error();
            return; // Do not exit the shell
        }
        exit(0);
    } 
    else if (cmd_name == "cd") {
        if (cmd_args.empty()) { // no path argument
            call_error();
        } else {
            if (chdir(cmd_args.c_str()) != 0) {
                call_error();
            }
        }
    }
    else if (cmd_name == "path") {
        set_path(cmd_args);
    } else {
        // Handle redirection
        string redirect_file;
        size_t redirect_pos = cmd_args.find(">");
        if (redirect_pos != string::npos) {
            // Split the command and the redirection part
            string actual_args = cmd_args.substr(0, redirect_pos);
            redirect_file = cmd_args.substr(redirect_pos + 1);
            // Remove any leading/trailing whitespaces
            actual_args.erase(actual_args.find_last_not_of(" ") + 1);
            redirect_file.erase(0, redirect_file.find_first_not_of(" "));
            redirect_file.erase(redirect_file.find_last_not_of(" ") + 1);

            cmd_args = actual_args; // Update cmd_args to be the actual command arguments
            if (cmd_args.empty())
                {
                    call_error();
                }
        } else {
            // Handle redirection
            string redirect_file;
            size_t redirect_pos = cmd_args.find(">");
            if (redirect_pos != string::npos) {
                // Split the command and the redirection part
                string actual_args = cmd_args.substr(0, redirect_pos);
                redirect_file = cmd_args.substr(redirect_pos + 1);
                // Remove any leading/trailing whitespaces
                actual_args.erase(actual_args.find_last_not_of(" ") + 1);
                redirect_file.erase(0, redirect_file.find_first_not_of(" "));
                redirect_file.erase(redirect_file.find_last_not_of(" ") + 1);

                cmd_args = actual_args; // Update cmd_args to be the actual command arguments
                
                
                // Check if the redirection file is empty (error condition)
                if (redirect_file.empty()) {
                    call_error();
                    return;
                }
            }
        // Try to execute external commands
        string executable = find_executable(cmd_name);
        if (executable.empty()) {
            call_error();
        } else {
            // Prepare to execute the command
            pid_t pid = fork();
            if (pid == 0) {
                // Child process

                // Handle output redirection if specified
                if (!redirect_file.empty()) {
                    int fd = open(redirect_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        call_error();
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO); // Redirect stdout to the file
                    close(fd); // Close the file descriptor
                }
                /*
                char *exec_args[] = {strdup(executable.c_str()), strdup(cmd_args.c_str()), NULL};
                execv(exec_args[0], exec_args); */

                // Prepare command arguments
                vector<char*> exec_args;
                exec_args.push_back(strdup(executable.c_str()));

                // Split cmd_args if there are additional arguments
                istringstream args_stream(cmd_args);
                string arg;
                while (args_stream >> arg) {
                    exec_args.push_back(strdup(arg.c_str()));
                }

                exec_args.push_back(nullptr); // Null-terminate the array

                // Execute the command
                execv(exec_args[0], exec_args.data());

                // If execv fails
                call_error();
                exit(1);
            } else if (pid > 0) {
                // Parent process waits for child
                wait(NULL);
            } else {
                call_error();
            }
        }
        }
    }
}

int main(int argc, char *argv[]) {
    string command;
    ifstream input_file;

    // Check if a file name was provided as a command-line argument
    if (argc > 1) {
        input_file.open(argv[1]);
        if (!input_file.is_open()) {
            cerr << "Error: Could not open file " << argv[1] << endl;
            return 1;
        }
    }

                                                                    // // If command line arguments are provided, use them to set the initial path
                                                                    // if (argc > 2) {
                                                                    //     string initial_path;
                                                                    //     for (int i = 2; i < argc; i++) {
                                                                    //         initial_path += argv[i];
                                                                    //         if (i < argc - 1) {
                                                                    //             initial_path += " ";
                                                                    //         }
                                                                    //     }
                                                                    //     set_path(initial_path);
                                                                    // }
 
    // Read commands from either the input file or standard input
    while (true) {
        if (argc > 1) {
            // Batch mode: read from the file
            if (argc > 2)
            {   
                // call exactly one file as argument in batch mode
                call_error();
                exit(1);
            } else if (!getline(input_file, command)) {
                break; // End of file reached
            }
        } else {
            // Interactive mode: read from standard input
            cout << "wish> ";
            getline(cin, command);
            if (command.empty()) {
                continue; // Ignore empty commands
            }
        }

        execute_command(command);
    }

    if (input_file.is_open()) {
        input_file.close();
    }

    return 0;
}