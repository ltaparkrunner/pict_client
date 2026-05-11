#include "auxilary.h"

QString cleanNetworkFilePath(QString networkFilePath){
    QUrl url(networkFilePath);
    url.setQuery(QUrlQuery());
    return url.toString();
}
QString cleanLocalFilePath(QString localFilePath){
    if (localFilePath.startsWith("file:///")) {
        return QUrl(localFilePath).toLocalFile();
    }
    return localFilePath;
}

QString cleanFilePath(QString filePath, bool isNetwork){
    if(isNetwork){
        QUrl url(filePath);
        url.setQuery(QUrlQuery());
        return url.toString();
    }
    else return QUrl(filePath).toLocalFile();
}
