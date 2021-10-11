#include "CCMainWindow.h"
#include "GameWindow.h"
#include "CommonUtils.h"
#include <QNetworkAccessManager>
#include <QJsonParseError>
#include <QMessageBox>
#include <QJsonObject>
#include <QFile>
#include <QSslKey>
#include <curl/curl.h>
#include <QBoxLayout>
#include <QLineEdit>
#include <QCheckBox>

extern QString gToken;
extern QString gId;
QString gUuid;

CCMainWindow::CCMainWindow(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	//setTitleBarTitle(QString::fromLocal8Bit("猪尾巴"), "");
	loadStyleSheet("CCMainWindow");
	ui.userNameLabel->setText(QString::fromLocal8Bit("用户名:") + gId);
	m_isPrivate = false;
	//initControl();
}

CCMainWindow::~CCMainWindow()
{

}

char* CCMainWindow::getFile(const char* filename)
{
	//将文件指针移到末尾
	FILE* fp;
	if ((fp = fopen(filename, "r")) == NULL)
		return "";
	fseek(fp, 0, SEEK_END);
	char tmp[100];
	int filesize = ftell(fp);//通过ftell函数获得指针到文件头的偏移字节数。

	char* str = new char[filesize];
	memset(str, 0, filesize);
	rewind(fp);

	while ((fgets(tmp, 100, fp)) != NULL) {//循环读取100字节,如果没有数据则退出循环
		strcat(str, tmp);//拼接字符串
	}
	fclose(fp);
	return str;
}

void CCMainWindow::on_PvEBtn_clicked()
{
	close();
	GameWindow* gameWindow = new GameWindow(nullptr, true, false);
	gameWindow->show();
}

void CCMainWindow::on_onLocalPvPBtn_clicked()
{
	close();
	GameWindow* gameWindow = new GameWindow(nullptr, false, false);
	gameWindow->show();
}

void CCMainWindow::on_onlinePvPBtn_clicked()
{
	m_dialog = new QDialog(this); 
	QHBoxLayout* hLayout = new QHBoxLayout;
	QHBoxLayout* h1Layout = new QHBoxLayout;
	QVBoxLayout* vLayout = new QVBoxLayout;
	QLineEdit* uidLineEdit = new QLineEdit(m_dialog);
	QLabel* label = new QLabel(QString::fromLocal8Bit("uid:"));
	uidLineEdit->setObjectName("uidLineEdit");
	uidLineEdit->setPlaceholderText(QString::fromLocal8Bit("输入待加入对局的uid"));

	QPushButton* createBtn = new QPushButton(QString::fromLocal8Bit("创建对战"));
	QPushButton* joinBtn = new QPushButton(QString::fromLocal8Bit("加入对战"));
	QPushButton* getUidBtn = new QPushButton(QString::fromLocal8Bit("获取对局列表"));
	QCheckBox* privateGame = new QCheckBox(QString::fromLocal8Bit("私人对局"));
	createBtn->setObjectName("createBtn");
	joinBtn->setObjectName("joinBtn");
	getUidBtn->setObjectName("getUidBtn");
	privateGame->setObjectName("privateGame");
	hLayout->addWidget(createBtn);
	hLayout->addWidget(privateGame);
	hLayout->addWidget(joinBtn);
	hLayout->addWidget(getUidBtn);
	h1Layout->addWidget(label);
	h1Layout->addWidget(uidLineEdit);
	vLayout->addLayout(h1Layout);
	vLayout->addLayout(hLayout);
	m_dialog->setLayout(vLayout);

	connect(privateGame, &QCheckBox::stateChanged, this, &CCMainWindow::setPrivate);
	connect(createBtn, &QPushButton::clicked, this, &CCMainWindow::createOnlineGame);	
	connect(joinBtn, &QPushButton::clicked, this, [this, uidLineEdit]() {
		if (uidLineEdit->text().isEmpty())
			return;
		m_uid = uidLineEdit->text().toUtf8();
	});
	connect(joinBtn, &QPushButton::clicked, this, &CCMainWindow::joinOnlineGame);
	connect(getUidBtn, &QPushButton::clicked, this, &CCMainWindow::getGmaeList);

	m_dialog->show();	
}

void CCMainWindow::createOnlineGame(bool)
{
	CURL* curl;
	CURLcode res;
	FILE* fp;
	if ((fp = fopen("onlineReturn.txt", "w+")) == NULL)
		return;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		if(m_isPrivate)
			curl_easy_setopt(curl, CURLOPT_URL, "http://172.17.173.97:9000/api/game?private=true");
		else
			curl_easy_setopt(curl, CURLOPT_URL, "http://172.17.173.97:9000/api/game?private=false");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, ("Authorization: Bearer " + gToken).toUtf8().data());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);// 数据写入文件      

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
		QMessageBox::warning(nullptr, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("Json文件解析失败!"));
		return;
	}

	QJsonObject obj = document.object();
	QJsonValue jsonValue = obj.value(QLatin1Literal("data"));
	QJsonObject child = jsonValue.toObject();
	gUuid = child["uuid"].toString();

	GameWindow* game = new GameWindow(this, false, true);
	game->show();
}

void CCMainWindow::joinOnlineGame(bool)
{
	if (m_uid.isEmpty())
		return;

	FILE* fp;
	if ((fp = fopen("onlineReturn.txt", "w+")) == NULL)
		return;

	CURL* curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, (QByteArray("http://172.17.173.97:9000/api/game/") + m_uid).data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, (QByteArray("Authorization: Bearer ") + gToken.toUtf8()).data());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
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
		QMessageBox::warning(nullptr, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("Json文件解析失败!"));
		return;
	}

	QJsonObject obj = document.object();
	QString msg = obj["student_id"].toString();

	if (msg != QString::fromLocal8Bit("操作成功"))
	{
		QMessageBox::warning(nullptr, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("连接失败！"));
		return;
	}

	GameWindow* gameWindow = new GameWindow(this, false, true);
	gameWindow->show();
}

void CCMainWindow::getGmaeList(bool)
{

}

void CCMainWindow::setPrivate(int state)
{
	m_isPrivate = state;
}
