#pragma once

#include "BasicWindow.h"
#include "ui_GameWindow.h"
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

private:
	void initControl();
	void addAllCardToPlayer(QVector<Card*>* player);
	void wait(int msec);
	void judgeWinner();
	void onRobotOper();
	void onRobotTackCard();//电脑
	void onPlayerOutCard(QVector<Card*>* player, int id, QString opt = ""); // 出牌
	void onPlayerFlop(QVector<Card*>* player, QString opt = "");// 翻牌

private slots:
	void onCardClicked(int id, POSITION position);

protected:
	virtual void paintEvent(QPaintEvent* event);
	virtual void loadStyleSheet(const QString& sheetName);

signals:
	void playerOper(bool); // 玩家1操作完发送true信号，玩家2操作完发送false信号

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
};
