#include "GameWindow.h"
#include "CCMainWindow.h"
#include <QTime>
#include <QTimer>
#include <QEventLoop>
#include <QMessageBox>
#include <QPaintEvent>
#include <QMap>
#include <QApplication>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>
#include <curl/curl.h>
#include <QMutex>

extern QString gUuid;
extern QString gToken;
extern QString gId;
static QMutex mutex;

GameWindow::GameWindow(QWidget* parent, bool isRobot, bool isOnline)
	: BasicWindow(parent)
	, m_isRobot(isRobot)
	, m_isOnline(isOnline)
	, m_onPlayer1(false)
	, m_isOnPlayer2(false)
{	
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	initControl();
	m_thread1 = new WorkThread(this);

	connect(this, SIGNAL(playerOper(bool, int)), this, SLOT(onPlayer2Oper(bool, int)));

	if (isOnline) {
		m_isGameAtFirst = true;
		m_thread1->start();
	}

	m_isMousePressed = false;
	loadStyleSheet("GameWindow");
	connect(_titleBar->m_pButtonClose, &QPushButton::clicked, []() {QApplication::quit(); });
}

GameWindow::~GameWindow()
{

}

void GameWindow::initControl()
{
	ui.userLabel->setText(QString::fromLocal8Bit("�û�:") + gId + QString::fromLocal8Bit("������"));
	m_isOnGameWindow = true;
	_titleBar->setGameStatu(m_isOnGameWindow);
	QTime tm = QTime::currentTime();
	qsrand(tm.msec() + tm.second() * 1000);
	for (int i = 0; i < 52; ) // ��52����ѹ��ջ��
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
			if (m_isOnline)
				card->setCardType(BACK);
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
	if (m_thread1)
		m_thread1->exit();

	if (m_player1Card.size() > m_player2Card.size())
	{
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("��Ҷ�ʤ��!"));
	}
	else if (m_player1Card.size() < m_player2Card.size() && !m_isRobot)
	{
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("���һʤ��!"));
	}
	else if (m_player1Card.size() > m_player2Card.size() && m_isRobot)
	{
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("����ʤ��!"));
	}
	else
	{
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("ƽ��!"));
	}
	close();
}

void GameWindow::onRobotOper()
{
	// ����count0����DIAMOND 1����SPADE
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

		// ��count����
		typedef QPair<DECOR, int> Decor;
		QVector<Decor> Vec;
		for (auto it = count.begin(); it != count.end(); it++)
		{
			Decor element(it.key(), it.value());
			Vec.append(element);
		}

		// ����
		qSort(Vec.begin(), Vec.end(), [](Decor ele1, Decor ele2) {return ele1.second > ele2.second; });		



		if (m_cemeteryCard.size() > 0 && m_cemeteryCard.top()->getId() % 4 == SPADE)
		{
			QVector<Decor>::iterator it = Vec.begin();
			// ��������һ�ɫ��cemetery������ɫ��ͬ�Ŀ�
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
				else // ֻ�ܳ���
				{
					onRobotTackCard();
				}
			}

		}
		else if (m_cemeteryCard.size() > 0 && m_cemeteryCard.top()->getId() % 4 == HEART)
		{
			auto it = Vec.begin();
			// ��������һ�ɫ��cemetery������ɫ��ͬ�Ŀ�
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
			// ��������һ�ɫ��cemetery������ɫ��ͬ�Ŀ�
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
			// ��������һ�ɫ��cemetery������ɫ��ͬ�Ŀ�
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
		else // cemetery������ʱ����������һ�ֻ�ɫ
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
	else // ֻ�ܳ���
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

	m_deckCard.pop_back();
	if (m_cemeteryCard.size() > 0 && card->getId() % 4 == m_cemeteryCard.top()->getId() % 4)
	{
		// �����ƶ�ȫ���ŵ�����			
		m_player2Card.push_back(card);
		card->setPosition(PLAYER2);
		card->setParent(ui.Player2Widget);
		addAllCardToPlayer(&m_player2Card);
	}
	else
	{
		m_cemeteryCard.push_back(card);
		card->setPosition(CEMETERY);
		card->setParent(ui.cemeteryWidget);
	}
}

