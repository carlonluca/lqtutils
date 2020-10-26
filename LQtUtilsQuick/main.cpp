#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "../lqtutils_settings.h"
#include "../lqtutils_string.h"
#include "../lqtutils_prop.h"

#include "ltestsignals.h"
#include "lquicksettings.h"
#include "lenums.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    qmlRegisterUncreatableMetaObject(MyEnum::staticMetaObject, "com.luke", 1, 0, "MyEnum", "Access to enums & flags only");
    engine.rootContext()->setContextProperty("settings", &LQuickSettings::notifier());
    engine.rootContext()->setContextProperty("myEnumValue", MyEnum::Value2);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
}
