#include "../include/utils.h"
#define CHILD_NAME_LOWER "./bin/child1"
#define CHILD_NAME_REPLACE "./bin/child2"
#define MAX_BUFFER_SIZE 1024
#define SEMAFORE_COUNT 3

int main() {
    pid_t pid = getpid();
    std::cout << "This is process with pid: " << pid << std::endl;

    int fdLower[2];
    int fdReplace[2];
    int fdConnector[2];

    pipeFD(fdLower);
    pipeFD(fdReplace);
    pipeFD(fdConnector);

    int inLower = fdLower[0]; int outLower = fdLower[1];
    int inConnector = fdConnector[0]; int outConnector = fdConnector[1];
    int inReplace = fdReplace[0]; int outReplace = fdReplace[1];
    pid = createProcess();

    if (pid == 0) {
        closeFD(outLower);
        closeFD(inReplace);
        dup2FD(inLower, STDIN_FILENO);
        closeFD(inLower);
        closeFD(outReplace);

        dup2FD(outConnector, STDOUT_FILENO);
        execute(CHILD_NAME_LOWER);
    } else {
        closeFD(outConnector);
        closeFD(inReplace);

        pid_t pid2 = createProcess();
        if (pid2 == 0) {
            dup2FD(inConnector, STDIN_FILENO);
            dup2FD(outReplace, STDOUT_FILENO);
            execute(CHILD_NAME_REPLACE);
        }

        closeFD(inConnector);
        closeFD(inLower);
        closeFD(outLower);
        closeFD(outReplace);
    }

    std::string answer;
    char c;
    while (read(inReplace, &c, sizeof(char))) {
        answer += c;
    }

    std::cout << answer << std::endl;

    return 0;
}