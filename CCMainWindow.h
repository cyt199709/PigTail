#pragma once

#include <QNetworkReply>
#include <QDialog>
#include "BasicWindow.h"
#include "ui_CCMainWindow.h"

class CCMainWindow : public BasicWindow
{
	Q_OBJECT

public:
	CCMainWindow(QWidget *parent = Q_NULLPTR);
	~CCMainWindow();

	static char* getFile(const char* filename);

public slots:
	void on_PvEBtn_clicked();
	void on_onLocalPvPBtn_clicked();
	void on_onlinePvPBtn_clicked();
	void createOnlineGame(bool);
	void joinOnlineGame(bool);
	void getGmaeList(bool);
	void setPrivate(int state);

private:
	Ui::CCMainWindow ui;

private:
	bool m_isPrivate;
	QDialog* m_dialog;
	QByteArray m_uid;
};
