#include <iostream>
#include "../include/utils.h"

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
        for (int i = 0; i < sd.st_size; ++i) {
            mp[i] = tolower(mp[i]);
        }

        sem_post(semaphoreList[1]);
    }

    return 0;
}
