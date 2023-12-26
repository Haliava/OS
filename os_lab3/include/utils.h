#ifndef OC_LAB1_UTILS_H
#define OC_LAB1_UTILS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>
#include <semaphore.h>

void dup2FD(int oldfd, int newfd);
pid_t createProcess();

sem_t* semaphoreOpen(std::string);
int incrementSemaphore(sem_t*);
int setFileSize(int, int);
int openSharedMemoryFile(std::string);
char* mmapCreate(size_t, int);

void execute(std::string, std::string, std::string);
int killProcess(pid_t);

#endif //OC_LAB1_UTILS_H
