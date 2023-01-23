#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){

    ui->setupUi(this);
    ui->gameGB->setEnabled(false);
    ui->bttA->setStyleSheet("background-color: #777");
    ui->bttB->setStyleSheet("background-color: #777");
    ui->bttC->setStyleSheet("background-color: #777");
    ui->bttD->setStyleSheet("background-color: #777");
    ui->display->setReadOnly(true);
    ui->log->setReadOnly(true);
    ui->nickGB->setEnabled(false);
    ui->createRoomGB->setEnabled(false);
    ui->roomCodeGB->setEnabled(false);

    connect(ui->joinServerBtt, &QPushButton::clicked, this, &MainWindow::connectBtnHit);

    connect(ui->createBtt, &QPushButton::clicked, this, &MainWindow::createRoom);
    connect(ui->joinBtt, &QPushButton::clicked, this, &MainWindow::joinRoom);
    connect(ui->startGameBtt, &QPushButton::clicked, this, &MainWindow::startGame);

    connect(ui->bttA, &QPushButton::clicked, this, &MainWindow::sendAnswearA);
    connect(ui->bttB, &QPushButton::clicked, this, &MainWindow::sendAnswearB);
    connect(ui->bttC, &QPushButton::clicked, this, &MainWindow::sendAnswearC);
    connect(ui->bttD, &QPushButton::clicked, this, &MainWindow::sendAnswearD);

}

void MainWindow::connectBtnHit(){
    ui->serverGB->setEnabled(false);
    ui->log->append("connecting to " + ui->serverAdress->text() + ":" + QString::number(2115));
    if(sock)
        delete sock;
    sock = new QTcpSocket(this);
    connTimeoutTimer = new QTimer(this);
    connTimeoutTimer->setSingleShot(true);
    connect(connTimeoutTimer, &QTimer::timeout, [&]{
        sock->abort();
        sock->deleteLater();
        sock = nullptr;        connTimeoutTimer->deleteLater();
        connTimeoutTimer=nullptr;

        ui->serverGB->setEnabled(true);
        ui->log->append("Connect timed out...");
        QMessageBox::critical(this, "Error", "Connect timed out");
    });



    connect(sock, &QTcpSocket::connected, this, &MainWindow::socketConnected);
    connect(sock, &QTcpSocket::disconnected, this, &MainWindow::socketDisconnected);
    connect(sock, &QTcpSocket::errorOccurred, this, &MainWindow::socketError);
    connect(sock, &QTcpSocket::readyRead, this, &MainWindow::socketReadable);

    sock->connectToHost(ui->serverAdress->text(), port);
    connTimeoutTimer->start(3000);

}

void MainWindow::socketConnected(){
    connTimeoutTimer->stop();
    connTimeoutTimer->deleteLater();
    connTimeoutTimer=nullptr;
    ui->nickGB->setEnabled(true);
    ui->createRoomGB->setEnabled(true);
    ui->roomCodeGB->setEnabled(true);
    ui->startGameBtt->setEnabled(false);
    ui->roomCode->setText("");
    //ui->log->append("connected!");
}

void MainWindow::socketDisconnected(){
    ui->log->append("disconnected! :((");

    ui->serverGB->setEnabled(true);
    ui->nickGB->setEnabled(false);
    ui->createRoomGB->setEnabled(false);
    ui->roomCodeGB->setEnabled(false);
    ui->createBtt->setEnabled(true);
    ui->gameGB->setEnabled(false);
    ui->bttA->setStyleSheet("background-color: #777");
    ui->bttB->setStyleSheet("background-color: #777");
    ui->bttC->setStyleSheet("background-color: #777");
    ui->bttD->setStyleSheet("background-color: #777");
}

void MainWindow::socketError(QTcpSocket::SocketError err){
    if(err == QTcpSocket::RemoteHostClosedError)
        return;
    if(connTimeoutTimer){
        connTimeoutTimer->stop();
        connTimeoutTimer->deleteLater();
        connTimeoutTimer=nullptr;
    }
    QMessageBox::critical(this, "Error", sock->errorString());
    ui->log->append("socket error: "+sock->errorString());
    ui->serverGB->setEnabled(true);
}

