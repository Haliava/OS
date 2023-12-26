#include "../include/utils.h"
#define CHILD_NAME_LOWER "../build/child1"
#define CHILD_NAME_REPLACE "../build/child2"
#define SHARED_MEMORY_FILENAME "/mmap_file"
#define MAX_BUFFER_SIZE 1024
#define SEMAPHORE_COUNT 3

int main() {
    std::string semaphoreNameList[] = {"/main-lower", "/lower-replace", "/replace-main"};
    sem_t* semaphoreList[SEMAPHORE_COUNT];

    for (int i = 0; i < SEMAPHORE_COUNT; ++i) {
        semaphoreList[i] = semaphoreOpen(semaphoreNameList[i]);
    }

    int mmapFileDescriptor = openSharedMemoryFile(SHARED_MEMORY_FILENAME);

    pid_t pidLower = createProcess();
    if (pidLower == 0) {
        dup2FD(mmapFileDescriptor, STDIN_FILENO);
        execute(CHILD_NAME_LOWER, semaphoreNameList[0], semaphoreNameList[1]);
        exit(-1);
    }

    pid_t pidReplace = createProcess();
    if (pidReplace == 0) {
        dup2FD(mmapFileDescriptor, STDIN_FILENO);
        execute(CHILD_NAME_REPLACE, semaphoreNameList[1], semaphoreNameList[2]);
        exit(-1);
    }

    char userInput[MAX_BUFFER_SIZE];
    int inputLength;
    char* mp;

    char enter[20] = "Enter a string: \n";
    write(1, enter, 20);

    while ((inputLength = read(0, userInput, MAX_BUFFER_SIZE)) > 0) {
        setFileSize(mmapFileDescriptor, inputLength);
        mp = mmapCreate(inputLength, mmapFileDescriptor);

        for (int i = 0; i < inputLength; ++i) {
            mp[i] = userInput[i];
        }

        incrementSemaphore(semaphoreList[0]);
        sem_wait(semaphoreList[2]);

        char result_buffer[MAX_BUFFER_SIZE];
        for (int i = 0; i <= MAX_BUFFER_SIZE; i++) {
            result_buffer[i] = mp[i];
        }

        printf("Result:\n");
        printf("%s\n", result_buffer);
    }

    unlink(SHARED_MEMORY_FILENAME);

    killProcess(pidLower);
    killProcess(pidReplace);

    for (int i = 0; i < SEMAPHORE_COUNT; ++i) {
        sem_close(semaphoreList[i]);
        sem_unlink(semaphoreNameList[i].c_str());
    }

    return 0;
}