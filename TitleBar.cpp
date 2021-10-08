#include "titlebar.h"
#include <QBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>

#define BUTTON_HEIGHT 27 // 按钮高度
#define BUTTON_WIDTH  27 // 按钮宽度
#define TITLE_HEIGHT  27 // 标题栏高度

TitleBar::TitleBar(QWidget* parent)
	: QWidget(parent)
	, m_isPressed(false)
	, m_buttonType(MIN_MAX_BUTTON)
{
	initControl();
	initConnections();
	loadStyleSheet("Title");
}

TitleBar::~TitleBar()
{

}

void TitleBar::initControl()
{
	m_pTitleContent = new QLabel(this);

	m_pButtonMin = new QPushButton(this);
	m_pButtonRestore = new QPushButton(this);
	m_pButtonMax = new QPushButton(this);
	m_pButtonClose = new QPushButton(this);

	// 按钮设置固定大小
	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

	// 设置对象名
	m_pTitleContent->setObjectName("TitleContent");
	m_pButtonMin->setObjectName("ButtonMin");
	m_pButtonMax->setObjectName("ButtonMax");
	m_pButtonRestore->setObjectName("ButtonRestore");
	m_pButtonClose->setObjectName("ButtonClose");

	// 标题栏设置布局
	QHBoxLayout* myLayout = new QHBoxLayout(this);
	//myLayout->addWidget(m_pIcon);
	myLayout->addWidget(m_pTitleContent);

	myLayout->addWidget(m_pButtonMin);
	myLayout->addWidget(m_pButtonRestore);
	myLayout->addWidget(m_pButtonMax);
	myLayout->addWidget(m_pButtonClose);

	myLayout->setContentsMargins(5, 0, 0, 0);	// 设置布局的间隙
	myLayout->setSpacing(0);					// 设置部件间的间隙

	// 标题栏的尺寸策略
	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_pTitleContent->setAlignment(Qt::AlignCenter);
	setFixedHeight(TITLE_HEIGHT);
	setWindowFlags(Qt::FramelessWindowHint);	// 设置无边框


}

void TitleBar::initConnections()
{
	connect(m_pButtonMin, &QPushButton::clicked, this, &TitleBar::onButtonMinClicked);
	connect(m_pButtonMax, &QPushButton::clicked, this, &TitleBar::onButtonMaxClicked);
	connect(m_pButtonRestore, &QPushButton::clicked, this, &TitleBar::onButtonRestoreClicked);
	connect(m_pButtonClose, &QPushButton::clicked, this, &TitleBar::onButtonCloseClicked);
}

// 设置标题栏的内容
void TitleBar::setTitleContent(const QString& titleContent)
{
	QFont ft;
	ft.setPointSize(16);
	m_pTitleContent->setFont(ft);

	QPalette pa;
	pa.setColor(QPalette::WindowText, Qt::black);
	m_pTitleContent->setPalette(pa);

	m_pTitleContent->setText(titleContent);


	m_titleContent = titleContent;
}

// 设置标题栏的长度
void TitleBar::setTitleWidth(int width)
{
	setFixedWidth(width);
}

// 设置标题栏按钮类型
void TitleBar::setButtonType(ButtonType buttonType)
{
	m_buttonType = buttonType;

	switch (buttonType)
	{
	case MIN_BUTTON:
		m_pButtonRestore->setVisible(false);
		m_pButtonMax->setVisible(false);
		break;
	case MIN_MAX_BUTTON:
		m_pButtonRestore->setVisible(false);
		break;
	case ONLY_CLOSE_BUTTON:
		m_pButtonRestore->setVisible(false);
		m_pButtonMax->setVisible(false);
		m_pButtonMin->setVisible(false);
		break;
	default:
		break;
	}
}

// 保存窗口最大化前窗口的位置及大小
void TitleBar::saveRestoreInfo(const QPoint& point, const QSize& size)
{
	m_restorePos = point;
	m_restoreSize = size;
}

// 获取窗口最大化前窗口位置及大小
void TitleBar::getRestoreInfo(QPoint& point, QSize& size)
{
	point = m_restorePos;
	size = m_restoreSize;
}

// 绘制标题栏
void TitleBar::paintEvent(QPaintEvent* event)
{
	// 设置背景色
	QPainter painter(this);
	QPainterPath pathBack;
	pathBack.setFillRule(Qt::WindingFill); // 设置填充规则
	pathBack.addRoundedRect(QRect(0, 0, width(), height()), 3, 3); // 添加圆角矩形到绘图路径
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

	// 当窗口最大化或还原后，窗口长度改变标题栏相应改变
	// parentWidget() 返回父部件
	if (width() != parentWidget()->width())
	{
		setFixedWidth(parentWidget()->width());
	}
	QWidget::paintEvent(event);
}

// 双击响应事件, 主要实现双击标题栏实现最大化最小化操作
void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	// 只有存在最大化，按钮才有效
	if (m_buttonType == MIN_MAX_BUTTON)
	{
		if (m_pButtonMax->isVisible())
			onButtonMaxClicked();
		else
			onButtonRestoreClicked();
	}

	return QWidget::mouseDoubleClickEvent(event);
}

// 通过鼠标按下，移动，释放，实现拖动标题栏移动达到移动窗口
void TitleBar::mousePressEvent(QMouseEvent* event)
{
	if (m_buttonType == MIN_MAX_BUTTON)
	{
		// 在窗口最大化时禁止拖动窗口
		if (m_pButtonMax->isVisible())
		{
			m_isPressed = true;
			m_startMovePos = event->globalPos(); // 返回事件发生时，鼠标的全局位置
		}
	}
	else
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}

	return QWidget::mousePressEvent(event);
}

// 鼠标移动
void TitleBar::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint widgetPos = parentWidget()->pos();
		m_startMovePos = event->globalPos();
		parentWidget()->move(widgetPos + movePoint);
	}

	return QWidget::mouseMoveEvent(event);
}

// mouseRelease
void TitleBar::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
	return QWidget::mouseReleaseEvent(event);
}

// 加载样式表
void TitleBar::loadStyleSheet(const QString& sheetName)
{
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen())
	{
		QString styleSheet = this->styleSheet();
		styleSheet += QLatin1String(file.readAll());
		setStyleSheet(styleSheet);
	}
	file.close();
}

void TitleBar::onButtonMinClicked()
{
	emit signalButtonMinClicked();
}

void TitleBar::onButtonRestoreClicked()
{
	m_pButtonRestore->setVisible(false);
	m_pButtonMax->setVisible(true);
	emit signalButtonRestoreClicked();
}

void TitleBar::onButtonMaxClicked()
{
	m_pButtonRestore->setVisible(true);
	m_pButtonMax->setVisible(false);
	emit signalButtonMaxClicked();
}

void TitleBar::onButtonCloseClicked()
{
	emit signalButtonCloseClicked();
}