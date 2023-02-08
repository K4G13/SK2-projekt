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
#include "room.cpp"

class User{

    public:
        int fd;
        sockaddr_in clientAddr{};

        std::string nick;
        int roomCode;
        long int points;

    User(int clientFd);

};

std::vector<User> users;

User::User(int clientFd){
    this->fd = clientFd;
    this->nick = "user" + std::to_string(clientFd-3);
    this->roomCode = 0000;
    this->points = 0;
}

void listUsers(int roomCode){

    if(!roomCode){
        printf(" | > list users\n");
        printf("     <address>:<port> [<fd>] <nick> | <room>\n");
        for(User u: users){
            printf("   + %s:%hu [%d] %s | %d\n", inet_ntoa(u.clientAddr.sin_addr), ntohs(u.clientAddr.sin_port), u.fd, u.nick.c_str(), u.roomCode);
        }
    }

    else{
        printf(" | > list users in room %d\n", roomCode);
        printf("     <address>:<port> [<fd>] <nick>\n");
        for(User u: users){ 
            if( u.roomCode == roomCode )
                printf("   + %s:%hu [%d] %s\n", inet_ntoa(u.clientAddr.sin_addr), ntohs(u.clientAddr.sin_port), u.fd, u.nick.c_str());
        }
    }

}
int getIndexOfUserInUsers(int fileDescriptor){
    for( int i=0; i<int(users.size()); i++ ){
        //printf("T: fd:%d\n",i);
        if( users[i].fd == fileDescriptor ){
            return i;
        }
    }
    return -1;
} 
void delUser(int clientFd){

    int uIndex = getIndexOfUserInUsers(clientFd);
    if(uIndex!=-1){

        int rIndex = getIndexOfRoomInRooms(users[uIndex].roomCode);

        users.erase(users.begin() + uIndex);   

        ///CHECK IF USER WASNT A ROOM OWNER AND NUMBER OF PLAYERS IN ROOM
        if(rIndex != -1){
            bool isOwner = false; if( rooms[rIndex].ownerFd == clientFd ) isOwner = true;  

            int nrOfPlayers = 0;
            for(User u: users)
                if(u.roomCode == rooms[rIndex].code)nrOfPlayers++;

            if(nrOfPlayers>=1 && isOwner){
                nrOfPlayers--;
                printf(" | ! changing owner of room%d\n",rooms[rIndex].code); 
                for(User u: users)
                    if(u.roomCode == rooms[rIndex].code){ 
                        rooms[rIndex].ownerFd = u.fd; 
                        char response[] = "you are room owner;";
                        write(u.fd, response, 19);
                        break; }

            }
            else if(nrOfPlayers==0){
                printf(" | ! deleting room%d\n",rooms[rIndex].code); 
                rooms.erase(rooms.begin() + rIndex);  
            }
        } 


    } 
    else 
        printf(" | ! user doesnt exist!\n");

}   