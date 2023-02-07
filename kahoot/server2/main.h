#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <error.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <poll.h> 
#include <unordered_set>
#include <list>
#include <signal.h>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
using namespace std::this_thread;
using namespace std::chrono;

uint16_t serverPort = 2115; /// server port
int servFd;                 /// server socket fd

/// data used in poll
int descrCapacity = 16;
int descrCount = 1;
pollfd * descr;

void ctrl_c(int);
void setReuseAddr(int sock);

void eventOnServFd(int revents);
void eventOnClientFd(int indexInDescr);

void interpretMsg(int clientFd, char * buffer, int bSize);
void sendToAllInRoom(std::string msg, int roomCode);

struct question{
    int num;
    int time;
    std::string content;
    std::string correct;
    std::string ansA;
    std::string ansB;
    std::string ansC;
    std::string ansD;
};
std::vector<question> quiz;

void startGame(int roomCode);