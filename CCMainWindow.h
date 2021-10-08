#pragma once

#include "BasicWindow.h"
#include "ui_CCMainWindow.h"

class CCMainWindow : public BasicWindow
{
	Q_OBJECT

public:
	CCMainWindow(QWidget *parent = Q_NULLPTR);
	~CCMainWindow();

public slots:
	void on_PvEBtn_clicked();
	void on_onLocalPvPBtn_clicked();
	void on_onlinePvPBtn_clicked();

private:
	Ui::CCMainWindow ui;
};
