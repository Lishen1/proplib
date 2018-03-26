#pragma once

#include <QtWidgets/QWidget>
#include "ui_proplibqtgui.h"

class proplibqtgui : public QWidget
{
    Q_OBJECT

public:
    proplibqtgui(QWidget *parent = Q_NULLPTR);

private:
    Ui::proplibqtguiClass ui;
};
