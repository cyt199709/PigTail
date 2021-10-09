#pragma once

#include <QDialog>
#include "TitleBar.h"

class BasicWindow : public QDialog
{
	Q_OBJECT

public:
	BasicWindow(QWidget* parent = nullptr);
	virtual ~BasicWindow();

public:
	// ������ʽ��
	void loadStyleSheet(const QString& sheetName);

private:
	void initBackGroundColor(); // ��ʼ������

protected:
	void paintEvent(QPaintEvent*); // �����¼�
	void mousePressEvent(QMouseEvent* event); // ��갴���¼�
	void mouseMoveEvent(QMouseEvent* event);	// ����ƶ��¼�
	void mouseReleaseEvent(QMouseEvent* event); // ����ͷ��¼�

protected:
	void initTitleBar(ButtonType buttontype = MIN_BUTTON);
	void setTitleBarTitle(const QString& title, const QString& icon = "");

public slots:
	void onShowClose(bool);
	void onShowMin(bool);
	void onShowHide(bool);
	void onShowNormal(bool);
	void onShowQuit(bool);

	void onButtonMinClicked();
	void onButtonRestoreClicked();
	void onButtonMaxClicked();
	void onButtonCloseClicked();

protected:
	QPoint m_mousePoint; // ���λ��
	bool m_mousePressed; // ����Ƿ���
	QString m_styleName; // ��ʽ�ļ�����
	TitleBar* _titleBar;
	bool m_isOnGameWindow;

};
