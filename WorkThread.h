#pragma once

#include <QThread>

class GameWindow;

class WorkThread : public QThread
{
	Q_OBJECT

public:
	WorkThread(GameWindow* wnd = nullptr);
	~WorkThread();

protected:
	void run();

private:
	GameWindow* m_gamewindow;
};
