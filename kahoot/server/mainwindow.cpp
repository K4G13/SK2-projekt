#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->log->setReadOnly(true);


    servSock = new QTcpServer(this);
    if(!servSock->listen(QHostAddress::AnyIPv4, port)){
        QMessageBox::critical(this, "Error", "Could not bind or listen: " + servSock->errorString());
        QApplication::exit(1);
    }


    connect(servSock, &QTcpServer::newConnection, [&]{
        QString str = "connected to KAHOOT";
        QByteArray wellcomeMsg = (str+"\n").toUtf8();
        do {
            User * u = new User(servSock->nextPendingConnection(), this);

            u->sock->write(wellcomeMsg);
            ui->log->append(u->toString()+" connected to server.");
            ui->users->addItem(u);

        } while(servSock->hasPendingConnections());
    });

    connect(ui->showPlayers, &QPushButton::clicked, this, &MainWindow::showPlayersInRoom);
    connect(ui->kick, &QPushButton::clicked, this, &MainWindow::kickSelected);



    QFile file("quiz.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Could not open quiz.txt");
        QApplication::exit(1);
    }
    QTextStream in(&file);
    while(!in.atEnd()) {

        question newQuestion;

        QString line = in.readLine();
        QStringList splited = line.split(';');
        newQuestion.num = splited[0].toInt();
        newQuestion.time = splited[1];
        newQuestion.content = splited[2];
        newQuestion.correct = splited[3];

        line = in.readLine();
        newQuestion.ansA = line;

        line = in.readLine();
        newQuestion.ansB = line;

        line = in.readLine();
        newQuestion.ansC = line;

        line = in.readLine();
        newQuestion.ansD = line;

        quiz.append(newQuestion);
    }
    file.close();

    /*for(int i=0; i<quiz.length(); i++){
        ui->log->append(quiz[i].content);
        ui->log->append("  "+quiz[i].ansA);
        ui->log->append("  "+quiz[i].ansB);
        ui->log->append("  "+quiz[i].ansC);
        ui->log->append("  "+quiz[i].ansD);
    }*/

}

MainWindow::User::User(QTcpSocket * sock, MainWindow * parent) :  QListWidgetItem(), sock(sock) {
    connect(sock, &QTcpSocket::errorOccurred, [=,this](QTcpSocket::SocketError err){ parent->error(this, err);});
    connect(sock, &QTcpSocket::readyRead, [=,this]{ parent->socketReadable(this);});
    setText(toString());
}

MainWindow::Room::Room(int roomID) : QListWidgetItem(){
    this -> roomID = roomID;
    setText(toString());
}

void MainWindow::error(User* u, QTcpSocket::SocketError){
    ui->users->removeItemWidget(u);
    ui->log->append(u->toString()+" disconnected...");


    Room * r;
    for(int i = 0 ; i < ui->rooms->count(); ++i){ r = reinterpret_cast<Room*>(ui->rooms->item(i)); if( r->roomID == u->roomID ) break; }
    if(r){

        int nrPlayers = 0;
        for(int i = 0 ; i < ui->users->count(); ++i){
            User * u2 = reinterpret_cast<User*>(ui->users->item(i));
            if( u2->roomID == r->roomID && u2 != u ) nrPlayers++;
        }

        if(nrPlayers>=1 && u == r->owner){
            for(int i = 0 ; i < ui->users->count(); ++i){
                User * u2 = reinterpret_cast<User*>(ui->users->item(i));
                if( u2->roomID == r->roomID && u2 != u ) {
                    r->owner = u2;

                    QByteArray msg = QString("you are room owner;").toUtf8();
                    u2->sock->write(msg);

                    break;
                }
            }
        }
        else if(nrPlayers == 0){
            if(r->timer){r->timer->stop(); r->timer->deleteLater();} delete r;
        }

    }


    u->sock->close();
    delete u;
}

