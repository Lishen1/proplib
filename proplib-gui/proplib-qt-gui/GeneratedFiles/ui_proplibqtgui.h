/********************************************************************************
** Form generated from reading UI file 'proplibqtgui.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROPLIBQTGUI_H
#define UI_PROPLIBQTGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_proplibqtguiClass
{
public:

    void setupUi(QWidget *proplibqtguiClass)
    {
        if (proplibqtguiClass->objectName().isEmpty())
            proplibqtguiClass->setObjectName(QStringLiteral("proplibqtguiClass"));
        proplibqtguiClass->resize(600, 400);

        retranslateUi(proplibqtguiClass);

        QMetaObject::connectSlotsByName(proplibqtguiClass);
    } // setupUi

    void retranslateUi(QWidget *proplibqtguiClass)
    {
        proplibqtguiClass->setWindowTitle(QApplication::translate("proplibqtguiClass", "proplibqtgui", nullptr));
    } // retranslateUi

};

namespace Ui {
    class proplibqtguiClass: public Ui_proplibqtguiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROPLIBQTGUI_H
