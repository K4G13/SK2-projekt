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
#include <vector>


class Room{

    public:
        int code;
        int ownerFd;
        int currentQuestion;

    Room(int ownerFd);

};

std::vector<Room> rooms;

Room::Room(int ownerFd){
    this->code = 0000;
    this->ownerFd = ownerFd;
    this->currentQuestion = 0;

    int minC = 1000;
    int maxC = 9999;

    if( int(rooms.size()) < maxC - minC +1 )  {
        srand(time(0));
        int code; 

        bool unique = false;
        while(!unique){

            code = minC+ (rand() % (maxC-minC + 1));
            unique = true;

            for(int i=0;i<int(rooms.size());i++){
                if(rooms[i].code==code){unique=false;break;}
            }
        }
        this->code=code;
    }
}

void listRooms(){
    printf(" | > list rooms:\n");
    for(Room r: rooms){ 
        printf("   + room%d owner: [%d]\n", r.code, r.ownerFd);
    }
}
int getIndexOfRoomInRooms(int roomCode){
    for(int i=0; i<int(rooms.size()); i++){
        if(rooms[i].code == roomCode) return i;
    }
    return -1;
}