void GameWindow::onPlayerOutCard(QVector<Card*>* player, int id, QString opt)
{
	for (int i = 0; i < player->size(); i++)
	{
		if ((*player)[i]->getId() == id)
		{
			Card* card = (*player)[i];

			if (m_cemeteryCard.size() > 0 && card->getId() % 4 == m_cemeteryCard.top()->getId() % 4)
			{
				addAllCardToPlayer(player);
			}
			else
			{
				player->remove(i);
				m_cemeteryCard.push_back(card);
				card->setPosition(CEMETERY);
				card->setParent(ui.cemeteryWidget);
			}

			mutex.lock();
			m_onPlayer1 = true;
			if(*player == m_player1Card)
				m_onPlayer1 = false;
			mutex.unlock();
			break;
		}
	}
}

void GameWindow::onPlayerFlop(QVector<Card*>* player, QString opt)
{
	m_deckCard.top()->setCardType(POSITIVE);
	update();
	wait(1000);

	Card* card = m_deckCard.top();

	if (m_cemeteryCard.size() > 0 && m_deckCard.top()->getId() % 4 == m_cemeteryCard.top()->getId() % 4)
	{
		// �����ƶ�ȫ���ŵ�����		
		m_deckCard.pop_back();
		player->push_back(card);
		card->setPosition(PLAYER1);
		if (*player == m_player1Card)
			card->setParent(ui.Player1Widget);
		else
			card->setParent(ui.Player2Widget);
		addAllCardToPlayer(player);
	}
	else
	{
		m_deckCard.pop_back();
		m_cemeteryCard.push_back(card);
		card->setPosition(CEMETERY);
		card->setParent(ui.cemeteryWidget);
	}
	ui.userLabel->setText(QString::fromLocal8Bit("�û�:") + gId + QString::fromLocal8Bit("������") + "\n�ֵ�����");
	mutex.lock();
	m_onPlayer1 = true;
	if (*player == m_player1Card)
		m_onPlayer1 = false;
	mutex.unlock();
}

int GameWindow::analysisCode(QString code)
{
	int id;
	QString card = code.mid(4);
	if (card.mid(1) == QString("J"))
		id = 10 * 4;
	else if (card.mid(1) == QString("Q"))
		id = 11 * 4;
	else if (card.mid(1) == QString("K"))
		id = 12 * 4;
	else
		id = (card.mid(1).toInt() - 1) * 4;


	if (card[0] == "D")
		id += 4;
	else if (card[0] == "S")
		id += 1;
	else if (card[1] == "H")
		id += 2;
	else
		id += 3;

	return id;
}

void GameWindow::onlineFlop(QVector<Card*>* player, int id)
{
	m_deckCard.top()->setId(id);
	m_deckCard.top()->setCardType(POSITIVE);
	update();
	wait(800);

	Card* card = m_deckCard.top();
	m_deckCard.pop_back();
	if (m_cemeteryCard.size() > 0 && m_cemeteryCard.top()->getId() % 4 == card->getId() % 4)
	{
		if (*player == m_player1Card) {
			card->setParent(ui.Player1Widget);
			card->setPosition(PLAYER1);
		}
		else {
			card->setParent(ui.Player2Widget);
			card->setPosition(PLAYER2);
			card->setCardType(BACK);
		}
		player->push_back(card);
		addAllCardToPlayer(player);
	}
	else {
		m_cemeteryCard.push_back(card);
		card->setPosition(CEMETERY);
		card->setParent(ui.cemeteryWidget);
	}

	update();
}

