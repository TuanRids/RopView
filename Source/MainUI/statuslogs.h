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
        void addLogMessage(const QString& prefix, const QString& message, const QColor& color) {
            QMutexLocker locker(&mtx);
            if (plogOutputWidget) {
                plogOutputWidget->setTextColor(Qt::white);
                plogOutputWidget->insertPlainText(prefix);  
                plogOutputWidget->setTextColor(color);
                plogOutputWidget->insertPlainText(message);  
                plogOutputWidget->insertPlainText("\n");
                plogOutputWidget->moveCursor(QTextCursor::End);
                plogOutputWidget->setTextColor(Qt::white);  
            }

            if (filelog.isOpen()) {
                QTextStream out(&filelog);
                out << prefix << message << "\n";
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
            addLogMessage( crtime() + ": [debug] " , qmessage, Qt::green);
        }
        void logInfo(const std::string& message) {
            QString qmessage = QString::fromStdString(message);
            addLogMessage( crtime() + ": [info] " , qmessage, QColor(0.73 * 255, 0.68 * 255, 0.8 * 255));
        }
        void logWarning(const std::string& message) {
            QString qmessage = QString::fromStdString(message);
            addLogMessage(crtime() + ": [warning] " , qmessage, Qt::yellow);
        }
        void logNotify(const std::string& message) {
            QString qmessage = QString::fromStdString(message);
            addLogMessage(crtime() + ": [notify] " , qmessage, QColor(1.0 * 255, 0.7 * 255, 0.0 * 255));
        }
        void logCritical(const std::string& message) {
            QString qmessage = QString::fromStdString(message);
            addLogMessage( crtime() + ": [critical] " , qmessage, Qt::red);
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
        std::shared_ptr<spdlog::logger> spdLoggers() { return file_logger; }
    private:
        QString crtime(){return QTime::currentTime().toString("hh:mm:ss");}
        statuslogs() : file_logger(nullptr) {
            std::shared_ptr<spdlog::logger> file_logger = spdlog::basic_logger_mt("file_logger", "log.txt");
        };
        ~statuslogs() = default;

        statuslogs(const statuslogs&) = delete;
        statuslogs& operator=(const statuslogs&) = delete;

        std::shared_ptr<spdlog::logger> file_logger;

        QTextEdit* plogOutputWidget = nullptr;
        QFile filelog;
        QMutex mtx;
        std::unique_ptr<QString> upFilePath;
    };

}

#endif // STATUSLOGS_H