void MainWindow::socketReadable(){

    QByteArray ba = sock->readAll();
    QStringList commands = QString::fromUtf8(ba).trimmed().split(';');
    for( int i=0; i<commands.size(); i++ ){
        if(commands[i]=="cant create room") {
            QMessageBox::critical(this, "Error", "Can not create a new room.");
            ui->createRoomGB->setEnabled(true);
            ui->roomCodeGB->setEnabled(true);
            ui->nickGB->setEnabled(true);
        }
        else if(commands[i]=="room doesnt exist") {
            QMessageBox::critical(this, "Error", "Room doesnt exist. Try again.");
            ui->createRoomGB->setEnabled(true);
            ui->roomCodeGB->setEnabled(true);
            ui->nickGB->setEnabled(true);
        }
        else if(commands[i]=="joined room"){
            ui->log->append("Joined room :)");
        }
        else if(commands[i].size() == 19 && commands[i].mid(0,15)=="your room code "){
            ui->createRoomGB->setEnabled(true);
            ui->createBtt->setEnabled(false);
            ui->startGameBtt->setEnabled(true);
            ui->roomCode->setText(commands[i].mid(15,4));
        }
        else if(commands[i]=="starting game"){
            ui->gameGB->setEnabled(true);
            ui->bttA->setStyleSheet("background-color: #4285F4");
            ui->bttB->setStyleSheet("background-color: #34a853");
            ui->bttC->setStyleSheet("background-color: #fbbc05");
            ui->bttD->setStyleSheet("background-color: #ea4335");
        }
        else if(commands[i].mid(0,9)=="question ") displaQuestion(commands[i].mid(9,commands[i].size()-9));
        else if(commands[i].mid(0,6)=="sumup ") sumUp(commands[i].mid(6,commands[i].size()-6));
        else if(commands[i].size()) ui->log->append( "<b>" +commands[i]+"</b>" );
    }



}

void MainWindow::setNick(){
        QString str = "nick "+ui->nick->text()+";";
        QByteArray command = str.toUtf8();
        sock->write(command);
        ui->nickGB->setEnabled(false);
}

void MainWindow::createRoom(){

    QString str = "create room;";
    QByteArray command = str.toUtf8();
    sock->write(command);

    ui->createRoomGB->setEnabled(false);
    ui->roomCodeGB->setEnabled(false);

    setNick();
}

void MainWindow::joinRoom(){

    QString str = "join "+ui->roomCode->text()+";";

    int roomCodeInt = ui->roomCode->text().toInt();

    QByteArray command = str.toUtf8();
    sock->write(command);

    ui->createRoomGB->setEnabled(false);
    ui->roomCodeGB->setEnabled(false);

    setNick();
}

void MainWindow::startGame(){

    QString str = "start game "+ ui->roomCode->text() +";";
    QByteArray command = str.toUtf8();
    sock->write(command);

    ui->startGameBtt->setEnabled(false);
    ui->createRoomGB->setEnabled(false);
}

void MainWindow::displaQuestion(QString str){

    QStringList content = str.split("|");

    ui->display->setText(content[0]+") "+content[2]);
    ui->bttA->setText(content[3]);
    ui->bttB->setText(content[4]);
    ui->bttC->setText(content[5]);
    ui->bttD->setText(content[6]);

    ui->bttA->setEnabled(true);
    ui->bttB->setEnabled(true);
    ui->bttC->setEnabled(true);
    ui->bttD->setEnabled(true);

}

void MainWindow::sendAnswearA(){
    QString str = "answear A;";
    QByteArray command = str.toUtf8();
    sock->write(command);
    ui->bttA->setEnabled(false);
    ui->bttB->setEnabled(false);
    ui->bttC->setEnabled(false);
    ui->bttD->setEnabled(false);
}

void MainWindow::sendAnswearB(){
    QString str = "answear B;";
    QByteArray command = str.toUtf8();
    sock->write(command);
    ui->bttA->setEnabled(false);
    ui->bttB->setEnabled(false);
    ui->bttC->setEnabled(false);
    ui->bttD->setEnabled(false);
}

void MainWindow::sendAnswearC(){
    QString str = "answear C;";
    QByteArray command = str.toUtf8();
    sock->write(command);
    ui->bttA->setEnabled(false);
    ui->bttB->setEnabled(false);
    ui->bttC->setEnabled(false);
    ui->bttD->setEnabled(false);
}

void MainWindow::sendAnswearD(){
    QString str = "answear D;";
    QByteArray command = str.toUtf8();
    sock->write(command);
    ui->bttA->setEnabled(false);
    ui->bttB->setEnabled(false);
    ui->bttC->setEnabled(false);
    ui->bttD->setEnabled(false);
}

void MainWindow::sumUp(QString str){

    QStringList str2 = str.split("/");
    float procent = (str2[0].toFloat() / str2[1].toFloat() )*100;
    ui->display->setText("Udalo ci sie zdobyc "+str2[0]+"/"+str2[1]+" punktow("+ QString::number(procent) +"%)");

    ui->nickGB->setEnabled(true);
    ui->createRoomGB->setEnabled(true);
    ui->createBtt->setEnabled(true);
    ui->roomCodeGB->setEnabled(true);
    ui->gameGB->setEnabled(false);
    ui->bttA->setStyleSheet("background-color: #777");
    ui->bttB->setStyleSheet("background-color: #777");
    ui->bttC->setStyleSheet("background-color: #777");
    ui->bttD->setStyleSheet("background-color: #777");
}

MainWindow::~MainWindow()
{
    delete ui;
}

