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
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTextEdit *log;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QListWidget *rooms;
    QPushButton *showPlayers;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QListWidget *users;
    QPushButton *kick;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(513, 551);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        log = new QTextEdit(centralwidget);
        log->setObjectName("log");
        log->setGeometry(QRect(10, 290, 491, 221));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(360, 10, 141, 271));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName("verticalLayout");
        rooms = new QListWidget(groupBox);
        rooms->setObjectName("rooms");

        verticalLayout->addWidget(rooms);

        showPlayers = new QPushButton(groupBox);
        showPlayers->setObjectName("showPlayers");

        verticalLayout->addWidget(showPlayers);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(10, 10, 341, 271));
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setObjectName("verticalLayout_2");
        users = new QListWidget(groupBox_2);
        users->setObjectName("users");

        verticalLayout_2->addWidget(users);

        kick = new QPushButton(groupBox_2);
        kick->setObjectName("kick");

        verticalLayout_2->addWidget(kick);

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
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "KAHOOT server", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Rooms", nullptr));
        showPlayers->setText(QCoreApplication::translate("MainWindow", "show players", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "Users", nullptr));
        kick->setText(QCoreApplication::translate("MainWindow", "kick", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
