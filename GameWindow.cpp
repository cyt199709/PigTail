#include "GameWindow.h"
#include <QTime>
#include <QTimer>
#include <QEventLoop>
#include <QMessageBox>
#include <QPaintEvent>
#include <QMap>
#include <QApplication>

GameWindow::GameWindow(QWidget *parent, bool isRobot, bool isOnline)
	: BasicWindow(parent)
	, m_isRobot(isRobot)
	, m_isOnline(isOnline)
{	
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	initControl();
	m_onPlayer1 = true;
	m_isMousePressed = false;
	loadStyleSheet("GameWindow");
	connect(_titleBar->m_pButtonClose, &QPushButton::clicked, []() {QApplication::quit(); });
}

GameWindow::~GameWindow()
{

}

void GameWindow::initControl()
{
	m_isOnGameWindow = true;
	_titleBar->setGameStatu(m_isOnGameWindow);
	QTime tm = QTime::currentTime();
	qsrand(tm.msec() + tm.second() * 1000);
	for (int i = 0; i < 52; ) // 将52张牌压入栈中
	{
		int tmp = (qrand() % 52) + 1;
		Card *card = new Card(ui.deckWidget, POSITION::DECK, tmp, BACK);
		m_deckCard.push_back(card);
		bool isExist = false;
		for (int j = 0; j < i; j++)
		{
			if (m_deckCard[i]->getId() == m_deckCard[j]->getId())
			{
				delete m_deckCard[i];
				m_deckCard.pop_back();
				isExist = true;
				break;
			}
		}
		if (isExist)
			continue;
		connect(m_deckCard[i], &Card::clicked, this, &GameWindow::onCardClicked);
		i++;
	}	

	connect(this, SIGNAL(playerOper(bool)), this, SLOT(onPlayerOper(bool)));
}

void GameWindow::addAllCardToPlayer(QVector<Card*>* player)
{
	int count = m_cemeteryCard.size();
	for(int i = 0; i < count; i++)
	{
		Card* card = m_cemeteryCard.top();
		m_cemeteryCard.pop_back();
		player->push_back(card);
		if (*player == m_player1Card)
		{
			card->setPosition(PLAYER1);
			card->setParent(ui.Player1Widget);
		}
		else
		{
			card->setPosition(PLAYER2);
			card->setParent(ui.Player2Widget);
		}
	}
	update();
}

void GameWindow::wait(int msec)
{
	QEventLoop loop;
	QTimer::singleShot(msec, &loop, SLOT(quit()));
	loop.exec();
}

void GameWindow::judgeWinner()
{
	if (m_player1Card.size() > m_player2Card.size())
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("玩家一胜利!"));
	}
	else if (m_player1Card.size() < m_player2Card.size() && !m_isRobot)
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("玩家二胜利!"));
	}
	else if (m_player1Card.size() < m_player2Card.size() && m_isRobot)
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("电脑胜利!"));
	}
	else
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("平局!"));
	}
	close();
}

