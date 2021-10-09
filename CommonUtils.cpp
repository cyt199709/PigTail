#include <QFile>
#include <QWidget>
#include <QProxyStyle>
#include "CommonUtils.h"

CustomProxyStyle::CustomProxyStyle(QObject *parent)
{
	setParent(parent);
}

CustomProxyStyle::~CustomProxyStyle()
{
}

void CustomProxyStyle::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
	if (PE_FrameFocusRect == element)
	{
		// 去掉windows中部件默认的边框或虚线框, 部件获取焦点时直接返回, 不进行绘制
		return;
	}
	else
	{
		QProxyStyle::drawPrimitive(element, option, painter, widget);
	}
}

CommonUtils::CommonUtils()
{
}

void CommonUtils::loadStyleSheet(QWidget* widget, const QString& sheetName)
{
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen())
	{
		widget->setStyleSheet("");
		QString qsstyleSheet = QLatin1String(file.readAll());
		widget->setStyleSheet(qsstyleSheet);
	}
	file.close();
}
