#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "listStringModel.h"
#include "filehelper.h"
#include "websocketclient.h"
#include "unifiedstoragemodel.h"
#include "authhandler.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    WebSocketClient wsClient(QUrl("wss://localhost:8081"));
//    WebSocketClient wsClient(QUrl("ws://0.0.0.0:8081"));


    FileHelper fileHlp(&wsClient);
    ImageModel imodel(&wsClient);
    UnifiedStorageModel usModel(&wsClient);

    QQmlApplicationEngine engine;

    //  QObject::connect(&usModel, &UnifiedStorageModel::udsmToIm, &imodel, &ImageModel::getImageFromUdsm);
    //  AuthHandler authHandler;
    // Expose the instance to QML context
    //  engine.rootContext()->setContextProperty("authHandler", &authHandler);
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

    QObject *rootObject = engine.rootObjects().first();
    QObject *authDialog = rootObject->findChild<QObject*>("myAuthDialog");
    AuthHandler authHandler(&wsClient);

    if (authDialog) {
        // 3. Connect loginRequested signal to C++ slot
        QObject::connect(authDialog, SIGNAL(loginRequested(QString,QString)),
                         &authHandler, SLOT(login(QString,QString)));

        // 4. Connect registerRequested signal to C++ slot
        QObject::connect(authDialog, SIGNAL(registerRequested(QString,QString)),
                         &authHandler, SLOT(registerUser(QString,QString)));
    }
    return app.exec();
}
