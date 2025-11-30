#ifndef MAIN_HPP
#define MAIN_HPP

#define NUMSTUDENTS 20
#define NUMQUESTIONS 5
#define STUDENTFILESIZE 4 // assume student files will all be 4 bytes long

#include <string>
#include <fstream>
#include <vector>
#include <iostream>

#include <sys/sem.h>

// struct to hold all shared memory stuff
struct shared_vars {
    char rubric_mem[NUMQUESTIONS];

    /**
     * students is a matrix of n students with m questions each:
     * 
     * _______0__________1_____2_____3_____4_____5__
     * _______________|done?|done?|done?|done?|done?
     * student_number |  0  |  0  |  0  |  0  |  0
     * ____________________________________________
     * student_number |  0  |  0  |  0  |  0  |  0
     * .....
     * 
     * if done? == 1, that question has been graded
     * 
     */
    int students[NUMSTUDENTS][NUMQUESTIONS + 1]; 
    //sem_t per_question_semaphore[NUMQUESTIONS];
};

/**
 * Function to replace rubric answer if an answer is selected to be corrected
 * The way this works is that the entire file gets rewritten since C++ sucks at file I/O
 */
void increment_rubric(int question_index, const std::string &filepath) {
    std::vector<std::string> lines;
    std::string line;

    std::ifstream infile(filepath);
    if (!infile.is_open()) {
        std::cout << "ERROR: cannot open rubric file." << std::endl;
        return;
    }

    while (std::getline(infile, line)) {
        lines.push_back(line);
    }
    infile.close();

    if (question_index < 0 || question_index >= (int)lines.size()) {
        std::cout << "ERROR: index out of range" << std::endl;
        return;
    }

    if (lines[question_index].size() >= 4) {
        char &answer = lines[question_index][3];
        answer++;
    }

    std::ofstream outfile(filepath);

    if (!outfile.is_open()) {
        std::cout << "ERROR: cannot write rubric file" << std::endl;
        return;
    }

    for (const auto &l : lines) {
        outfile << l << std::endl;
    }

    outfile.close();

}

#endif