void GameWindow::onPlayer2Oper(bool oper, int id)
{
	if(oper)
		onlineFlop(&m_player2Card, id);
	else
		onPlayerOutCard(&m_player2Card, id);

	mutex.lock();
	m_onPlayer1 = true;
	m_isOnPlayer2 = false;
	mutex.unlock();
}

void GameWindow::getMsgInServer()
{
	if (m_deckCard.size() == 0)
		judgeWinner();

	if (m_onPlayer1 || m_isOnPlayer2)
		return;

	mutex.lock();
	CURL* curl;
	CURLcode res;
	FILE* fp;
	if ((fp = fopen("onlineReturn.txt", "w")) == NULL)
		return;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
		QByteArray get = "http://172.17.173.97:9000/api/game/" + gUuid.toUtf8() + "/last";
		curl_easy_setopt(curl, CURLOPT_URL, get.data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);// ����д���ļ�        
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, (QByteArray("Authorization: Bearer ") + gToken.toUtf8()).data());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		res = curl_easy_perform(curl);

		fclose(fp);
		if (res != 0)
			return;
	}
	curl_easy_cleanup(curl);

	char* str = CCMainWindow::getFile("onlineReturn.txt");

	mutex.unlock();

	QByteArray Json = QByteArray(str);

	QJsonParseError err;
	QJsonDocument document = QJsonDocument::fromJson(Json, &err);
	if (!(err.error == QJsonParseError::NoError))
	{
		QMessageBox::warning(nullptr, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("Json�ļ�����ʧ��!"));
		return;
	}

	QJsonObject obj = document.object();
	int code = obj["code"].toInt();
	QJsonValue jsonValue = obj.value(QLatin1Literal("data"));
	obj = jsonValue.toObject();	
	if (code != 200)
		return;

	bool turn = false;
	turn = obj["your_turn"].toBool();
	if (turn && m_isGameAtFirst)
	{
		m_isGameAtFirst = false;
		mutex.lock();
		m_isOnPlayer2 = false;
		m_onPlayer1 = true;
		ui.userLabel->setText(QString::fromLocal8Bit("�û�:") + gId + QString::fromLocal8Bit("������") + "\n�ֵ�����");
		mutex.unlock();
	}	

	if (turn)
	{
		if (obj["last_msg"].toString() == QString::fromLocal8Bit("�Ծָտ�ʼ"))
			return;
		
		mutex.lock();
		m_isOnPlayer2 = true;
		mutex.unlock();
		int id;
		QString last_code = obj["last_code"].toString();
		id = analysisCode(last_code);
		
		if (last_code[2] == "0") // ����
		{
			emit playerOper(true, id);
			//onlineFlop(&m_player2Card, id);
		}
		else //����
		{
			emit playerOper(false, id);
			//onPlayerOutCard(&m_player2Card, id);
		}		
	}
}

// �ػ��¼�
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

		// ��ȡ��ǰ�û���Ƥ��RGBֵ
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

