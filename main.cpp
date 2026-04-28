#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "listStringModel.h"
#include "filehelper.h"
#include "websocketclient.h"
#include "unifiedstoragemodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    WebSocketClient wsClient(QUrl("wss://localhost:8080"));

    FileHelper fileHlp(&wsClient);
    ImageModel imodel(&wsClient);
    // for(int i=0; i<5; i++){
    //     model.addImagePath("file:///C:/Windows/WinSxS/amd64_microsoft-windows-shell-wallpaper-themea_31bf3856ad364e35_10.0.22621.1_none_386b894098b0f0c7/img23.jpg");
    //     model.addImagePath("file:///C:/Windows/WinSxS/amd64_microsoft-windows-shell-wallpaper-themea_31bf3856ad364e35_10.0.22621.1_none_386b894098b0f0c7/img20.jpg");
    //     model.addImagePath("file:///C:/Windows/WinSxS/amd64_microsoft-windows-shell-wallpaper-themea_31bf3856ad364e35_10.0.22621.1_none_386b894098b0f0c7/img21.jpg");
    //     model.addImagePath("file:///C:/Windows/WinSxS/amd64_microsoft-windows-shell-wallpaper-themea_31bf3856ad364e35_10.0.22621.1_none_386b894098b0f0c7/img22.jpg");
    // }
    UnifiedStorageModel usModel(&wsClient);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("wsClient", &wsClient);
    engine.rootContext()->setContextProperty("imageModel", &imodel);
    engine.rootContext()->setContextProperty("FileHelper", &fileHlp);
    engine.rootContext()->setContextProperty("storageModel", &usModel);
    qmlRegisterUncreatableType<FileHelper>("com.myapp.helpers", 1, 0, "FileHelperType", "Error: FileHelperType is enum only");
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("pict_client", "Main");

    return app.exec();
}
