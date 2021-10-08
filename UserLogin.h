#pragma once

#include "BasicWindow.h"
#include "ui_UserLogin.h"

class QNetworkReply;

class UserLogin : public BasicWindow
{
	Q_OBJECT

public:
	UserLogin(QWidget *parent = Q_NULLPTR);
	~UserLogin();

private slots:
	void onLoginBtnClicked();
	void finishedSlot(QNetworkReply* reply);

private:
	void initControl();

private:
	Ui::UserLogin ui;
};
