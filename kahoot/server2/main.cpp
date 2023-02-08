#include "main.h"
#include "user.cpp"

std::vector<std::thread> gameThreads;

int main(int argc, char ** argv){

    servFd = socket(AF_INET, SOCK_STREAM, 0);
    if(servFd == -1) error(1, errno, "socket failed");

    signal(SIGINT, ctrl_c);
    signal(SIGPIPE, SIG_IGN);

    setReuseAddr(servFd);

    sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)serverPort), .sin_addr={INADDR_ANY}};
    int res = bind(servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
    if(res) error(1, errno, "bind failed");

    res = listen(servFd, 1);
    if(res) error(1, errno, "listen failed");

    descr = (pollfd*) malloc(sizeof(pollfd)*descrCapacity);
    
    descr[0].fd = servFd;
    descr[0].events = POLLIN;


    std::fstream f;
    f.open("quiz.txt",std::ios::in);
    if (f.is_open()){
        std::string line;
        while(getline(f, line)){ 

            question newQ;

            std::string title[4]; int t=0;
            std::string str;
            for(int i=0;i<int(line.size());i++){
                if(line[i]==';') {title[t]=str; t++; str="";}
                else str += line[i];
            }
            title[t]=str;

            newQ.num = std::stoi(title[0]);
            newQ.time = std::stoi(title[1]);
            newQ.content = title[2];
            newQ.correct = title[3];

            getline(f, line); newQ.ansA = line;
            getline(f, line); newQ.ansB = line;
            getline(f, line); newQ.ansC = line;
            getline(f, line); newQ.ansD = line;

            quiz.push_back(newQ);

        }
        f.close();
   }
    else{
        printf(" | ! can not open file quiz.txt\n");
        return 0;
    }

    while(true){

        int ready = poll(descr, descrCount, -1);
        if(ready == -1){
            error(0, errno, "poll failed");
            ctrl_c(SIGINT);
        }
        
        for(int i = 0 ; i < descrCount && ready > 0 ; ++i){
            if(descr[i].revents){
                if(descr[i].fd == servFd)
                    eventOnServFd(descr[i].revents);
                else
                    eventOnClientFd(i);
                ready--;
            }
        }

    }

    return 0;
}

void ctrl_c(int){
    printf("\n | > closing server...\n");

    for(int i = 1 ; i < descrCount; ++i){
        shutdown(descr[i].fd, SHUT_RDWR);
        close(descr[i].fd);
    }

    close(servFd);

    exit(0);
}

void setReuseAddr(int sock){
    const int one = 1;
    int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if(res) error(1,errno, "setsockopt failed");
}