void MainWindow::socketReadable(User * u){

    QByteArray ba = u->sock->readAll();
    QString msg = QString::fromUtf8(ba).trimmed();
    QStringList commands = msg.split(';');

    for( int i=0; i<commands.size(); i++ ){
        if(commands[i]=="create room") createNewRoom(u);
        else if(commands[i].size()==9 && commands[i].mid(0,5)=="join ") joinRoom(u, commands[i].mid(5,4/*4 digit codes*/));
        else if(commands[i].size()!=9 && commands[i].mid(0,5)=="join ") {QByteArray msg = QString("room doesnt exist;").toUtf8(); u->sock->write(msg); }
        else if(commands[i].size() >= 5 && commands[i].mid(0,5)=="nick ") changeNick(u,commands[i].mid(5,commands[i].size()-5));
        else if(commands[i].size()==15 && commands[i].mid(0,11)=="start game ") {
            QString roomCode = commands[i].mid(11,4);
            for(int i = 0 ; i < ui->rooms->count(); ++i){
                Room * r = reinterpret_cast<Room*>(ui->rooms->item(i));
                if( r->toString() == roomCode ) startGame(r);
            }
        }
        else if(commands[i].size()==9 && commands[i].mid(0,8)=="answear ") checkAnswear(u,commands[i].mid(8,1));
        else if(commands[i].size()) ui->log->append( u->toString() + " | <b>" +commands[i]+"</b>" );
    }



}

void MainWindow::createNewRoom(User * u){
    int minID = 1000; int maxID = 9999+1;
    int randomID = QRandomGenerator::global()->bounded(minID,maxID);

    if( ui->rooms->count() >= maxID-minID ){
        ui->log->append("Too many rooms. Cant create new one!!!");
        QByteArray msg = QString("cant create room;").toUtf8();
        u->sock->write(msg);
    }
    else{
        for(int i = 0 ; i < ui->rooms->count(); ++i){
            Room * r = reinterpret_cast<Room*>(ui->rooms->item(i));
            if( randomID == r->getRoomID() ){
                randomID = QRandomGenerator::global()->bounded(minID,maxID);
                i = 0;
            }
        }
        Room * r = new Room(randomID);

        r->owner = u;
        ui->rooms->addItem(r);
        ui->log->append("created [room " + r->toString() + "] for " + u->toString());

        u->roomID = r->roomID;        

        QByteArray msg = QString("your room code "+ r->toString() +";").toUtf8();
        u->sock->write(msg);
    }
}

void MainWindow::joinRoom(User * u, QString roomID){

    //QString roomID = msg.mid(5,4/* <4> <- digits in room id */);

    bool roomExist = false;
    for(int i = 0 ; i < ui->rooms->count(); ++i){
        Room * r = reinterpret_cast<Room*>(ui->rooms->item(i));
        if( roomID.toInt() == r->getRoomID() ){
            roomExist = true;
            break;
        }
    }



    if(roomExist) {
        ui->log->append(u->toString()+" joined to room "+roomID);
        u->roomID = roomID.toInt();

        QByteArray msg = QString("joined room;").toUtf8();
        u->sock->write(msg);
    }
    else{
        QByteArray msg = QString("room doesnt exist;").toUtf8();
        u->sock->write(msg);
    }
}

void MainWindow::changeNick(User * u, QString str){

    if(str=="")str = "user"+QString::number(u->sock->peerPort()) ;

    for(int i = 0 ; i < ui->users->count(); ++i){
        User * u2 = reinterpret_cast<User*>(ui->users->item(i));
        if( u2->nick == str ) str = str + "(2)";
    }

    u->nick = str;
    u->setText(u->toString());

    QByteArray msg = QString("your nick "+str+";").toUtf8();
    u->sock->write(msg);
}

void MainWindow::kickSelected(){
    for(auto i : ui->users->selectedItems()){
        User * u = reinterpret_cast<User*>(i);
        ui->users->removeItemWidget(u);
        ui->log->append(u->toString()+" kicked...");


        ///deleting room or changing room owner
        Room * r;
        for(int i = 0 ; i < ui->rooms->count(); ++i){ r = reinterpret_cast<Room*>(ui->rooms->item(i)); if( r->roomID == u->roomID ) break; }
        if(r){

            int nrPlayers = 0;
            for(int i = 0 ; i < ui->users->count(); ++i){
                User * u2 = reinterpret_cast<User*>(ui->users->item(i));
                if( u2->roomID == r->roomID && u2 != u ) nrPlayers++;
            }

            if(nrPlayers>=1 && u == r->owner){
                for(int i = 0 ; i < ui->users->count(); ++i){
                    User * u2 = reinterpret_cast<User*>(ui->users->item(i));
                    if( u2->roomID == r->roomID && u2 != u ) {
                        r->owner = u2;

                        QByteArray msg = QString("you are room owner;").toUtf8();
                        u2->sock->write(msg);

                        break;
                    }
                }
            }
            else if(nrPlayers == 0){
                if(r->timer){r->timer->stop(); r->timer->deleteLater();} delete r;
            }

        }


        u->sock->close();
        delete u;
    }
}

