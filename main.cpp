#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QLoggingCategory>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>

QString logFileTimestamp;

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString formattedMsg = QString("%1: %2 (%3:%4, %5)\n")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                               .arg(localMsg.constData())
                               .arg(context.file)
                               .arg(context.line)
                               .arg(context.function);

    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Qt6-CMake-QML Log";
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
