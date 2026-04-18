#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "listStringModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);


    ImageModel model;
    for(int i=0; i<5; i++){
        model.addImagePath("file:///C:/Windows/WinSxS/amd64_microsoft-windows-shell-wallpaper-themea_31bf3856ad364e35_10.0.22621.1_none_386b894098b0f0c7/img23.jpg");
        model.addImagePath("file:///C:/Windows/WinSxS/amd64_microsoft-windows-shell-wallpaper-themea_31bf3856ad364e35_10.0.22621.1_none_386b894098b0f0c7/img20.jpg");
        model.addImagePath("file:///C:/Windows/WinSxS/amd64_microsoft-windows-shell-wallpaper-themea_31bf3856ad364e35_10.0.22621.1_none_386b894098b0f0c7/img21.jpg");
        model.addImagePath("file:///C:/Windows/WinSxS/amd64_microsoft-windows-shell-wallpaper-themea_31bf3856ad364e35_10.0.22621.1_none_386b894098b0f0c7/img22.jpg");
    }




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
