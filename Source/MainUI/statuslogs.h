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

        void initialize(QTextEdit* logOutputWidget) {
            if (!this->logOutputWidget) {
                this->logOutputWidget = logOutputWidget;
                logOutputWidget->setReadOnly(true);
            }
        }

        void addLogMessage(const QString& message, const QColor& color) {
            QMutexLocker locker(&mutex);
            if (logOutputWidget) {
                logOutputWidget->setTextColor(color);  
                logOutputWidget->append(message);  
            }
            if (logFile.isOpen()) {
                QTextStream out(&logFile);
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
            QMutexLocker locker(&mutex);
            if (logOutputWidget) {
                logOutputWidget->clear();
            }
        }

        void startLoggingToFile() {
            if (!filePath) { filePath = std::make_unique<QString>("application.log"); }
            QMutexLocker locker(&mutex);
            if (!logFile.isOpen()) {
                logFile.setFileName(*filePath);
                if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
                    logCritical("Unable to open log file for writing.");
                }
            }
        }

        void stopLoggingToFile() {
            QMutexLocker locker(&mutex);
            if (logFile.isOpen()) {
                logFile.close();
            }
        }

    private:
        QString crtime(){return QTime::currentTime().toString("hh:mm:ss");}
        statuslogs() = default;
        ~statuslogs() = default;

        statuslogs(const statuslogs&) = delete;
        statuslogs& operator=(const statuslogs&) = delete;

        QTextEdit* logOutputWidget = nullptr;
        QFile logFile;
        QMutex mutex;
        std::unique_ptr<QString> filePath;
    };

}

#endif // STATUSLOGS_H
