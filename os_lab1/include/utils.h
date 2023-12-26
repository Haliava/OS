#ifndef OC_LAB1_UTILS_H
#define OC_LAB1_UTILS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>

void closeFD(int fd);
void dup2FD(int oldfd, int newfd);
void pipeFD(int *fd);
pid_t createProcess();
int openFile(const char *str);
void execute(const char *str1);

#endif //OC_LAB1_UTILS_H
