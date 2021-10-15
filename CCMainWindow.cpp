#include "CCMainWindow.h"
#include "GameWindow.h"
#include "CommonUtils.h"
#include <QNetworkAccessManager>
#include <QJsonParseError>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
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
	m_dialog->setModal(true);
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
	/*m_dialog->close();
	this->close();*/


	GameWindow* game = new GameWindow(nullptr, false, true);
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
		gUuid = m_uid;
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
	QString msg = obj["msg"].toString();
	

	if (msg != QString::fromLocal8Bit("操作成功"))
	{
		QMessageBox::warning(nullptr, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("连接失败！"));
		return;
	}

	GameWindow* gameWindow = new GameWindow(nullptr, false, true);
	gameWindow->show();
}

void CCMainWindow::getGmaeList(bool)
{
	updateGameList(1);

	QDialog* dialog = new QDialog(m_dialog);

	QPushButton* nextBtn = new QPushButton(QString::fromLocal8Bit("下一页"));
	QPushButton* previousBtn = new QPushButton(QString::fromLocal8Bit("上一页"));
	QLineEdit* pageLineEdit = new QLineEdit;
	pageLineEdit->setValidator(new QIntValidator(1, m_totalPage));
	pageLineEdit->setText("1");
	QLabel* label = new QLabel(QString::fromLocal8Bit("当前页:") + pageLineEdit->text() + QString::fromLocal8Bit("   总:") + QString::number(m_totalPage));

	QTableView* tableView = new QTableView;
	tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	QStandardItemModel* model = new QStandardItemModel(tableView);
	model->setColumnCount(4);
	model->setHeaderData(0, Qt::Horizontal, "uuid");
	model->setHeaderData(1, Qt::Horizontal, "host_id");
	model->setHeaderData(2, Qt::Horizontal, "client_id");
	model->setHeaderData(3, Qt::Horizontal, "created_at");

	model->setRowCount(10);
	for (int i = 0; i < 10; i++)
	{
		model->setHeaderData(i, Qt::Vertical, i + 1);
	}

	QHBoxLayout* hLayout = new QHBoxLayout;
	QVBoxLayout* vLayout = new QVBoxLayout;
	hLayout->addWidget(previousBtn);
	hLayout->addWidget(pageLineEdit);
	hLayout->addWidget(nextBtn);
	hLayout->addWidget(label);
	vLayout->addWidget(tableView);
	vLayout->addLayout(hLayout);
	dialog->setLayout(vLayout);
	updateGameList(1, model);
	tableView->setModel(model);
	dialog->show();

	connect(previousBtn, &QPushButton::clicked, this, [model, this, pageLineEdit, tableView]() {
		int page = pageLineEdit->text().toInt() - 1;
		if (page < 1)
			return;
		updateGameList(page, model);
		pageLineEdit->setText(QString::number(page));
		tableView->setModel(model);
	});
	connect(pageLineEdit, &QLineEdit::textChanged, this, [model, this, pageLineEdit, tableView]() {
		int page = pageLineEdit->text().toInt();
		updateGameList(page, model);	
		tableView->setModel(model);
	});
	connect(nextBtn, &QPushButton::clicked, this, [model, this, pageLineEdit, tableView]() {
		int page = pageLineEdit->text().toInt() + 1;
		if (page > m_totalPage)
			return;
		updateGameList(page, model);
		pageLineEdit->setText(QString::number(page));
		tableView->setModel(model);
	});

	

}

void CCMainWindow::setPrivate(int state)
{
	m_isPrivate = state;
}

void CCMainWindow::updateGameList(int page_num, QStandardItemModel* model)
{
	CURL* curl;
	CURLcode res;

	FILE* fp;
	if ((fp = fopen("onlineReturn.txt", "w+")) == NULL)
		return;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(curl, CURLOPT_URL, ("http://172.17.173.97:9000/api/game/index?page_size=10&page_num=" + QByteArray::number(page_num)).data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, (QString("Authorization: Bearer ") + gToken).toUtf8().data());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		res = curl_easy_perform(curl);
		fclose(fp);
		if (res != 0)
			return;
	}
	curl_easy_cleanup(curl);

	char* str = getFile("onlineReturn.txt");
	QByteArray Json = QByteArray(str);

	QJsonParseError err;
	QJsonDocument document = QJsonDocument::fromJson(Json, &err);
	if (!(err.error == QJsonParseError::NoError))
	{
		QMessageBox::warning(nullptr, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("Json文件解析失败!"));
		return;
	}

	QJsonObject obj = document.object();
	QJsonValue value = obj.value(QLatin1String("data"));
	obj = value.toObject();
	m_total = obj["total"].toInt();
	m_totalPage = obj["total_page_num"].toInt();
	value = obj.value(QLatin1String("games"));
	QJsonArray jsonarry = value.toArray();

	if (model == nullptr)
		return;

	for (int i = 0; i < model->rowCount(); i++)
	{
		value = jsonarry.at(i);
		obj = value.toObject();
		
		model->setItem(i, 0, new QStandardItem(obj["uuid"].toString()));
		model->setItem(i, 1, new QStandardItem(QString::number(obj["host_id"].toInt())));
		model->setItem(i, 2, new QStandardItem(QString::number(obj["client_id"].toInt())));
		model->setItem(i, 3, new QStandardItem(obj["created_at"].toString()));
	}

}

