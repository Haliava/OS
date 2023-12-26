#include "functions.hpp"

#include <iostream>
#include <sstream>

std::string getAddres(size_t port) {
    return std::string("tcp://127.0.0.1:") + std::to_string(port);
}

void setOperationType(Request &request, std::string &operationType) {
    if (operationType == "quit") {
        request.operationType = OperationType::QUIT;
    } else if (operationType == "create") {
        request.operationType = OperationType::CREATE;
    } else if (operationType == "exec") {
        request.operationType = OperationType::EXEC;
    } else if (operationType == "ping") {
        request.operationType = OperationType::PING;
    } else {
        std::cout << "Error: Invalid operation" << std::endl;
    }
}

void setId(Request &request, ssize_t id) {
    if (id < 0) {
        std::cout << "Error: " << id << " : Invalid worker node id" << std::endl;
    }
    request.id = id;
}

void validateSubrequest(Request &request, std::string &timerSubrequest) {
    using std::invalid_argument;
    if (request.operationType == OperationType::PING && !timerSubrequest.empty()) {
        std::cout << "Error: Ping can't accept third argument" << std::endl;
    } else if (request.operationType == OperationType::CREATE && timerSubrequest != "-1") {
        std::cout << "Error: Create can push only in end" << std::endl;
    } else if (request.operationType == OperationType::PING || request.operationType == OperationType::CREATE) {
        request.subrequest = TimerSubrequest::NOTHING;
    }

    if (request.operationType == OperationType::EXEC && timerSubrequest == "start") {
        request.subrequest = TimerSubrequest::START;
    } else if (request.operationType == OperationType::EXEC && timerSubrequest == "stop") {
        request.subrequest = TimerSubrequest::STOP;
    } else if (request.operationType == OperationType::EXEC && timerSubrequest == "time") {
        request.subrequest = TimerSubrequest::TIME;
    } else if (request.operationType == OperationType::EXEC) {
        std::cout << "Error: Invalid subrequest" << std::endl;
    }
}

Request readRequest() {
    using std::cout, std::cin, std::endl, std::string, std::stringstream, std::getline, std::invalid_argument;

    string operation;
    Request request;

    string operationType;
    size_t id = -1;
    string timerSubrequest;

    getline(cin, operation);
    stringstream ss(operation);

    ss >> operationType;
    setOperationType(request, operationType);
    if (request.operationType == OperationType::QUIT) {
        request.id = -1;
        request.subrequest = TimerSubrequest::NOTHING;
        return request;
    }

    ss >> id;
    setId(request, id);
    ss >> timerSubrequest;
    validateSubrequest(request, timerSubrequest);

    return request;
}

// MESSAGE SENDER/PULLER --------------------------------------------------------------
void pushRequest(zmq::socket_t &socket, Request &request) {
    zmq::message_t message(&request, sizeof(Request));

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-result"
    socket.send(message, zmq::send_flags::none);
    #pragma GCC diagnostic pop
}

void pushReply(zmq::socket_t &socket, Reply &reply) {
    zmq::message_t message(&reply, sizeof(Reply));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    socket.send(message, zmq::send_flags::none);
#pragma GCC diagnostic pop
}

Reply pullReply(zmq::socket_t &socket) {
    zmq::message_t message(sizeof(Reply));

    auto reply = socket.recv(message, zmq::recv_flags::dontwait);
    if (reply.has_value()) {
        return *message.data<Reply>();
    }
    return Reply{OperationType::NOTHING, -1, ErrorTypes::NO_ERRORS, -1, TimerSubrequest::NOTHING};
}

Request pullRequest(zmq::socket_t &socket) {
    zmq::message_t message(sizeof(Request));

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-result"
    socket.recv(message, zmq::recv_flags::none);
    #pragma GCC diagnostic pop

    return *message.data<Request>();
}


std::pair<pid_t, bool> createNewNode(
        std::unordered_map <ssize_t, std::pair<pid_t, size_t>> &nodes, ssize_t nodeId, size_t &currentPort
) {
    bool replace = false;

    if (nodes.count(nodeId) > 0) {
        std::cout << "Error: Already exists" << std::endl;
        return std::pair < pid_t, bool > {PID_FOR_ALREADY_EXIST_NODE, replace};
    }

    size_t currentPortCopy = currentPort;
    for (auto &node: nodes) {
        if (!isProcessExists(node.second.first) && node.second.second != ALREADY_REPLACED) {
            currentPortCopy = node.second.second;
            node.second.second = ALREADY_REPLACED;
            replace = true;
            break;
        }
    }

    pid_t processId = fork();
    if (processId == -1) {
        perror("Fork error (server)");
        exit(1);
    }

    if (processId == 0) {
        char srtNodeId[MAX_NODE_ID_LENGTH + 1];
        char strCurrentPort[MAX_PORT_LENGTH + 1];
        sprintf(srtNodeId, "%zd", nodeId);
        sprintf(strCurrentPort, "%zu", currentPortCopy);
        if (execl("./build/worker_exe", "./build/worker_exe", srtNodeId, strCurrentPort, NULL) == -1) {
            perror("Exec error (server)");
            exit(1);
        }
    }

    std::cout << "Ok: " << processId << std::endl;
    return std::pair < pid_t, bool > {processId, replace};
}

void updateNodeMap(std::unordered_map <ssize_t, std::pair<pid_t, size_t>> &nodes,
                   size_t &currentPort, Request &request) {
    std::pair<pid_t, bool> newWorkerInfo = createNewNode(nodes, request.id, currentPort);
    if (newWorkerInfo.first != PID_FOR_ALREADY_EXIST_NODE) {
        nodes[request.id] = std::pair<pid_t, size_t> {newWorkerInfo.first, currentPort};

        if (!newWorkerInfo.second) {
            currentPort += 2;
        }
    }
}

void killWorkers(std::unordered_map <ssize_t, std::pair<pid_t, size_t>> &nodes) {
    for (auto worker: nodes) {
        kill(worker.second.first, SIGTERM);
    }
}

bool isProcessExists(pid_t pid) {
    int errnoBefore = errno;
    kill(pid, 0);
    int newErrno = errno;
    errno = errnoBefore;
    return newErrno != ESRCH;
}