#pragma once

#include "BasicWindow.h"
#include "ui_GameWindow.h"
#include "WorkThread.h"
#include "Card.h"
#include <QStack>
#include <QVector>

// 花色
enum DECOR {
	DIAMOND = 0,
	SPADE,
	HEART,
	CLUB	
};

class GameWindow : public BasicWindow
{
	Q_OBJECT

public:
	GameWindow(QWidget *parent = Q_NULLPTR, bool isRobot = true, bool isOnline = false);
	~GameWindow();
	void getMsgInServer();

private:
	void initControl();
	void addAllCardToPlayer(QVector<Card*>* player);
	void wait(int msec);
	void judgeWinner();
	void onRobotOper();
	void onRobotTackCard();//电脑
	void onPlayerOutCard(QVector<Card*>* player, int id, QString opt = ""); // 出牌
	void onPlayerFlop(QVector<Card*>* player, QString opt = "");// 翻牌
	int analysisCode(QString code);
	void onlineFlop(QVector<Card*>* player, int id);

private slots:
	void onCardClicked(int id, POSITION position);
	void onPlayer2Oper(bool oper, int id);
	

protected:
	virtual void paintEvent(QPaintEvent* event);
	virtual void loadStyleSheet(const QString& sheetName);

signals:
	void playerOper(bool, int); // 抽牌true，出牌false

private:
	Ui::GameWindow ui;

private:
	QVector<Card*> m_player1Card;
	QVector<Card*> m_player2Card;
	QStack<Card*> m_deckCard;
	QStack<Card*> m_cemeteryCard;
	bool m_onPlayer1;
	bool m_isRobot;
	bool m_isOnline;
	bool m_isMousePressed;
	bool m_isGameAtFirst;
	bool m_isOnPlayer2;
	WorkThread* m_thread1;
};
