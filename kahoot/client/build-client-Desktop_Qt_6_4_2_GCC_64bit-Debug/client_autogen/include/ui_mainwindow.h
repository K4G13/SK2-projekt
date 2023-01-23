/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *nickGB;
    QLineEdit *nick;
    QGroupBox *createRoomGB;
    QPushButton *createBtt;
    QPushButton *startGameBtt;
    QGroupBox *roomCodeGB;
    QLineEdit *roomCode;
    QPushButton *joinBtt;
    QGroupBox *serverGB;
    QLineEdit *serverAdress;
    QPushButton *joinServerBtt;
    QGroupBox *gameGB;
    QLineEdit *display;
    QPushButton *bttA;
    QPushButton *bttB;
    QPushButton *bttC;
    QPushButton *bttD;
    QTextEdit *log;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(513, 551);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        nickGB = new QGroupBox(centralwidget);
        nickGB->setObjectName("nickGB");
        nickGB->setGeometry(QRect(260, 10, 241, 71));
        nick = new QLineEdit(nickGB);
        nick->setObjectName("nick");
        nick->setGeometry(QRect(10, 30, 221, 31));
        createRoomGB = new QGroupBox(centralwidget);
        createRoomGB->setObjectName("createRoomGB");
        createRoomGB->setGeometry(QRect(260, 100, 241, 71));
        createBtt = new QPushButton(createRoomGB);
        createBtt->setObjectName("createBtt");
        createBtt->setGeometry(QRect(10, 30, 101, 31));
        startGameBtt = new QPushButton(createRoomGB);
        startGameBtt->setObjectName("startGameBtt");
        startGameBtt->setGeometry(QRect(118, 30, 111, 31));
        roomCodeGB = new QGroupBox(centralwidget);
        roomCodeGB->setObjectName("roomCodeGB");
        roomCodeGB->setGeometry(QRect(10, 100, 241, 71));
        roomCode = new QLineEdit(roomCodeGB);
        roomCode->setObjectName("roomCode");
        roomCode->setGeometry(QRect(10, 30, 113, 31));
        joinBtt = new QPushButton(roomCodeGB);
        joinBtt->setObjectName("joinBtt");
        joinBtt->setGeometry(QRect(130, 30, 101, 31));
        serverGB = new QGroupBox(centralwidget);
        serverGB->setObjectName("serverGB");
        serverGB->setGeometry(QRect(10, 10, 241, 71));
        serverAdress = new QLineEdit(serverGB);
        serverAdress->setObjectName("serverAdress");
        serverAdress->setGeometry(QRect(10, 30, 121, 31));
        joinServerBtt = new QPushButton(serverGB);
        joinServerBtt->setObjectName("joinServerBtt");
        joinServerBtt->setGeometry(QRect(140, 30, 91, 31));
        gameGB = new QGroupBox(centralwidget);
        gameGB->setObjectName("gameGB");
        gameGB->setGeometry(QRect(10, 180, 491, 301));
        display = new QLineEdit(gameGB);
        display->setObjectName("display");
        display->setGeometry(QRect(10, 30, 471, 81));
        QFont font;
        font.setPointSize(16);
        display->setFont(font);
        display->setAlignment(Qt::AlignCenter);
        bttA = new QPushButton(gameGB);
        bttA->setObjectName("bttA");
        bttA->setGeometry(QRect(10, 120, 231, 81));
        bttB = new QPushButton(gameGB);
        bttB->setObjectName("bttB");
        bttB->setGeometry(QRect(250, 120, 231, 81));
        bttC = new QPushButton(gameGB);
        bttC->setObjectName("bttC");
        bttC->setGeometry(QRect(10, 210, 231, 81));
        bttD = new QPushButton(gameGB);
        bttD->setObjectName("bttD");
        bttD->setGeometry(QRect(250, 210, 231, 81));
        log = new QTextEdit(centralwidget);
        log->setObjectName("log");
        log->setGeometry(QRect(200, 490, 301, 31));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 513, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "KAHOOT", nullptr));
        nickGB->setTitle(QCoreApplication::translate("MainWindow", "Your nick", nullptr));
        createRoomGB->setTitle(QCoreApplication::translate("MainWindow", "New room", nullptr));
        createBtt->setText(QCoreApplication::translate("MainWindow", "Create", nullptr));
        startGameBtt->setText(QCoreApplication::translate("MainWindow", "Start game", nullptr));
        roomCodeGB->setTitle(QCoreApplication::translate("MainWindow", "Room code", nullptr));
        joinBtt->setText(QCoreApplication::translate("MainWindow", "Join", nullptr));
        serverGB->setTitle(QCoreApplication::translate("MainWindow", "Server IP", nullptr));
        serverAdress->setText(QCoreApplication::translate("MainWindow", "localhost", nullptr));
        joinServerBtt->setText(QCoreApplication::translate("MainWindow", "Join server", nullptr));
        gameGB->setTitle(QCoreApplication::translate("MainWindow", "Game", nullptr));
        bttA->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        bttB->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        bttC->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        bttD->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
