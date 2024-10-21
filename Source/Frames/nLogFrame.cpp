#include "nLogFrame.h"
#include "../pch.h"
QWidget* nLogFrame::createFrame()
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

void nLogFrame::update()
{
	if (!sttlogs) {
		sttlogs = &nmainUI::statuslogs::getinstance();
	}
	if (sttlogs) {
		sttlogs->startLoggingToFile();
	}
}