void GameWindow::onCardClicked(int id, POSITION position)
{

	if (m_isMousePressed)
		return;
	m_isMousePressed = true;	

	if (m_onPlayer1 && position == PLAYER1)
	{
		if (m_isOnline)
		{
			QString tmp;
			if (id % 4 == DIAMOND)
				tmp = "D";
			else if (id % 4 == SPADE)
				tmp = "S";
			else if (id % 4 == HEART)
				tmp = "H";
			else if (id % 4 == CLUB)
				tmp = "C";

			if ((id / 4 == 10 && id % 4 > 0) || (id / 4 == 11 && id % 4 == 0))
				tmp += "J";
			else if ((id / 4 == 11 && id % 4 > 0) || (id / 4 == 12 && id % 4 == 0))
				tmp += "Q";
			else if ((id / 4 == 12 && id % 4 > 0) || (id / 4 == 13 && id % 4 == 0))
				tmp += "K";
			else
				tmp += QString::number((id % 4 == 0) ? (id / 4) : (id / 4 + 1));

			FILE* fp;
			if ((fp = fopen("onlineReturn.txt", "w+")) == NULL)
				return;

			CURL* curl;
			CURLcode res;
			curl = curl_easy_init();
			if (curl) {
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
				curl_easy_setopt(curl, CURLOPT_URL, (QString("http://172.17.173.97:9000/api/game/") + gUuid +  QString("?type=1&card=") + tmp).toUtf8().data());
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
				curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
				struct curl_slist* headers = NULL;
				headers = curl_slist_append(headers, (QString("Authorization: Bearer ") + gToken).toUtf8().data());
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
				res = curl_easy_perform(curl);

				fclose(fp);
				if (res != 0)
					return;
			}
			curl_easy_cleanup(curl);

			char* str = CCMainWindow::getFile("onlineReturn.txt");
			QByteArray Json = QByteArray(str);

			QJsonParseError err;
			QJsonDocument document = QJsonDocument::fromJson(Json, &err);
			if (!(err.error == QJsonParseError::NoError))
			{
				QMessageBox::warning(nullptr, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("Json�ļ�����ʧ��!"));
				return;
			}

			QJsonObject obj = document.object();

			if (obj["msg"].toString() == QString::fromLocal8Bit("���󲻴���"))
			{
				QMessageBox::information(nullptr, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("û�ж���!"));
				return;
			}		

		}		
		onPlayerOutCard(&m_player1Card, id);	
		m_isMousePressed = false;
	}
	else if (m_onPlayer1 && position == DECK)
	{
		if (m_isOnline)
		{
			mutex.lock();
			FILE* fp;
			if ((fp = fopen("onlineReturn.txt", "w")) == NULL)
				return;

			CURL* curl;
			CURLcode res;
			curl = curl_easy_init();
			if (curl) {
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
				QByteArray put = ("http://172.17.173.97:9000/api/game/" + gUuid.toUtf8() + "?type=0");
				curl_easy_setopt(curl, CURLOPT_URL, put.data());
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
				curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
				struct curl_slist* headers = NULL;
				headers = curl_slist_append(headers, (QString("Authorization: Bearer ") + gToken).toUtf8().data());
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
				res = curl_easy_perform(curl);

				fclose(fp);
				if (res != 0)
					return;
			}
			curl_easy_cleanup(curl);			

			char* str = CCMainWindow::getFile("onlineReturn.txt");
			mutex.unlock();
			QByteArray Json = QByteArray(str);

			QJsonParseError err;
			QJsonDocument document = QJsonDocument::fromJson(Json, &err);
			if (!(err.error == QJsonParseError::NoError))
			{
				QMessageBox::warning(nullptr, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("Json�ļ�����ʧ��!"));
				return;
			}

			QJsonObject obj = document.object();
			QString msg = obj["msg"].toString();
			
			if (obj["msg"].toString() == QString::fromLocal8Bit("���󲻴���"))
			{
				QMessageBox::information(nullptr, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("û�ж���!"));
				return;
			}

			QJsonValue value = obj.value("data");
			obj = value.toObject();
			QString code = obj["last_code"].toString();
			// ��������
			int id = analysisCode(code);
			onlineFlop(&m_player1Card ,id);
			mutex.lock();
			m_onPlayer1 = false;
			mutex.unlock();
		}
		else
		{
			onPlayerFlop(&m_player1Card);
		}		
		m_isMousePressed = false;
	}
	// ������ͬһ̨�����������
	else if (!m_isRobot && !m_onPlayer1 && !m_isOnline)
	{
		if (position == PLAYER2)
		{
			onPlayerOutCard(&m_player2Card, id);
		}
		else if (position == DECK)
		{
			onPlayerFlop(&m_player2Card);
		}
	}


	if (m_deckCard.size() == 0)
		judgeWinner();

	wait(500);

	if (m_isRobot && !m_onPlayer1)
	{
		onRobotOper();
		if (m_deckCard.size() == 0)
			judgeWinner();
	}

	update();
	m_isMousePressed = false;
}
