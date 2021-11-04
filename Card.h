#pragma once

#include <QLabel>

enum CARDTYPE {
	BACK = 0,
	POSITIVE
};

enum POSITION {
	DECK = 0,
	CEMETERY,
	PLAYER1,
	PLAYER2
};

class Card : public QLabel
{
	Q_OBJECT

public:
	Card(QWidget *parent, POSITION position, int id = 0, CARDTYPE type = POSITIVE);
	~Card();

	int getId() const;
	void setPosition(POSITION position);
	void setCardType(CARDTYPE cardType);
	void setId(int id);

private:
	void initControl(CARDTYPE type = POSITIVE, QString pixPath = "");

protected:
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void keyPressEvent(QKeyEvent* event);

signals:
	void clicked(int, POSITION);

private:
	int m_id;// id
	CARDTYPE m_type;
	POSITION m_position;
};
