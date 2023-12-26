#include <iostream>
#include <thread>
#include <chrono>
#include <semaphore>


#include "definitions.hpp"
#include "functions.hpp"

/*
    TODO
    - Сейчас у меня при выходе не дожидается ответа
    - Добавить отчистку ресурсов после ошибки
    - Валидация порта и nodeId (createNewNode)
    - Нормальный путь сделать (createNewNode)
    - У меня никак не соединяется поток в воркере.
    - При ошибке все по пизде
    - Возможно, вычисления делать тоже в дргом потоке
*/

zmq::context_t context1;
zmq::context_t context2;

zmq::socket_t pullReplySocket(context1, zmq::socket_type::pull);
zmq::socket_t pushRequestSocket(context2, zmq::socket_type::push);

bool waitForNewRequest = true;

std::binary_semaphore endSemaphore(0);

void getReply() {
    while (waitForNewRequest) {
        Reply data = pullReply(pullReplySocket);
        if (data.operationType != OperationType::NOTHING) {

            if (data.error == ErrorTypes::STOP_BEFORE_START) {
                std::cout << "Error: " << data.id << ": Stop Before Start" << std::endl;
            } else if (data.subrequest == TimerSubrequest::START) {
                std::cout << "Ok: " << data.id << std::endl;
            } else if (data.subrequest == TimerSubrequest::STOP) {
                std::cout << "Ok: " << data.id << std::endl;
            } else if (data.subrequest == TimerSubrequest::TIME) {
                std::cout << "Ok: " << data.id << ": " << data.result << std::endl;
            }
            
        }
    }
    endSemaphore.release();
}

void updateWorkersCount(std::unordered_map<ssize_t, std::pair<pid_t, size_t>>& nodes, size_t& workersCount) {
    workersCount = 0;
    for (auto node : nodes) {
        workersCount += isProcessExists(node.second.first);
    }
}

int main() {
    size_t workerksCount = 0;
    /*
        Key: nodeId
        Value: <ProcessId, currentPort>
    */ 
    std::unordered_map<ssize_t, std::pair<pid_t, size_t>> nodes;

    size_t currentPort = MIN_DYNAMIC_PORT;
    Request request;
    
    pullReplySocket.bind(getAddres(currentPort + 0));
    pushRequestSocket.bind(getAddres(currentPort + 1));

    currentPort += 2;

    std::thread replyThread(getReply);

    while (waitForNewRequest) {
        request = readRequest();
        
        if (workerksCount && waitpid(-1, NULL, WNOHANG) == -1) {
            perror("Wait error (server)");
            exit(1);
        }
        updateWorkersCount(nodes, workerksCount);
        switch (request.operationType) {
        case OperationType::QUIT:
            waitForNewRequest = false;
            endSemaphore.acquire();
            break;
        case OperationType::EXEC:
            if (nodes.count(request.id) == 0) {
                std::cout << "Error: " << request.id << ": Not found" << std::endl;
            } else if (!isProcessExists(nodes[request.id].first)){
                std::cout << "Error: " << request.id << ": Node is unvaliable" << std::endl;
            } else {
                pushRequest(pushRequestSocket, request);
            }
            break;
        case OperationType::CREATE:
            ++workerksCount;
            updateNodeMap(nodes, currentPort, request);
            break;
        case OperationType::PING:
            if (nodes.count(request.id) == 0) {
                std::cout << "Error: " << request.id << ": Not found" << std::endl;
            } else {
                if (isProcessExists(nodes[request.id].first)) {
                    std::cout << "Ok: 1" << std::endl;
                } else {
                    std::cout << "Ok: 0" << std::endl;
                }
            }
            break;
        case OperationType::NOTHING:
            break;
        }
    }

    replyThread.join();
    pullReplySocket.close();
    pushRequestSocket.close();
    killWorkers(nodes);
    
    return 0;
}