void GameWindow::onRobotOper()
{
	// 数量count0代表DIAMOND 1代表SPADE
	QMap<DECOR, int> count;
	count.insert(DIAMOND, 0);
	count.insert(SPADE, 0);
	count.insert(HEART, 0);
	count.insert(CLUB, 0);
	
	int cardCount = m_player2Card.size();
	if (cardCount > 0)
	{
		for (int i = 0; i < cardCount; i++)
		{
			if (m_player2Card[i]->getId() % 4 == DIAMOND)
				count[DIAMOND]++;
			else if (m_player2Card[i]->getId() % 4 == SPADE)
				count[SPADE]++;
			else if (m_player2Card[i]->getId() % 4 == HEART)
				count[HEART]++;
			else if (m_player2Card[i]->getId() % 4 == CLUB)
				count[CLUB]++;
		}

		// 对count排序
		typedef QPair<DECOR, int> Decor;
		QVector<Decor> Vec;
		for (auto it = count.begin(); it != count.end(); it++)
		{
			Decor element(it.key(), it.value());
			Vec.append(element);
		}

		// 排序
		qSort(Vec.begin(), Vec.end(), [](Decor ele1, Decor ele2) {return ele1.second > ele2.second; });		



		if (m_cemeteryCard.size() > 0 && m_cemeteryCard.top()->getId() % 4 == SPADE)
		{
			QVector<Decor>::iterator it = Vec.begin();
			// 打出最多的且花色与cemetery顶部花色不同的卡
			if (it->first != SPADE)
			{				
				for (int i = 0; i < cardCount; i++)
				{
					if (m_player2Card[i]->getId() % 4 == it->first)
					{
						Card* card = m_player2Card[i];
						m_player2Card.remove(i);
						m_cemeteryCard.push_back(card);
						card->setPosition(CEMETERY);
						card->setParent(ui.cemeteryWidget);
						break;
					}
				}
			}
			else 
			{
				it++;
				if (it->second != 0)
				{
					for (int i = 0; i < cardCount; i++)
					{
						if (m_player2Card[i]->getId() % 4 == it->first)
						{
							Card* card = m_player2Card[i];
							m_player2Card.remove(i);
							m_cemeteryCard.push_back(card);
							card->setPosition(CEMETERY);
							card->setParent(ui.cemeteryWidget);
							break;
						}						
					}
				}
				else // 只能抽牌
				{
					onRobotTackCard();
				}
			}

		}
		else if (m_cemeteryCard.size() > 0 && m_cemeteryCard.top()->getId() % 4 == HEART)
		{
			auto it = Vec.begin();
			// 打出最多的且花色与cemetery顶部花色不同的卡
			if (it->first != HEART)
			{
				for (int i = 0; i < cardCount; i++)
				{
					if (m_player2Card[i]->getId() % 4 == it->first)
					{
						Card* card = m_player2Card[i];
						m_player2Card.remove(i);
						m_cemeteryCard.push_back(card);
						card->setPosition(CEMETERY);
						card->setParent(ui.cemeteryWidget);
						break;
					}
				}
			}
			else
			{
				it++;
				if (it->second != 0)
				{
					for (int i = 0; i < cardCount; i++)
					{
						if (m_player2Card[i]->getId() % 4 == it->first)
						{
							Card* card = m_player2Card[i];
							m_player2Card.remove(i);
							m_cemeteryCard.push_back(card);
							card->setPosition(CEMETERY);
							card->setParent(ui.cemeteryWidget);
							break;
						}
					}
				}
				else
				{
					onRobotTackCard();
				}
			}

		}
		else if (m_cemeteryCard.size() > 0 && m_cemeteryCard.top()->getId() % 4 == CLUB)
		{
			auto it = Vec.begin();
			// 打出最多的且花色与cemetery顶部花色不同的卡
			if (it->first != CLUB)
			{
				for (int i = 0; i < cardCount; i++)
				{
					if (m_player2Card[i]->getId() % 4 == it->first)
					{
						Card* card = m_player2Card[i];
						m_player2Card.remove(i);
						m_cemeteryCard.push_back(card);
						card->setPosition(CEMETERY);
						card->setParent(ui.cemeteryWidget);
						break;
					}
				}
			}
			else
			{
				it++;
				if (it->second != 0)
				{
					for (int i = 0; i < cardCount; i++)
					{
						if (m_player2Card[i]->getId() % 4 == it->first)
						{
							Card* card = m_player2Card[i];
							m_player2Card.remove(i);
							m_cemeteryCard.push_back(card);
							card->setPosition(CEMETERY);
							card->setParent(ui.cemeteryWidget);
							break;
						}
					}
				}
				else
				{
					onRobotTackCard();
				}
			}

		}
		else if (m_cemeteryCard.size() > 0 && m_cemeteryCard.top()->getId() % 4 == DIAMOND)
		{
			auto it = Vec.begin();
			// 打出最多的且花色与cemetery顶部花色不同的卡
			if (it->first != DIAMOND)
			{
				for (int i = 0; i < cardCount; i++)
				{
					if (m_player2Card[i]->getId() % 4 == it->first)
					{
						Card* card = m_player2Card[i];
						m_player2Card.remove(i);
						m_cemeteryCard.push_back(card);
						card->setPosition(CEMETERY);
						card->setParent(ui.cemeteryWidget);
						break;
					}
				}
			}
			else
			{
				it++;
				if (it->second != 0)
				{
					for (int i = 0; i < cardCount; i++)
					{
						if (m_player2Card[i]->getId() % 4 == it->first)
						{
							Card* card = m_player2Card[i];
							m_player2Card.remove(i);
							m_cemeteryCard.push_back(card);
							card->setPosition(CEMETERY);
							card->setParent(ui.cemeteryWidget);
							break;
						}
					}
				}
				else
				{
					onRobotTackCard();
				}
			}
		}		
		else // cemetery中无牌时，出最多的那一种花色
		{
			auto it = Vec.begin();
			for (int i = 0; i < cardCount; i++)
			{
				if (m_player2Card[i]->getId() % 4 == it->first)
				{
					Card* card = m_player2Card[i];
					m_player2Card.remove(i);
					m_cemeteryCard.push_back(card);
					card->setPosition(CEMETERY);
					card->setParent(ui.cemeteryWidget);
					break;
				}
			}
		}
	}
	else // 只能抽牌
	{
		onRobotTackCard();
	}
	
	m_onPlayer1 = true;
	update();
}

