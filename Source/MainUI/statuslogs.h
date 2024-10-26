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
            oss << "logs//Robview_Genelal_" << ".txt";
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
            if (plogOutputWidget) {
                plogOutputWidget->setTextColor(Qt::white);
                plogOutputWidget->insertPlainText(prefix + ": ");
                plogOutputWidget->setTextColor(color);
                plogOutputWidget->insertPlainText(message + "\n");
                plogOutputWidget->moveCursor(QTextCursor::End);
                plogOutputWidget->setTextColor(Qt::white);
            }
        }
        std::shared_ptr<spdlog::logger> logger;
        QTextEdit* plogOutputWidget = nullptr;
        QMutex mtx;
    };

}



class spdThoughout
{
private:
    std::atomic<float> throughout;
    std::atomic<float> fps_allScan;

    spdThoughout() : throughout(0) {}
    ~spdThoughout() = default;

    spdThoughout(const spdThoughout&) = delete;
    spdThoughout& operator=(const spdThoughout&) = delete;

public:
    static spdThoughout& getinstance() {
        static spdThoughout instance;
        return instance;
    }

    void set(float value) { throughout = value; }/* Throughout*/
    void set_Fps(float value) { fps_allScan = value; }/* FPS*/
    float get() { return throughout.load(); } /* Throughout*/
    float get_FPS() { return fps_allScan.load(); }/* FPS*/
};
#endif // STATUSLOGS_H
