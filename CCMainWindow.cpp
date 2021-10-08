#include "CCMainWindow.h"

CCMainWindow::CCMainWindow(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	//setTitleBarTitle(QString::fromLocal8Bit("÷ÌŒ≤∞Õ"), "");
	loadStyleSheet("CCMainWindow");
	//initControl();
}

CCMainWindow::~CCMainWindow()
{

}

void CCMainWindow::on_PvEBtn_clicked()
{
	
}

void CCMainWindow::on_onLocalPvPBtn_clicked()
{

}

void CCMainWindow::on_onlinePvPBtn_clicked()
{

}
