#ifndef MAIN_HPP
#define MAIN_HPP

#define NUMSTUDENTS 20
#define NUMQUESTIONS 5

#include <map>
#include <string>

struct shared_vars {
    char rubric_mem[NUMQUESTIONS];
    int students[NUMSTUDENTS];
    sem_t per_student_semaphore[NUMSTUDENTS];
    int student_grading_counter[NUMSTUDENTS];
};

#endif