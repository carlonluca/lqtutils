#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "../lqtutils_settings.h"
#include "../lqtutils_string.h"
#include "../lqtutils_prop.h"

#include "ltestsignals.h"
#include "lquicksettings.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    LTestSignals test;
    engine.rootContext()->setContextProperty("settings", &LQuickSettings::instance());
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
}
