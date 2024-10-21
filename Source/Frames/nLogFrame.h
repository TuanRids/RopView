#ifndef NLOGFRAME_H
#define NLOGFRAME_H

#include "pch.h"
#include "MainUI/ObserverMgr.h"
#include "MainUI/mainwindow.h"
#include "MainUI/statuslogs.h"


class nLogFrame : public nFrame, public nObserver {
private:
	nmainUI::statuslogs* sttlogs;
public:
	QWidget* createFrame();
	void update()  override;
};
#endif

