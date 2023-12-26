#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>

void closeFD(int fd) {
    if (close(fd) == -1) {
        std::cerr << "Error: failed closing fd - " << fd << std::endl;
        exit(-1);
    }
}

void dup2FD(int oldfd, int newfd) {
    if (dup2(oldfd, newfd) == -1) {
        std::cerr << "Error: failed dup2 to this fds - "
                  << oldfd << '\t' << newfd << std::endl;
        exit(-1);
    }
}

void pipeFD(int *fd) {
    if (pipe(fd) == -1) {
        std::cerr << "Error: failed creating pipe" << std::endl;
        exit(-1);
    }
}

pid_t createProcess() {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Error: failed creating child process" << std::endl;
        exit(-1);
    }
    return pid;
}

int openSharedMemoryFile(const std::string& shmFileName) {
    int shmFileDescriptor = shm_open(shmFileName, O_CREAT | O_RDWR, S_IRWXU);
    if (shmFileDescriptor == -1) {
        std::cerr << "Error: failed to open file with name - " << shmFileName << std::endl;
        exit(-1);
    }
    return shmFileDescriptor;
}

char* mmapCreate(size_t len, int shmFileDescriptor) {
    char* mp = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, shmFileDescriptor, 0);
    if (mp == MAP_FAILED) {
        std::cerr << "Error using mmap for " << shmFileDescriptor << std::endl;
        exit(-1);
    }
    return mp;
}

int openFile(const char *str) {
    int file = open(str, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (file == -1) {
        std::cerr << "Error: failed openin file with name - " << str << std::endl;
        exit(-1);
    }
    return file;
}

void execute(const char *str1) {
    if (execl(str1, str1, NULL) == -1) {
        std::cerr << "Error: failed to execute child" << std::endl;
        exit(-1);
    }
}