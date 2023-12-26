#include <iostream>
#include <random>
#include <queue>
#include <atomic>

#define TOTAL_POINTS 1000
#define THREAD_COUNT 8

using namespace std;

typedef struct GeneratePointTask {
    int R;
} GeneratePointTask;

int taskCount = 0;
queue<GeneratePointTask> taskQueue;
std::atomic<int> pointsHit(0);
std::atomic<int> pointsGenerated(0);
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

void generatePoint(GeneratePointTask* params) {
    ++pointsGenerated;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(-params->R, params->R);

    int pointX = dist(gen);
    int pointY = dist(gen);
    if (pointX * pointX + pointY * pointY <= params->R * params->R)
        ++pointsHit;
    cout << "(" << pointX << ";" << pointY << ") hits -> " << pointsHit << endl;
}

void addTask(GeneratePointTask task) {
    pthread_mutex_lock(&mutexQueue);
    taskQueue.push(task);
    taskCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void* startThread(void* args) {
    while (true) {
        if (pointsGenerated >= TOTAL_POINTS)
            break;

        pthread_mutex_lock(&mutexQueue);
        while (taskCount == 0)
            pthread_cond_wait(&condQueue, &mutexQueue);

        GeneratePointTask task = taskQueue.front();
        taskQueue.pop();
        taskCount--;
        pthread_mutex_unlock(&mutexQueue);

        generatePoint(&task);
    }
}

int main() {
    int R;
    cin >> R;
    int RECTANGLE_AREA = 4 * R * R;

    pthread_t threads[THREAD_COUNT];
    pthread_cond_init(&condQueue, NULL);
    pthread_mutex_init(&mutexQueue, NULL);
    for (int i = 0; i < THREAD_COUNT; ++i) {
        if (pthread_create(&threads[i], NULL, &startThread, NULL) != 0)
            perror("Failed to create a process");
    }

    for (int i = 0; i < TOTAL_POINTS; ++i) {
        GeneratePointTask task = {.R = R};
        addTask(task);
    }

    for (int i = 0; i < THREAD_COUNT; ++i) {
        if (pthread_join(threads[i], NULL) != 0)
            perror("Failed to join a process");
    }
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);

    cout << "APPROXIMATED CIRCLE AREA = " << RECTANGLE_AREA * pointsHit / TOTAL_POINTS << endl;
    cout << "CALCULATED CIRCLE AREA = " << numbers::pi * R * R << endl;

    return 0;
}