#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <semaphore.h>


void dup2FD(int oldfd, int newfd) {
    if (dup2(oldfd, newfd) == -1) {
        std::cerr << "Error: failed dup2 to this fds - " << oldfd << '\t' << newfd << std::endl;
        exit(-1);
    }
}

int setFileSize(int fileDesc, int fileSize) {
    if (ftruncate(fileDesc, fileSize) == -1) {
        std::cerr << "Error: could not set size of file " << std::endl;
        exit(-1);
    }
    return 0;
}

pid_t createProcess() {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Error: failed creating child process" << std::endl;
        exit(-1);
    }
    return pid;
}

sem_t *semaphoreOpen(std::string name) {
    sem_t *semAddress = sem_open(name.c_str(), O_CREAT, S_IRWXU, 0);
    if (semAddress == SEM_FAILED) {
        std::cerr << "Error: failed opening \"" << name << "\" semaphore" << std::endl;
        exit(-1);
    }
    return semAddress;
}

int incrementSemaphore(sem_t *semaphore) {
    if (sem_post(semaphore) == -1) {
        perror("here");
    }
    return 0;
}

int killProcess(pid_t processId) {
    if (kill(processId, SIGTERM) == -1) {
        perror("Ошибка при отправке сигнала");
    }
    return 0;
}

int openSharedMemoryFile(std::string shmFileName) {
    int shmFileDescriptor = shm_open(shmFileName.c_str(), O_CREAT | O_RDWR, S_IRWXU);
    if (shmFileDescriptor == -1) {
        std::cerr << "Error: failed to open file with name - " << shmFileName << std::endl;
        exit(-1);
    }
    return shmFileDescriptor;
}

char* mmapCreate(size_t len, int shmFileDescriptor) {
    char* mp = reinterpret_cast<char*>(mmap(NULL, len, 0666, MAP_SHARED, shmFileDescriptor, 0));
    if (mp == MAP_FAILED) {
        std::cerr << "Error using mmap for " << shmFileDescriptor << std::endl;
        exit(-1);
    }
    return mp;
}

int openFile(const char *str) {
    int file = open(str, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (file == -1) {
        std::cerr << "Error: failed to open file " << str << std::endl;
        exit(-1);
    }
    return file;
}

void execute(std::string str1, std::string semaphoreFrom, std::string semaphoreTo) {
    if (execl(str1.c_str(), semaphoreFrom.c_str(), semaphoreTo.c_str(), NULL) == -1) {
        std::cerr << "Error: failed to execute child" << std::endl;
        exit(-1);
    }
}