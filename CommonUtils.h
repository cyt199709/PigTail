#pragma once

#include <QProxyStyle>

class CustomProxyStyle : public QProxyStyle
{

public:
	CustomProxyStyle(QObject *parent = nullptr);
	~CustomProxyStyle();

	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = 0) const;
};

class CommonUtils
{
public:
	CommonUtils();

public:
	static void loadStyleSheet(QWidget* widget, const QString& sheetName);
};