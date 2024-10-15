#ifndef STATUSLOGS_H
#define STATUSLOGS_H

#include "..\pch.h"
namespace nmainUI {

    class statuslogs : public QObject {
    public:
        static statuslogs& getinstance() {
            static statuslogs instance;
            return instance;
        }
        void initialize(QTextEdit* plogOutputWidget) {
            if (!this->plogOutputWidget) {
                this->plogOutputWidget = plogOutputWidget;
                plogOutputWidget->setReadOnly(true);
            }
        }
        void addLogMessage(const QString& message, const QColor& color) {
            QMutexLocker locker(&mtx);
            if (plogOutputWidget) {
                plogOutputWidget->setTextColor(color);  
                plogOutputWidget->append(message);  
            }
            if (filelog.isOpen()) {
                QTextStream out(&filelog);
                out << message << "\n";
            }
        }
        void logDebug(const std::string& message) {
            QString qmessage = QString::fromStdString(message);
            addLogMessage( crtime() + ": [DEBUG] " + qmessage, Qt::gray);
        }
        void logInfo(const std::string& message) {
            QString qmessage = QString::fromStdString(message);
            addLogMessage( crtime() + ": [INFO] " + qmessage, Qt::cyan);
        }
        void logWarning(const std::string& message) {
            QString qmessage = QString::fromStdString(message);
            addLogMessage(crtime() + ": [WARNING] " + qmessage, Qt::yellow);
        }
        void logCritical(const std::string& message) {
            QString qmessage = QString::fromStdString(message);
            addLogMessage( crtime() + ": [CRITICAL] " + qmessage, Qt::red);
        }
        void clearLogs() {
            QMutexLocker locker(&mtx);
            if (plogOutputWidget) {
                plogOutputWidget->clear();
            }
        }
        void startLoggingToFile() {
            if (!upFilePath) { upFilePath = std::make_unique<QString>("application.log"); }
            QMutexLocker locker(&mtx);
            if (!filelog.isOpen()) {
                filelog.setFileName(*upFilePath);
                if (!filelog.open(QIODevice::Append | QIODevice::Text)) {
                    logCritical("Unable to open log file for writing.");
                }
            }
        }
        void stopLoggingToFile() {
            QMutexLocker locker(&mtx);
            if (filelog.isOpen()) {
                filelog.close();
            }
        }
    private:
        QString crtime(){return QTime::currentTime().toString("hh:mm:ss");}
        statuslogs() = default;
        ~statuslogs() = default;

        statuslogs(const statuslogs&) = delete;
        statuslogs& operator=(const statuslogs&) = delete;

        QTextEdit* plogOutputWidget = nullptr;
        QFile filelog;
        QMutex mtx;
        std::unique_ptr<QString> upFilePath;
    };

}

#endif // STATUSLOGS_H
