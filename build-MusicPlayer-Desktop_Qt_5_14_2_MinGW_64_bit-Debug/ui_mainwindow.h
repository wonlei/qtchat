/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnOpen;
    QLineEdit *editSearch;
    QPushButton *btnSearch;
    QListWidget *listWidgetSongs;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btnPrevious;
    QPushButton *btnPlay;
    QPushButton *btnNext;
    QPushButton *btnMode;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelCurrentTime;
    QSlider *sliderProgress;
    QLabel *labelTotalTime;
    QHBoxLayout *horizontalLayout_4;
    QSlider *sliderVolume;
    QPushButton *btnMute;
    QLabel *label;
    QMenuBar *menubar;
    QMenu *menu;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(900, 600);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        btnOpen = new QPushButton(centralwidget);
        btnOpen->setObjectName(QString::fromUtf8("btnOpen"));

        horizontalLayout->addWidget(btnOpen);

        editSearch = new QLineEdit(centralwidget);
        editSearch->setObjectName(QString::fromUtf8("editSearch"));

        horizontalLayout->addWidget(editSearch);

        btnSearch = new QPushButton(centralwidget);
        btnSearch->setObjectName(QString::fromUtf8("btnSearch"));

        horizontalLayout->addWidget(btnSearch);


        verticalLayout->addLayout(horizontalLayout);

        listWidgetSongs = new QListWidget(centralwidget);
        listWidgetSongs->setObjectName(QString::fromUtf8("listWidgetSongs"));
        sizePolicy.setHeightForWidth(listWidgetSongs->sizePolicy().hasHeightForWidth());
        listWidgetSongs->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(listWidgetSongs);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        btnPrevious = new QPushButton(centralwidget);
        btnPrevious->setObjectName(QString::fromUtf8("btnPrevious"));

        horizontalLayout_2->addWidget(btnPrevious);

        btnPlay = new QPushButton(centralwidget);
        btnPlay->setObjectName(QString::fromUtf8("btnPlay"));

        horizontalLayout_2->addWidget(btnPlay);

        btnNext = new QPushButton(centralwidget);
        btnNext->setObjectName(QString::fromUtf8("btnNext"));

        horizontalLayout_2->addWidget(btnNext);

        btnMode = new QPushButton(centralwidget);
        btnMode->setObjectName(QString::fromUtf8("btnMode"));

        horizontalLayout_2->addWidget(btnMode);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        labelCurrentTime = new QLabel(centralwidget);
        labelCurrentTime->setObjectName(QString::fromUtf8("labelCurrentTime"));

        horizontalLayout_3->addWidget(labelCurrentTime);

        sliderProgress = new QSlider(centralwidget);
        sliderProgress->setObjectName(QString::fromUtf8("sliderProgress"));
        sliderProgress->setOrientation(Qt::Horizontal);

        horizontalLayout_3->addWidget(sliderProgress);

        labelTotalTime = new QLabel(centralwidget);
        labelTotalTime->setObjectName(QString::fromUtf8("labelTotalTime"));

        horizontalLayout_3->addWidget(labelTotalTime);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        sliderVolume = new QSlider(centralwidget);
        sliderVolume->setObjectName(QString::fromUtf8("sliderVolume"));
        sliderVolume->setOrientation(Qt::Horizontal);

        horizontalLayout_4->addWidget(sliderVolume);

        btnMute = new QPushButton(centralwidget);
        btnMute->setObjectName(QString::fromUtf8("btnMute"));

        horizontalLayout_4->addWidget(btnMute);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_4->addWidget(label);


        verticalLayout->addLayout(horizontalLayout_4);

        verticalLayout->setStretch(1, 1);

        verticalLayout_2->addLayout(verticalLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 900, 26));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        QFont font;
        font.setUnderline(false);
        font.setStrikeOut(true);
        menu->setFont(font);
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menu->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\344\270\211\345\215\201\351\237\263\344\271\220", nullptr));
        btnOpen->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200", nullptr));
        btnSearch->setText(QCoreApplication::translate("MainWindow", "\346\220\234\347\264\242", nullptr));
        btnPrevious->setText(QCoreApplication::translate("MainWindow", "\344\270\212\344\270\200\351\246\226", nullptr));
        btnPlay->setText(QCoreApplication::translate("MainWindow", "\346\222\255\346\224\276", nullptr));
        btnNext->setText(QCoreApplication::translate("MainWindow", "\344\270\213\344\270\200\351\246\226", nullptr));
        btnMode->setText(QCoreApplication::translate("MainWindow", "\351\241\272\345\272\217\346\222\255\346\224\276", nullptr));
        labelCurrentTime->setText(QCoreApplication::translate("MainWindow", "0:00", nullptr));
        labelTotalTime->setText(QCoreApplication::translate("MainWindow", "0:00", nullptr));
        btnMute->setText(QCoreApplication::translate("MainWindow", "\351\235\231\351\237\263", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        menu->setTitle(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