void MainWindow::showPlayersInRoom(){
    for(auto i : ui->rooms->selectedItems()){
        Room * r = reinterpret_cast<Room*>(i);
        ui->log->append("ROOM " + r->toString());

        for(int i = 0 ; i < ui->users->count(); ++i){
            User * u = reinterpret_cast<User*>(ui->users->item(i));
            if( r->roomID == u->roomID )
                ui->log->append(" - " + u->toString());
        }

    }
}

void MainWindow::startGame(Room * r){

       ui->log->append("<b>Starting game for room "+ r->toString() + " owned by "+ r->owner->toString() +"</b>");

       QByteArray msg = QString("starting game;").toUtf8();
       for(int i = 0 ; i < ui->users->count(); ++i){
           User * u = reinterpret_cast<User*>(ui->users->item(i));
           if(u->roomID == r->roomID)u->sock->write(msg);
       }


       r->timer = new QTimer;
       //timer->setSingleShot(true);
       connect(r->timer, &QTimer::timeout, [=]{

           if(r->currQuestion < quiz.size() ){
               r->timer->start(quiz[r->currQuestion].time.toInt());
               sendNextQuestion(r);
           }
           else{
               r->timer->stop();
               r->timer->deleteLater();
               r->timer=nullptr;
               sumUpGame(r);
           }

       });
       r->timer->start(quiz[r->currQuestion].time.toInt());
       sendNextQuestion(r);
}

void MainWindow::sendNextQuestion(Room * r){


    question q = quiz[r->currQuestion];

    QByteArray msg = QString("question "+QString::number(q.num)+"|"+q.time+"|"+q.content+"|"+q.ansA+"|"+q.ansB+"|"+q.ansC+"|"+q.ansD+";").toUtf8();
    for(int i = 0 ; i < ui->users->count(); ++i){
        User * u = reinterpret_cast<User*>(ui->users->item(i));
        if(u->roomID == r->roomID)u->sock->write(msg);
    }
    r->currQuestion++;

}

void MainWindow::checkAnswear(User * u, QString ans){

    int questionNr;
    for(int i = 0 ; i < ui->rooms->count(); ++i){
        Room * r = reinterpret_cast<Room*>(ui->rooms->item(i));
        if( r->roomID == u->roomID ){
            questionNr = r->currQuestion - 1;
            break;
        }
    }

   /// ui->log->append( quiz[questionNr].correct + "|" + ans );
    //if( quiz[questionNr].correct == ans ) u->points ++ ;

    if( quiz[questionNr].correct == ans ){
        Room * r;
        for(int i = 0 ; i < ui->rooms->count(); ++i){ r = reinterpret_cast<Room*>(ui->rooms->item(i)); if( r->roomID == u->roomID ) break; }
        u->points += r->timer->remainingTime();
        QByteArray msg = QString("points "+QString::number(r->timer->remainingTime())+";").toUtf8();
        u->sock->write(msg);
    }

}

void MainWindow::sumUpGame(Room * r){

    ui->log->append("<b>Game ended in room "+r->toString()+"<b>");

    for(int i = 0 ; i < ui->users->count(); ++i){
        User * u = reinterpret_cast<User*>(ui->users->item(i));
        if( r->roomID == u->roomID ){

            ui->log->append( u->toString()+": "+QString::number(u->points)+"pts." );

            QByteArray msg = QString("sumup "+QString::number(u->points)+";").toUtf8();
            u->sock->write(msg);

            u->points = 0;
            u->roomID = 0;

        }
    }    

    ///delete room after game ends
    for(int i = 0 ; i < ui->rooms->count(); ++i){
        Room * r2 = reinterpret_cast<Room*>(ui->rooms->item(i));
        if( r2==r ) { if(r->timer)r->timer->stop(); delete r;}
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
