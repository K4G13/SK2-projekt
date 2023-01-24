#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QtWidgets>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:    
    quint16 port = 2115;

    bool gameInProgress = false;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void connectBtnHit();
    void socketConnected();
    void socketDisconnected();
    void socketError(QTcpSocket::SocketError err);
    void socketReadable();

    void setNick();
    void createRoom();
    void joinRoom();
    void startGame();
    void displaQuestion(QString str);
    void sendAnswearA();
    void sendAnswearB();
    void sendAnswearC();
    void sendAnswearD();
    void sumUp(QString str);

protected:
    QTcpSocket * sock {nullptr};
    QTimer * connTimeoutTimer{nullptr};

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
