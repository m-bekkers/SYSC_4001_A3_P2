/**
 * SYSC 4001 Assignment 3
 * 
 * @author Matthe Bekkers
 * @author Shael Kotecha
 */
#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <filesystem>
#include <vector>
#include <cstring>

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <dirent.h>

#include "main.hpp"

// exit if argc is not 1
// argv is the number of TAs
int main(int argc, char* argv[]) {

    // shared key for memory access
    const key_t KEY = 6969;

    // vector to hold the rubric data
    std::vector<char> rubric;
    
    // buffer to hold student numbers
    std::vector<int> student_buffer;

    // input buffer to read the rubric
    std::string input_buffer;

    // this struct will hold all shared data for the TAs to access
    struct shared_vars *shared_data;

    // holds the file name of the current student
    std::string current_student;

    // struct to handle file & directory I/O
    struct dirent *entry = nullptr;

    // pointer to desired directory
    DIR *dp = nullptr;

    // make sure the correct arguments were passed
    if (argc == 1) {
        std::cout << "ERROR: No arguments passed beyond program name. Exiting with code 5..." << std::endl;
        return EIO;
    } else if (argc > 2) {
        std::cout << "ERROR: Expected 1 command line argument but received " << argc - 1 << ". Exiting with code 5..." << std::endl;
        return EIO;
    } else { // if we get here only 1 CLI arg was passed so proceed
        std::cout << "Program name is " << argv[0] << std::endl;
        std::cout << "Expected 1 command line argument and received 1." << std::endl;
        std::cout << "Number of TAs = " << argv[1] << std::endl;
    }

    // read rubric
    std::ifstream file("rubric.txt");
    
    // read each line and append the correct answer to the rubric vector
    if (file.is_open()) {
        while (std::getline(file, input_buffer)) {
            rubric.push_back(input_buffer[3]);
        }

        file.close();
    } else {
        std::cout << "ERROR: unable to open rubric." << std::endl;
        return -1;
    }

    // debug
    for (char i: rubric) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
    
    // create shared memory the size of a shared_vars struct
    int shmid = shmget((key_t) 6969, sizeof(struct shared_vars), IPC_CREAT | 0666); 

    // check that shmget worked
    if (shmid == -1) {
        std::cout << "ERROR: shmget failure" << std::endl;
        return -1;
    }

    // make shared memory accessible to program
    void *shared_mem = shmat(shmid, nullptr, 0);

    // check that shmat worked
    if (shared_mem == (void *)-1) {
        std::cout << "ERROR: shmat failure";
        return -1;
    }

    // debug
    std::cout << "Shared memory attached at " << shared_mem << "." << std::endl;

    // shared_mem contains the first address of the shared memory so we cast it to a shared_vars struct
    // this makes it so that the 2 variables defined in shared_vars now are in shared memory
    shared_data = (struct shared_vars *)shared_mem;

    // assign dp to the desired directory
    dp = opendir("students");

    // iterate through each student and add them to the student buffer
    // this entire block of code a disgusting amalgamation of C and C++ and I hate it
    if (dp != nullptr) {
        while (entry = readdir(dp)) {
            // do this to ignore current and prev dir
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            } else {

                std::string filepath = std::string("students/") + entry->d_name;
                
                // read file contents
                std::ifstream file(filepath);
                if (file.is_open()) {
                    char sn_buf[8];
                    int tmp_buf;

                    while (std::getline(file, input_buffer)) {
                        // since we are forced to make input_buffer a string, we must convert it to an
                        // array of chars since sscanf can't take strings
                        strcpy(sn_buf, input_buffer.c_str());
                        
                        sscanf(sn_buf, "%d", &tmp_buf);
                        student_buffer.push_back(tmp_buf);
                        
                        std::cout << "adding " << entry->d_name << " to buffer" << std::endl;
                    }

                    file.close();

                } else {
                    std::cout << "ERROR: unable to open student file." << std::endl;
                    return -1;
                }
            }
        }
    }

    closedir(dp);

    // initialize student stuff
    for (int i = 0; i < NUMSTUDENTS; i++) {
        shared_data->students[i][0] = student_buffer[i];
        //sem_init(&shared_data->per_question_semaphore[i], 1, 1);
        
        shared_data->student_grading_counter[i] = 0; // each student starts as fully ungraded

        for (int j = 0; j < NUMQUESTIONS; j++) {
            shared_data->students[i][j + 1] = 0; // each question starts as ungraded
        }
    }

    // add rubric to the shared rubric array
    for (int i = 0; i < NUMQUESTIONS; i++) {
        shared_data->rubric_mem[i] = rubric[i];
    }

    /**
     * If we reach this point, everything the TA (child) processes will need has been added to shared memory
     * We can now create as many TAs as specified in argv
     */

    // create PID
    pid_t pid;
    pid = fork();

    // see whether we are the parent process or a TA (child process)
    switch (pid)
    {
    case -1:
        std::cout << "ERROR: fork() failed." << std::endl;
        exit(1);
        break;

    case 0:
        std::cout << "Process is a TA" << std::endl;
        execl("./TA", "TA", NULL);
        break;
    
    default:
        std::cout << "Process is parent" << std::endl;
        break;
    }

    return EXIT_SUCCESS;
}