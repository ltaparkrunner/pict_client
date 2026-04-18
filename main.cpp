#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "listStringModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);


    ImageModel model;
    model.addImagePath("file:///path/to/img1.jpg");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("imageModel", &model);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("pict_client", "Main");



//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
