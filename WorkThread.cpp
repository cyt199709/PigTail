#include <QMutex>
#include "WorkThread.h"
#include "GameWindow.h"

static QMutex mutex;

WorkThread::WorkThread(GameWindow* wnd)
{
	m_gamewindow = wnd;
}

WorkThread::~WorkThread()
{
}

void WorkThread::run()
{
	while(1)
		m_gamewindow->getMsgInServer();
}