void eventOnServFd(int revents) {
    if(revents & ~POLLIN){
        error(0, errno, "Event %x on server socket", revents);
        ctrl_c(SIGINT);
    }
    
    if(revents & POLLIN){
        sockaddr_in clientAddr{};
        socklen_t clientAddrSize = sizeof(clientAddr);
        
        auto clientFd = accept(servFd, (sockaddr*) &clientAddr, &clientAddrSize);
        if(clientFd == -1) error(1, errno, "accept failed");
        
        if(descrCount == descrCapacity){
            descrCapacity<<=1;
            descr = (pollfd*) realloc(descr, sizeof(pollfd)*descrCapacity);
        }
        
        descr[descrCount].fd = clientFd;
        descr[descrCount].events = POLLIN|POLLRDHUP;
        descrCount++;
        
        printf(" | > new connection from: %s:%hu fd[%d]\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), clientFd);

        User u(clientFd);
        u.clientAddr = clientAddr;
        users.push_back(u);
    }
}

void eventOnClientFd(int indexInDescr) {
    auto clientFd = descr[indexInDescr].fd;
    auto revents = descr[indexInDescr].revents;
    
    if(revents & POLLIN){
        char buffer[255];
        int count = read(clientFd, buffer, 255);
        if(count < 1)
            revents |= POLLERR;
        else{
            interpretMsg(clientFd, buffer,count);
            memset(buffer,0,255);   
        }
    }
    
    if(revents & ~POLLIN){
        printf(" | > removing fd[%d]\n", clientFd);   

        delUser(clientFd);       
              
        descr[indexInDescr] = descr[descrCount-1];
        descrCount--;
        
        shutdown(clientFd, SHUT_RDWR);
        close(clientFd);
    }
}

void interpretMsg(int clientFd, char * buffer, int bSize){

    /*buffer[bSize]='\0';
    printf(" | > new message[%d]: %s",clientFd,buffer); 
    if(int(buffer[bSize-1])!=10)printf("\n");*/

    std::string comm;
    for(int i=0;i<bSize;i++){
        comm+=buffer[i];
        if(buffer[i]==';'){

            /// listing users
            if(comm=="l;") listUsers(0);
            else if(comm.size()>=3 && comm.substr(0,2)=="l ") listUsers( std::stoi( comm.substr(2,comm.size()-3) ) );
            /// listing rooms 
            else if(comm=="r;") listRooms();
            /// CREATE ROOM
            else if(comm == "create room;"){
                Room r(clientFd);
                if( !r.code ){
                    printf(" | ! ERROR: CREATING ROOM\n" );             
                    char response[] = "cant create room;";
                    write(clientFd, response, 18);
                }
                else if( users[getIndexOfUserInUsers(clientFd)].roomCode ){
                    printf(" | ! user already in room\n" ); 
                }
                else{
                    rooms.push_back(r);
                    users[getIndexOfUserInUsers(clientFd)].roomCode = r.code;
                    printf( " | > created new room [%d]\n",r.code );
                    std::string res = "your room code " + std::to_string(r.code) + ";";
                    char* response = new char[res.size() + 1];
                    strcpy(response, res.c_str());
                    write(clientFd, response, res.size());
                    delete response;
                }
            }
            /// JOIN
            else if(comm.size()>=6 && comm.substr(0,5)=="join "){

                int code = 0;
                try { code = std::stoi( comm.substr(5,comm.size()-6) ); } 
                catch (...) {
                    printf(" | ! room doesnt exist\n");
                    char response[] = "room doesnt exist;";
                    write(clientFd, response, 18);
                } 
                if(code){
                    bool roomExist = false;
                    for(Room r: rooms)
                        if(r.code == code)roomExist = true;
                    if(roomExist){
                        int uIndex = getIndexOfUserInUsers(clientFd);                        
                        users[uIndex].roomCode = code;

                         printf(" | ! [%d] joined room%d\n", clientFd, code);
                        
                         char response[] = "joined room;";
                         write(clientFd, response, 12);
                    }
                    else{
                        printf(" | ! room doesnt exist\n");
                        char response[] = "room doesnt exist;";
                        write(clientFd, response, 18);
                    }
                }

            }
            /// NICK
            else if(comm.size()>=6 && comm.substr(0,5)=="nick "){
                
                int uIndex = getIndexOfUserInUsers(clientFd);

                if(comm.substr(5,comm.size()-6) == ""){      
                    printf(" | > setting default nick for [%d]\n",clientFd);   
                    users[uIndex].nick = "user"+std::to_string(users[uIndex].fd-3); 
                    std::string res = "your nick " + users[uIndex].nick + ";";
                    char* response = new char[res.size() + 1];
                    strcpy(response, res.c_str());
                    write(clientFd, response, res.size());
                    delete response;
                }
                else{
                    printf(" | > change nick for [%d]\n",clientFd);  
                    users[uIndex].nick = comm.substr(5,comm.size()-6);
                }

            }
            /// START GAME
            else if(comm.size()>=12 && comm.substr(0,11)=="start game "){                

                int code = 0;
                try { code = std::stoi( comm.substr(11,comm.size()-12) ); } 
                catch (...) {
                    printf(" | ! can not start game, wrong code\n");
                } 
                if(code && std::to_string(code).size() == 4 && getIndexOfRoomInRooms(code)!=-1){  
                    if( rooms[getIndexOfRoomInRooms(code)].gameStarted )
                        printf(" | ! game have already started\n");
                    else
                        gameThreads.push_back(std::thread(startGame,code));
                }
                else printf(" | ! can not start game, wrong code\n");

            }                            
            ///ANSWEAR
            else if(comm.size()==10 && comm.substr(0,8)=="answear "){          
                char ans = comm[8];
                int rIndex = getIndexOfRoomInRooms(users[getIndexOfUserInUsers(clientFd)].roomCode);
                if( rIndex != -1 && rooms[rIndex].gameStarted && quiz[rooms[rIndex].currentQuestion-1].correct[0] == ans ){
                    std::time_t endTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();//std::time(0);
                    long int t = static_cast<long int> ( endTime - rooms[rIndex].startTime );                    
                    users[getIndexOfUserInUsers(clientFd)].points += quiz[rooms[rIndex].currentQuestion].time - t;
                    //printf("> czas odpowiedzi: %ld\n",t);
                }
            }
            /// unknown command - ignore 
            else printf(" ? %s\n",comm.c_str());

            comm="";

        }
    }

}

void sendToAllInRoom(std::string msg, int roomCode){

    for(User u: users){ 
        if( u.roomCode == roomCode ){
            char* response = new char[msg.size() + 1];
            strcpy(response, msg.c_str());
            write(u.fd, response, msg.size());
            delete response;
        }            
    }                    

}

void startGame(int roomCode){      

    int rIndex = getIndexOfRoomInRooms(roomCode);      
    rooms[rIndex].gameStarted = true;

    printf(" | > STARTING GAME FOR ROOM %d\n",roomCode);   
    sendToAllInRoom("starting game;",roomCode);

    for( int i=0; i<int(quiz.size()); i++ ){

        std::time_t startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();//std::time(0);
        rooms[rIndex].startTime = startTime;

        sendToAllInRoom( 
            "question "+
            std::to_string(quiz[i].num)+"|"
            +std::to_string(quiz[i].time)+"|"
            +quiz[i].content+"|"
            +quiz[i].ansA+"|"
            +quiz[i].ansB+"|"
            +quiz[i].ansC+"|"
            +quiz[i].ansD+";"
            ,roomCode); 
        
        rooms[getIndexOfRoomInRooms(roomCode)].currentQuestion++;

        sleep_for(milliseconds( quiz[i].time ));
        //sleep_for(milliseconds( 2000 ));

    }

    rooms.erase(rooms.begin() + rIndex);   
    for(User u: users){
        if(u.roomCode==roomCode) {
            users[getIndexOfUserInUsers(u.fd)].roomCode = 0;
            users[getIndexOfUserInUsers(u.fd)].points = 0;
            std::string msg = "sumup " + std::to_string(u.points) +";";
            char* response = new char[msg.size() + 1];
            strcpy(response, msg.c_str());
            write(u.fd, response, msg.size());
            delete response;
        }
    }

    printf(" | > GAME ENDED IN ROOM %d\n",roomCode);   

    return;
}
