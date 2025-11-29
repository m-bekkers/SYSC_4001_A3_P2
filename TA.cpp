#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <filesystem>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <ctime>

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

int main(void) {

    // make sure that the program gets different random numbers every time
    srand(time(0));

    int pid_num = getpid();

    // pointer to a shared vars struct, will contain shared memory
    struct shared_vars *shared_data;

    // shmat needs a void pointer
    void *shared_mem = nullptr;

    // get id of shared memory
    int shmid = shmget((key_t) 6969, sizeof(struct shared_vars), IPC_CREAT | 0666);

    // check for shmid failure
    if (shmid == -1) {
        std::cout << "Error while getting shared memory, exiting...";
        exit(EXIT_FAILURE);
    }

    shared_mem = shmat(shmid, (void *)0, 0); // make the shared mem accessible to program

    // check for shmat failure
    if (shared_mem == (void *)-1) {
        std::cout << "Error while using shmat, exiting...";
        exit(EXIT_FAILURE);
    }

    /* now that shared memory contains the first addess of the allocated mem, 
    cast it to a shared_vars struct so it has the 2 vars we want */
    shared_data = (struct shared_vars *)shared_mem; 

    int selected_student = 0; // make this -1 so that if there is a student_0000 it doesn't get selected
    int decision_delay;
    int edit_q; // flag to see if a question is edited
    int completed_students = 0; // tracker to make sure TAs don't grade a student who has been fully graded

    // input buffer to read the rubric
    std::string input_buffer;

    // main loop of things that happen
    while (selected_student != 9999) {

        // iterate through rubric and decide whether it needs to be fixed
        for (int i = 0; i < NUMQUESTIONS; i++) {
            // generate a random delay value between 500 and 1000 milliseconds
            decision_delay = ((rand() % 501) + 500);
            std::this_thread::sleep_for(std::chrono::milliseconds(decision_delay));

            // decide whether to edit a question
            edit_q = rand() % 4;

            if(edit_q == 0) {
                std::cout << "TA has decided to edit question " << i + 1 << std::endl;
                // edit question
                shared_data->rubric_mem[i] = shared_data->rubric_mem[i] + 1;

                // calls custom function to increment rubric
                increment_rubric(i, "rubric.txt");
            }

        }

        selected_student = shared_data->students[completed_students][0];
        std::cout << "TA has selected student number " << selected_student << std::endl;

        for (int i = 1; i < NUMQUESTIONS + 1; i++) {
            std::cout << "debug: " << shared_data->students[completed_students][i] << std::endl;
            
        }

        for (int i = 1; i < NUMQUESTIONS + 1; i++) {
            if (shared_data->students[completed_students][i] == 0) {
                std::cout << "TA is grading question " << i << " from student " << selected_student << std::endl;

                decision_delay = (rand() % 1001) + 1000;
                std::this_thread::sleep_for(std::chrono::milliseconds(decision_delay));

                shared_data->students[completed_students][i] = 1;
            }
        }

        /**

        while (shared_data->student_grading_counter[completed_students] < 6) { // 6 bc apparently they all start at 1??

            std::cout << shared_data->student_grading_counter[completed_students] - 1 << " questions graded" << std::endl;
            std::cout << "Student " << selected_student << " has " << 6 - shared_data->student_grading_counter[completed_students] << " ungraded questions" << std::endl;
            decision_delay = (rand() % 1001) + 1000;
            std::this_thread::sleep_for(std::chrono::milliseconds(decision_delay));
            shared_data->student_grading_counter[completed_students]++;
            std::cout << "TA has marked question " << shared_data->student_grading_counter[completed_students] - 1 << std::endl;
        } 
        */
       completed_students++;
       std::cout << "completed students: " << completed_students << std::endl;
    }

    exit(0);

    return 0;
}