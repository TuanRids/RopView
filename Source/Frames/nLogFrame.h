#ifndef NLOGFRAME_H
#define NLOGFRAME_H
#include "..\pch.h"

#include "MainUI/statuslogs.h"
#include "MainUI/FactoryMgr.h"
#include "MainUI/ObserverMgr.h"

// Log Frame
class nLogFrame : public nFrame, public nObserver {
private:
	static nmainUI::statuslogs* sttlogs;
public:
	QWidget* createFrame() override
	{
		QTextEdit* logWidget = new QTextEdit();
		logWidget->setLineWrapMode(QTextEdit::NoWrap);
		logWidget->setReadOnly(true);
		if (!sttlogs) {  
			sttlogs = &nmainUI::statuslogs::getinstance();
		}

		sttlogs->initialize(logWidget);
		return logWidget;
	}
	// Observer pattern
	void update() override {		 
		if (sttlogs) {
			sttlogs->startLoggingToFile();
		}
	}
};
#endif

