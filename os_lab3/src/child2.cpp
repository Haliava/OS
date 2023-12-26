#include <iostream>
#include <algorithm>
#include "../include/utils.h"

std::string replace_spaces(std::string source, char replacement) {
    for (auto spaceChar: {' ', '\t', '\r', '\t', '\v'})
        std::replace(source.begin(), source.end(), spaceChar, replacement);

    return source;
}

sem_t* semaphoreList[2];
void handleSigterm(int signum) {
    for (int i = 0; i < 2; ++i) {
        close(i);
        sem_close(semaphoreList[i]);
    }
    exit(signum);
}

int main(int argc, char *argv[]) {
    std::cout << "This is child process with pid: " << getpid() << std::endl;

    for (int i = 0; i < 2; ++i) {
        semaphoreList[i] = sem_open(argv[i], O_CREAT);
    }

    struct stat sd;

    while (true) {
        sem_wait(semaphoreList[0]);

        if (signal(SIGTERM, handleSigterm) == SIG_ERR) {
            perror("Error while setting a signal handler");
            return EXIT_FAILURE;
        }

        if (fstat(STDIN_FILENO, &sd) == -1) {
            perror("could not get file size. \n");
        }

        char* mp = mmapCreate(sd.st_size, STDIN_FILENO);
        std::string res;
        for (int i = 0; i < sd.st_size; ++i) {
            res += mp[i];
        }

        res = replace_spaces(res, '_');
        for (int i = 0; i < res.size(); ++i) {
            mp[i] = res[i];
        }

        sem_post(semaphoreList[1]);
    }

    return 0;
}
