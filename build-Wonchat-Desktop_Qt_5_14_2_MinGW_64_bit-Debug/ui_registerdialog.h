/********************************************************************************
** Form generated from reading UI file 'registerdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTERDIALOG_H
#define UI_REGISTERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RegisterDialog
{
public:
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_2;
    QWidget *widget;
    QLabel *err_label;
    QHBoxLayout *horizontalLayout_6;
    QLabel *user_label;
    QLineEdit *user_edit;
    QHBoxLayout *horizontalLayout_5;
    QLabel *email_label;
    QLineEdit *email_edit;
    QHBoxLayout *horizontalLayout_4;
    QLabel *pass_label;
    QLineEdit *pass_edit;
    QHBoxLayout *horizontalLayout_3;
    QLabel *passgain_label;
    QLineEdit *passgain_edit;
    QHBoxLayout *horizontalLayout;
    QLabel *varify_label;
    QLineEdit *varify_edit;
    QPushButton *get_code;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *ok_pb;
    QPushButton *cancel_pb;
    QSpacerItem *verticalSpacer_3;

    void setupUi(QDialog *RegisterDialog)
    {
        if (RegisterDialog->objectName().isEmpty())
            RegisterDialog->setObjectName(QString::fromUtf8("RegisterDialog"));
        RegisterDialog->resize(300, 500);
        RegisterDialog->setMinimumSize(QSize(300, 500));
        verticalLayout = new QVBoxLayout(RegisterDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        widget = new QWidget(RegisterDialog);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMinimumSize(QSize(0, 50));
        err_label = new QLabel(widget);
        err_label->setObjectName(QString::fromUtf8("err_label"));
        err_label->setGeometry(QRect(40, 0, 200, 50));
        err_label->setTextFormat(Qt::MarkdownText);
        err_label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(widget);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        user_label = new QLabel(RegisterDialog);
        user_label->setObjectName(QString::fromUtf8("user_label"));
        user_label->setMinimumSize(QSize(50, 40));
        user_label->setSizeIncrement(QSize(0, 25));

        horizontalLayout_6->addWidget(user_label);

        user_edit = new QLineEdit(RegisterDialog);
        user_edit->setObjectName(QString::fromUtf8("user_edit"));

        horizontalLayout_6->addWidget(user_edit);


        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        email_label = new QLabel(RegisterDialog);
        email_label->setObjectName(QString::fromUtf8("email_label"));
        email_label->setMinimumSize(QSize(50, 40));
        email_label->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_5->addWidget(email_label);

        email_edit = new QLineEdit(RegisterDialog);
        email_edit->setObjectName(QString::fromUtf8("email_edit"));

        horizontalLayout_5->addWidget(email_edit);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        pass_label = new QLabel(RegisterDialog);
        pass_label->setObjectName(QString::fromUtf8("pass_label"));
        pass_label->setMinimumSize(QSize(50, 40));

        horizontalLayout_4->addWidget(pass_label);

        pass_edit = new QLineEdit(RegisterDialog);
        pass_edit->setObjectName(QString::fromUtf8("pass_edit"));
        pass_edit->setEchoMode(QLineEdit::Password);

        horizontalLayout_4->addWidget(pass_edit);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        passgain_label = new QLabel(RegisterDialog);
        passgain_label->setObjectName(QString::fromUtf8("passgain_label"));
        passgain_label->setMinimumSize(QSize(50, 40));

        horizontalLayout_3->addWidget(passgain_label);

        passgain_edit = new QLineEdit(RegisterDialog);
        passgain_edit->setObjectName(QString::fromUtf8("passgain_edit"));
        passgain_edit->setEchoMode(QLineEdit::Password);

        horizontalLayout_3->addWidget(passgain_edit);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        varify_label = new QLabel(RegisterDialog);
        varify_label->setObjectName(QString::fromUtf8("varify_label"));
        varify_label->setMinimumSize(QSize(50, 40));

        horizontalLayout->addWidget(varify_label);

        varify_edit = new QLineEdit(RegisterDialog);
        varify_edit->setObjectName(QString::fromUtf8("varify_edit"));

        horizontalLayout->addWidget(varify_edit);

        get_code = new QPushButton(RegisterDialog);
        get_code->setObjectName(QString::fromUtf8("get_code"));
        get_code->setMinimumSize(QSize(0, 40));

        horizontalLayout->addWidget(get_code);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        ok_pb = new QPushButton(RegisterDialog);
        ok_pb->setObjectName(QString::fromUtf8("ok_pb"));
        ok_pb->setMinimumSize(QSize(0, 40));

        horizontalLayout_2->addWidget(ok_pb);

        cancel_pb = new QPushButton(RegisterDialog);
        cancel_pb->setObjectName(QString::fromUtf8("cancel_pb"));
        cancel_pb->setMinimumSize(QSize(0, 40));

        horizontalLayout_2->addWidget(cancel_pb);


        verticalLayout->addLayout(horizontalLayout_2);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);


        retranslateUi(RegisterDialog);

        QMetaObject::connectSlotsByName(RegisterDialog);
    } // setupUi

    void retranslateUi(QDialog *RegisterDialog)
    {
        RegisterDialog->setWindowTitle(QCoreApplication::translate("RegisterDialog", "Dialog", nullptr));
        err_label->setText(QCoreApplication::translate("RegisterDialog", "\351\224\231\350\257\257\346\217\220\347\244\272", nullptr));
        user_label->setText(QCoreApplication::translate("RegisterDialog", "\347\224\250\346\210\267\357\274\232", nullptr));
        email_label->setText(QCoreApplication::translate("RegisterDialog", "\351\202\256\347\256\261\357\274\232", nullptr));
        pass_label->setText(QCoreApplication::translate("RegisterDialog", "\345\257\206\347\240\201\357\274\232", nullptr));
        passgain_label->setText(QCoreApplication::translate("RegisterDialog", "\347\241\256\350\256\244", nullptr));
        varify_label->setText(QCoreApplication::translate("RegisterDialog", "\351\252\214\350\257\201\347\240\201\357\274\232", nullptr));
        get_code->setText(QCoreApplication::translate("RegisterDialog", "\350\216\267\345\217\226", nullptr));
        ok_pb->setText(QCoreApplication::translate("RegisterDialog", "\347\241\256\345\256\232", nullptr));
        cancel_pb->setText(QCoreApplication::translate("RegisterDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RegisterDialog: public Ui_RegisterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTERDIALOG_H
