#pragma once

#include <QtWidgets/QWidget>
#include "ui_PigTail.h"

class PigTail : public QWidget
{
    Q_OBJECT

public:
    PigTail(QWidget *parent = Q_NULLPTR);

private:
    Ui::PigTailClass ui;
};
