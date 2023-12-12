#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QLoggingCategory>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>

#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Foundation.h>

QString getUwpAppDataLocation() {
    try {
        auto localFolder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
        std::wstring folderPath = localFolder.Path().c_str();
        return QString::fromStdWString(folderPath);
    } catch (const std::exception& e) {
        qWarning() << "Failed to get the Qt 5 AppData path:" << e.what();
        return QString();
    }
}

QString logFileTimestamp;
QString appDataLocation;

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString formattedMsg = QString("%1: %2 (%3:%4, %5)\n")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                               .arg(localMsg.constData())
                               .arg(context.file)
                               .arg(context.line)
                               .arg(context.function);

    QString dirPath = appDataLocation + "/Qt6-CMake-QML";
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString filePath = dirPath + "/OutputLog " + logFileTimestamp + ".txt";
    QFile outFile(filePath);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << formattedMsg;
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    // Generate the timestamp once when the application starts
    logFileTimestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");

    appDataLocation = getUwpAppDataLocation();
    if (appDataLocation.isEmpty())
        appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // Set up the custom message handler
    qInstallMessageHandler(customMessageHandler);

    QLoggingCategory::setFilterRules("qt*=true");

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/Primary/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
