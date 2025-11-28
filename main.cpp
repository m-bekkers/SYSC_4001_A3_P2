/**
 * @author Matthe Bekkers
 * @author Shael Kotecha
 */
#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <filesystem>
#include <vector>
#include <ifstream>

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <dirent.h>

#include "main.hpp"

#define STUDENTFILESIZE 4 // assume student files will all be 4 bytes long

// exit if argc is not 1
// argv is the number of TAs
int main(int argc, char* argv[]) {

    const key_t KEY = 6969;
    int student_counter = 0;
    std::vector<std::string> student_list; // holds filenames in shared mem

    // need null ptrs to assign later
    struct dirent *entry = nullptr;
    DIR *dp = nullptr;

    std::string input_buffer;
    std::vector<char> rubric;
    struct shared_vars *shared_data;

    dp = opendir("students");
    if (dp != (void *)0) {
        while (entry = readdir(dp)) {
            student_list.push_back(entry->d_name);
        }
    }

    std::ifstream read_rubric("rubric.txt");
    while(std::getline(read_rubric, input_buffer)) {
        rubric.push_back(input_buffer[3]);
    }

    // create PID
    pid_t pid;
    pid = fork();

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
    
    // create shared memory the size of 
    int shmid = shmget(KEY, sizeof(struct shared_data), 0666 | IPC_CREAT); 

    // check that shmget worked
    if (shmid == -1) {
        std::cout << "ERROR: shmget failure";
        return -1;
    }

    // make shared memory accessible to program
    int shared_mem = shmat(shmid, (void *)0, 0);

    // check that shmat worked
    if (shared_mem == -1) {
        std::cout << "ERROR: shmat failure";
        return -1;
    }

    std::cout << "Shared memory attached at " << shared_mem << "." << std::endl;

    // shared_mem contains the first address of the shared memory so we cast it to a shared_vars struct
    // this makes it so that the 2 variables defined in shared_vars now are in shared memory
    shared_data = (struct shared_vars *)shared_mem;

    return EXIT_SUCCESS;
}