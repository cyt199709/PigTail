#include "CCMainWindow.h"
#include "GameWindow.h"

CCMainWindow::CCMainWindow(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	//setTitleBarTitle(QString::fromLocal8Bit("ÖíÎ²°Í"), "");
	loadStyleSheet("CCMainWindow");
	//initControl();
}

CCMainWindow::~CCMainWindow()
{

}

void CCMainWindow::on_PvEBtn_clicked()
{
	GameWindow* gameWindow = new GameWindow(nullptr, true, false);
	gameWindow->show();
}

void CCMainWindow::on_onLocalPvPBtn_clicked()
{
	GameWindow* gameWindow = new GameWindow(nullptr, false, false);
	gameWindow->show();
}

void CCMainWindow::on_onlinePvPBtn_clicked()
{
	GameWindow* gameWindow = new GameWindow(nullptr, false, true);
	gameWindow->show();
}
