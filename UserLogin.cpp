#include "UserLogin.h"
#include "CCMainWindow.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QMessageBox>

//#include "CCMainWindow.h"

// cytcyt918
// 032092119 

QString gToken;

UserLogin::UserLogin(QWidget* parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle(QString::fromLocal8Bit("猪尾巴"), "");
	loadStyleSheet("login");
	initControl();
}

UserLogin::~UserLogin()
{
}

void UserLogin::finishedSlot(QNetworkReply* reply)
{

	if (reply->error() == QNetworkReply::NoError)
	{
		QString value = QString::fromLocal8Bit(reply->readAll());
		QJsonParseError err;
		QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(), &err);
		if (!(err.error == QJsonParseError::NoError))
		{
			QMessageBox::warning(nullptr, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("Json文件解析失败!"));
			return;
		}
		
		QJsonObject obj = document.object();
		QJsonValue jsonValue = obj.value(QLatin1Literal("data"));
		QJsonObject child = jsonValue.toObject();
		gToken = child["token"].toString();

		close();
		CCMainWindow* mainwindow = new CCMainWindow;
		mainwindow->show();

	}
	else
	{
		qDebug() << "finishedSlot errors here";
		qDebug("found error .... code: %d\n", (int)reply->error());
		qDebug(qPrintable(reply->errorString()));
		QMessageBox::warning(nullptr, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请求服务器数据失败!"));
	}
	reply->deleteLater();
}

void UserLogin::initControl()
{
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);
	connect(ui.cancelBtn, &QPushButton::clicked, this, &BasicWindow::onButtonCloseClicked);
}

void UserLogin::onLoginBtnClicked()
{
	QNetworkAccessManager* accessManager = new QNetworkAccessManager(this);
	connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));

	QByteArray post = (QString("student_id = %1 & password = %2").arg(ui.accountLineEdit->text()).arg(ui.passwordLineEdit->text())).toUtf8();

	//QMessageBox::information(nullptr, "Tips", post);

	QNetworkRequest request;
	request.setUrl(QUrl("http://172.17.173.97:8080/api/user/login"));

	QNetworkReply* reply = accessManager->post(request, "student_id=032092119&password=cytcyt918"/*post*/);

}