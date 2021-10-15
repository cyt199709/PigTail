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
#include <curl/curl.h>

//#include "CCMainWindow.h"

// cytcyt918
// 032092119 

QString gToken;
QString gId;

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

void UserLogin::initControl()
{
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);
	connect(ui.cancelBtn, &QPushButton::clicked, this, &BasicWindow::onButtonCloseClicked);
}

void UserLogin::onLoginBtnClicked()
{
 //   CURL* curl;
 //   CURLcode res;
 //   //QByteArray url = "http://172.17.173.97:8080/api/user/login?student_id=" + ui.accountLineEdit->text().toUtf8() + "&password=" + ui.passwordLineEdit->text().toUtf8();
 //   QByteArray url = "http://172.17.173.97:8080/api/user/login?student_id=032092119&password=cytcyt918"; //+ ui.accountLineEdit->text().toUtf8() + "&password=" + ui.passwordLineEdit->text().toUtf8();
 //   curl = curl_easy_init();
 //   FILE* fp;
 //   if ((fp = fopen("onlineReturn.txt", "w+")) == NULL)
 //       return;
 //   if (curl) {
 //       curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
 //       curl_easy_setopt(curl, CURLOPT_URL, /*url.data()*/"http://172.17.173.97:8080/api/user/login");
 //       curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
 //       curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
 //       curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);// 数据写入文件        
 //      
 //       struct curl_slist* headers = NULL;
 //       headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
 //       curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
 //       QByteArray pwd = ui.passwordLineEdit->text().toUtf8();
 //       QByteArray act = ui.accountLineEdit->text().toUtf8();
 //       //const char* data = "student_id=032092119&password=cytcyt918";
 //       //032092121  wzp123
 //       QByteArray tmp = ("student_id=" + act + "&password=" + pwd).data();
 //       const char* data = tmp.data();
 //       curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

 //       res = curl_easy_perform(curl);
 //       fclose(fp);
 //       if (res != 0)
 //           return ;       
 //       
 //   }
 //   curl_easy_cleanup(curl);

 //   char* str = CCMainWindow::getFile("onlineReturn.txt");
 //   QByteArray Json = QByteArray(str);
	//
	//QJsonParseError err;
	//QJsonDocument document = QJsonDocument::fromJson(Json, &err);
	//if (!(err.error == QJsonParseError::NoError))
	//{
	//	QMessageBox::warning(nullptr, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("Json文件解析失败!"));
	//	return;
	//}
	//
	//QJsonObject obj = document.object();
	//QJsonValue jsonValue = obj.value(QLatin1Literal("data"));
	//QJsonObject child = jsonValue.toObject();
	//gToken = child["token"].toString();
 //   jsonValue = child.value(QLatin1Literal("detail"));
 //   child = jsonValue.toObject();
 //   gId = child["student_id"].toString();

	close();
	CCMainWindow* mainwindow = new CCMainWindow;
	mainwindow->show();

}