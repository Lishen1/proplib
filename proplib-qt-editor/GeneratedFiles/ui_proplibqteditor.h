/********************************************************************************
** Form generated from reading UI file 'proplibqteditor.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROPLIBQTEDITOR_H
#define UI_PROPLIBQTEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_proplibqteditorClass
{
public:
    QAction *actionOpen_Config;
    QAction *actionOpen_HTTP;
    QAction *actionSave_Config;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_3;
    QSplitter *splitter;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *proplibqteditorClass)
    {
        if (proplibqteditorClass->objectName().isEmpty())
            proplibqteditorClass->setObjectName(QStringLiteral("proplibqteditorClass"));
        proplibqteditorClass->resize(735, 546);
        actionOpen_Config = new QAction(proplibqteditorClass);
        actionOpen_Config->setObjectName(QStringLiteral("actionOpen_Config"));
        actionOpen_HTTP = new QAction(proplibqteditorClass);
        actionOpen_HTTP->setObjectName(QStringLiteral("actionOpen_HTTP"));
        actionSave_Config = new QAction(proplibqteditorClass);
        actionSave_Config->setObjectName(QStringLiteral("actionSave_Config"));
        centralWidget = new QWidget(proplibqteditorClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout_3 = new QHBoxLayout(centralWidget);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        widget = new QWidget(splitter);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        splitter->addWidget(widget);
        widget_2 = new QWidget(splitter);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        splitter->addWidget(widget_2);

        horizontalLayout_3->addWidget(splitter);

        proplibqteditorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(proplibqteditorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 735, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        proplibqteditorClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(proplibqteditorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        proplibqteditorClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen_Config);
        menuFile->addAction(actionOpen_HTTP);
        menuFile->addAction(actionSave_Config);

        retranslateUi(proplibqteditorClass);

        QMetaObject::connectSlotsByName(proplibqteditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *proplibqteditorClass)
    {
        proplibqteditorClass->setWindowTitle(QApplication::translate("proplibqteditorClass", "proplibqteditor", nullptr));
        actionOpen_Config->setText(QApplication::translate("proplibqteditorClass", "Open Config", nullptr));
        actionOpen_HTTP->setText(QApplication::translate("proplibqteditorClass", "Open HTTP", nullptr));
        actionSave_Config->setText(QApplication::translate("proplibqteditorClass", "Save Config", nullptr));
        menuFile->setTitle(QApplication::translate("proplibqteditorClass", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class proplibqteditorClass: public Ui_proplibqteditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROPLIBQTEDITOR_H
