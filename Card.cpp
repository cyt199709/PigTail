#include <QMouseEvent>
#include "Card.h"

Card::Card(QWidget *parent, POSITION position, int id, CARDTYPE type)
	: QLabel(parent)
	, m_id(id)
	, m_type(type)
	, m_position(position)
{
	initControl(m_type, QString(":/Resources/%1.png").arg(m_id));
	setFixedSize(250, 350);
}

Card::~Card()
{
}

int Card::getId() const
{
	return m_id;
}

void Card::setPosition(POSITION position)
{
	m_position = position;
}

void Card::setCardType(CARDTYPE cardType)
{
	m_type = cardType;
	if (m_type == BACK)
	{
		this->setPixmap(QPixmap(":/Resources/0.png"));
	}
	else
	{
		this->setPixmap(QPixmap(QString(":/Resources/%1.png").arg(m_id)));
	}
	
	this->setScaledContents(true);
}

void Card::setId(int id)
{
	m_id = id;
}

void Card::initControl(CARDTYPE type, QString pixPath)
{
	if (type == BACK || m_position == PLAYER2)
	{
		pixPath = ":/Resources/0.png";
	}
	
	QPixmap pixmap = QPixmap(pixPath);

	this->setPixmap(pixmap);
	this->setScaledContents(true);
}

void Card::mouseReleaseEvent(QMouseEvent* event)
{
	event->accept();
	emit clicked(m_id, m_position);
}

void Card::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Tab)
	{
		return;
	}
	QLabel::keyPressEvent(event);
}