void GameWindow::onRobotTackCard()
{
	Card* card = m_deckCard.top();
	m_deckCard.top()->setCardType(POSITIVE);
	update();
	wait(1000);

	if (m_cemeteryCard.size() > 0 && card->getId() % 4 == m_cemeteryCard.top()->getId() % 4)
	{
		// 将弃牌堆全部放到手中		
		m_deckCard.pop_back();
		m_player2Card.push_back(card);
		card->setPosition(PLAYER2);
		card->setParent(ui.Player2Widget);
		addAllCardToPlayer(&m_player2Card);
	}
	else
	{
		m_deckCard.pop_back();
		m_cemeteryCard.push_back(card);
		card->setPosition(CEMETERY);
		card->setParent(ui.cemeteryWidget);
	}
}

void GameWindow::onPlayerOper(bool Oper)
{
	if (Oper && !m_isRobot)
	{
		QMessageBox::information(this, "提示", "轮到玩家二出牌!");
	}
	else if (!Oper)
	{
		QMessageBox::information(this, "提示", "轮到玩家一出牌!");
	}
}

void GameWindow::paintEvent(QPaintEvent* event)
{
	for (int i = m_deckCard.size() - 1; i >= 0; i--)
	{
		m_deckCard[i]->setGeometry(m_deckCard.size() - 1 - i, 0, m_deckCard[i]->width(), m_deckCard[i]->height());
		m_deckCard[i]->show();
	}

	for (int i = m_cemeteryCard.size() - 1; i >= 0; i--)
	{
		m_cemeteryCard[i]->setGeometry(m_cemeteryCard.size() - 1 - i, 0, m_cemeteryCard[i]->width(), m_cemeteryCard[i]->height());
		m_cemeteryCard[i]->show();
	}

	for (int i = 0; i < m_player1Card.size(); i++)
	{
		if(i * 45 < 1530)
			m_player1Card[i]->setGeometry(i * 45, 0, m_player1Card[i]->width(), m_player1Card[i]->height());
		else if(i * 45 >= 1530)
			m_player1Card[i]->setGeometry((i - 34) * 45, 75, m_player1Card[i]->width(), m_player1Card[i]->height());
		m_player1Card[i]->show();
	}

	for (int i = 0; i < m_player2Card.size(); i++)
	{
		if (i * 45 < 1530)
			m_player2Card[i]->setGeometry(1780 - i * 45 - 250, 0, m_player2Card[i]->width(), m_player2Card[i]->height());
		else if (i * 45 >= 1530)
			m_player2Card[i]->setGeometry(1780 - (i - 34) * 45 - 250, 75, m_player2Card[i]->width(), m_player2Card[i]->height());
		m_player2Card[i]->show();
	}

	return QWidget::paintEvent(event);
}

