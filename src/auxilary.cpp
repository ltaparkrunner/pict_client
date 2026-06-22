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

bool copyFileWithUniqueName(const QString &sourceFilePath, const QString &targetDirPath) {
    QFileInfo sourceInfo(sourceFilePath);
    if (!sourceInfo.exists()) {
        qDebug() << "Source file does not exist:" << sourceFilePath;
        return false;
    }

    // Extract base name and extension separately
    QString baseName = sourceInfo.baseName();    // e.g., "data" from "data.tar.gz"
    QString completeSuffix = sourceInfo.completeSuffix(); // e.g., "tar.gz"
    if (!completeSuffix.isEmpty()) {
        completeSuffix = "." + completeSuffix;
    }

    QDir targetDir(targetDirPath);
    QString destinationPath = targetDir.filePath(sourceInfo.fileName());

    int counter = 1;
    // Loop until a unique file name is found
    while (QFile::exists(destinationPath)) {
        QString newFileName = QString("%1(%2)%3").arg(baseName).arg(counter).arg(completeSuffix);
        destinationPath = targetDir.filePath(newFileName);
        counter++;
    }

    // Copy the file to the unique destination path
    if (QFile::copy(sourceFilePath, destinationPath)) {
        qDebug() << "File successfully saved to:" << destinationPath;
        return true;
    } else {
        qDebug() << "Failed to copy file to:" << destinationPath;
        return false;
    }
}
