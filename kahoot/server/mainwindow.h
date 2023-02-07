#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QtWidgets>
#include <QThread>
#include <QDebug>

struct question{
    int num;
    QString time;
    QString content;
    QString correct;
    QString ansA;
    QString ansB;
    QString ansC;
    QString ansD;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
protected:
    QTcpServer * servSock;

    class User : public QListWidgetItem {
        public:
            QTcpSocket * sock;
            MainWindow * partent;
            User(QTcpSocket * sock, MainWindow * parent);

            QString nick;
            int roomID = 0;
            int points = 0;

            QString toString(){return sock->peerAddress().toString()+":"+QString::number(sock->peerPort()) + "[ " + nick + " ]";}

            ~User(){sock->close();}
    };

    class Room : public QListWidgetItem{
        public:
            int roomID = 0;

            User * owner;
            int currQuestion = 0;
            QTimer * timer = nullptr;

            Room(int roomID);

            QString toString(){return QString::number(roomID);}
            int getRoomID(){return roomID;}

            ~Room(){timer->deleteLater();}
    };

    void socketReadable();

public:
    quint16 port = 2115;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void error(User* u, QTcpSocket::SocketError);

    void socketReadable(User * u);
    void createNewRoom(User * u);
    void joinRoom(User * u, QString roomID);
    void changeNick(User * u, QString str);

    void kickSelected();
    void showPlayersInRoom();

    void startGame(Room * r);
    void sendNextQuestion(Room * r);
    void checkAnswear(User * u, QString ans);
    void sumUpGame(Room * r);

    QList<question> quiz;


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