void GameWindow::loadStyleSheet(const QString& sheetName)
{
	m_styleName = sheetName;
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen())
	{
		setStyleSheet("");
		QString qsstyleSheet = QLatin1String(file.readAll());

		// 获取当前用户的皮肤RGB值
		qsstyleSheet += QString("QWidget[titleSkin=true]\
								{background-color:rgb(57,41,85);\
								border-top-left-radius:4px;}\
								QWidget[bottomSkin=true]\
								{border-top:1px solid rgb(0,153,102);\
								background-color:rgb(0,153,102);\
								border-bottom-left-radius:4px;\
								border-bottom-right-radius:4px;}\
								");
		setStyleSheet(qsstyleSheet);
	}

	file.close();
}

void GameWindow::onCardClicked(int m_id, POSITION position)
{

	if (m_isMousePressed)
		return;
	m_isMousePressed = true;
	if (m_onPlayer1 && position == PLAYER1)
	{
		for (int i = 0; i < m_player1Card.size(); i++)
		{
			if (m_player1Card[i]->getId() == m_id)
			{
				Card* card = m_player1Card[i];				

				if (m_cemeteryCard.size() > 0 && card->getId() % 4 == m_cemeteryCard.top()->getId() % 4)
				{
					addAllCardToPlayer(&m_player1Card);
				}
				else
				{
					m_player1Card.remove(i);
					m_cemeteryCard.push_back(card);
					card->setPosition(CEMETERY);
					card->setParent(ui.cemeteryWidget);
				}

				m_onPlayer1 = false;
				break;
			}
		}
	}
	else if (m_onPlayer1 && position == DECK)
	{	
		m_deckCard.top()->setCardType(POSITIVE);
		update();
		wait(1000);

		Card* card = m_deckCard.top();		

		if (m_cemeteryCard.size() > 0 && m_deckCard.top()->getId() % 4 == m_cemeteryCard.top()->getId() % 4)
		{
			// 将弃牌堆全部放到手中		
			m_deckCard.pop_back();
			m_player1Card.push_back(card);
			card->setPosition(PLAYER1);
			card->setParent(ui.Player1Widget);
			addAllCardToPlayer(&m_player1Card);
		}
		else
		{
			m_deckCard.pop_back();
			m_cemeteryCard.push_back(card);
			card->setPosition(CEMETERY);
			card->setParent(ui.cemeteryWidget);
		}
		
		m_onPlayer1 = false;
	}
	// 两人在同一台电脑上面操作
	else if (!m_isRobot && !m_onPlayer1 && !m_isOnline)
	{
		if (position == PLAYER2)
		{
			for (int i = 0; i < m_player2Card.size(); i++)
			{
				if (m_player2Card[i]->getId() == m_id)
				{
					Card* card = m_player2Card[i];

					if (m_cemeteryCard.size() > 0 && card->getId() % 4 == m_cemeteryCard.top()->getId() % 4)
					{
						addAllCardToPlayer(&m_player2Card);
					}
					else
					{
						m_player2Card.remove(i);
						m_cemeteryCard.push_back(card);
						card->setPosition(CEMETERY);
						card->setParent(ui.cemeteryWidget);
					}

					m_onPlayer1 = true;
					break;
				}
			}
		}
		else if (position == DECK)
		{
			m_deckCard.top()->setCardType(POSITIVE);
			update();
			wait(1000);

			Card* card = m_deckCard.top();
			
			if (m_cemeteryCard.size() > 0 && m_deckCard.top()->getId() % 4 == m_cemeteryCard.top()->getId() % 4)
			{
				// 将弃牌堆全部放到手中			
				m_deckCard.pop_back();
				m_player2Card.push_back(card);
				card->setPosition(PLAYER2);
				card->setParent(ui.Player2Widget);
				addAllCardToPlayer(&m_player2Card);
			}
			else
			{
				m_deckCard.pop_back();
				m_cemeteryCard.push_back(card);
				card->setPosition(CEMETERY);
				card->setParent(ui.cemeteryWidget);
			}

			m_onPlayer1 = true;
			if (m_deckCard.size() == 0)
				judgeWinner();
		}
	}

	wait(500);

	if (m_isRobot && !m_onPlayer1)
	{
		onRobotOper();
	}

	update();
	m_isMousePressed = false;
}
