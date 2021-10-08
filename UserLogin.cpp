#include "UserLogin.h"
//#include "CCMainWindow.h"

UserLogin::UserLogin(QWidget* parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle(QString::fromLocal8Bit("ÖíÎ²°Í"), "");
	loadStyleSheet("login");
	initControl();
}

UserLogin::~UserLogin()
{
}

void UserLogin::initControl()
{
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);
	connect(ui.cancelBtn, &QPushButton::clicked, this, &BasicWindow::onButtonCloseClicked);
}

void UserLogin::onLoginBtnClicked()
{
	close();
	//CCMainWindow* mainwindow = new CCMainWindow;
	//mainwindow->show();
}