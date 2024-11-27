#ifndef STATUSLOGS_H
#define STATUSLOGS_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <mutex>

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

        void setupLoggers() {
            std::ostringstream oss; 
            oss << "logs//Robview_General_" << ".txt";
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(oss.str(), true);
            logger = std::make_shared<spdlog::logger>("Robview Record", spdlog::sinks_init_list{ file_sink });
            logger->set_level(spdlog::level::info);
            logger->set_pattern("[%H:%M:%S] [%l] %v");

            spdlog::register_logger(logger);
        }


        void logDebug(const std::string& message) {
            logger->debug(message);
            addLogMessage("debug", QString::fromStdString(message), Qt::green);
        }

        void logInfo(const std::string& message) {
            logger->info(message);
            addLogMessage("info", QString::fromStdString(message), QColor(0.73 * 255, 0.68 * 255, 0.8 * 255));
        }

        void logWarning(const std::string& message) {
            logger->warn(message);
            addLogMessage("warning", QString::fromStdString(message), Qt::yellow);
        }

        void logNotify(const std::string& message) {
            logger->info(message);
            addLogMessage("notify", QString::fromStdString(message), QColor(1.0 * 255, 0.7 * 255, 0.0 * 255));
        }

        void logCritical(const std::string& message) {
            logger->critical(message);
            addLogMessage("critical", QString::fromStdString(message), Qt::red);
        }

        void clearLogs() {
            QMutexLocker locker(&mtx);
            if (plogOutputWidget) {
                plogOutputWidget->clear();
            }
        }

    private:
        statuslogs() {
            setupLoggers();
        }
        ~statuslogs() = default;

        statuslogs(const statuslogs&) = delete;
        statuslogs& operator=(const statuslogs&) = delete;

        void addLogMessage(const QString& prefix, const QString& message, const QColor& color) {
            // QMutexLocker locker(&mtx);
            QString displayedMessage = message;
            if (message.length() > 2000) {
                displayedMessage = message.left(500) + "\n..." + "[Too Long. Check the logs.txt]";
                logger->log(spdlog::level::info, message.toStdString()); 
                logger->flush();

            }

            if (plogOutputWidget) {
                plogOutputWidget->setTextColor(Qt::white);
                plogOutputWidget->insertPlainText(prefix + ": ");
                plogOutputWidget->setTextColor(color);
                plogOutputWidget->insertPlainText(displayedMessage + "\n");
                plogOutputWidget->moveCursor(QTextCursor::End);
                plogOutputWidget->setTextColor(Qt::white);
            }
        }

        std::shared_ptr<spdlog::logger> logger;
        QTextEdit* plogOutputWidget = nullptr;
        QMutex mtx;
    };

}



class ReadStatus
{
private:
    std::atomic<float> fps_allScan { 0 };
    std::atomic<float> throughout{ 0 };
    std::atomic<float> readingfps{ 0 };
    std::atomic<float> sviewfps{ 0 };

    ReadStatus() : throughout(0) {}
    ~ReadStatus() = default;

    ReadStatus(const ReadStatus&) = delete;
    ReadStatus& operator=(const ReadStatus&) = delete;

public:
    static ReadStatus& getinstance() {
        static ReadStatus instance;
        return instance;
    }

    void set_throughout(float value) { throughout = value; }/* Throughout*/
    void set_readPAUT(float value) { readingfps = value; }/* FPS*/
    void set_processData(float value) { fps_allScan = value; }
    void set_sviewfps(float value) { sviewfps = value; }

    float get_throughout() { return throughout.load(); } /* Throughout*/
    float get_readPAUT() { return readingfps.load(); }/* FPS*/
    float get_processData() { return fps_allScan.load(); }
    float get_sviewfps() { return sviewfps.load(); }
};


#endif // STATUSLOGS_H
