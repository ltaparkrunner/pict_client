#include "filehelper.h"

FileHelper::FileHelper(QObject *parent) : QObject(parent) {}

Q_INVOKABLE bool FileHelper::fileExists(const QString &path) {
    // Преобразуем QML URL (file:///...) в обычный путь, если нужно
    QString localPath = path;
    if (localPath.startsWith("file:///")) {
        localPath = QUrl(path).toLocalFile();
    }
    qDebug() << "FileHelper::fileExists";
    return QFile::exists(localPath);
}

Q_INVOKABLE bool FileHelper::exists(const QString &path) {
    qDebug() << "FileHelper::exists";
    if (path.isEmpty()) {
        qDebug() << "FileHelper::exists, path.isEmpty() ";
        return false;
    }
    qDebug() << "FileHelper::exists, path.is not Empty() ";
    // Превращаем QML URL (file:///...) в путь для файловой системы
    QString localPath = QUrl(path).isLocalFile()
                            ? QUrl(path).toLocalFile()
                            : path;
    qDebug() << "FileHelper::exists, path.is not Empty() 2 ";
    return QFile::exists(localPath);
}

Q_INVOKABLE int FileHelper::checkPathType(const QString &path) {
//    qDebug() << "FileHelper::checkPathType" << path;
    if (path.isEmpty()) return Unknown;

    QUrl url(path);

    // 1. Проверка локальной файловой системы
    if (url.isLocalFile() || path.startsWith("/") || (path.size() > 1 && path[1] == ':')) {
        QString localPath = url.isLocalFile() ? url.toLocalFile() : path;
        QFileInfo info(localPath);

        qDebug() << "localPath" << localPath << " is " << info.exists()
                 << " is local file " << info.isFile() << " is local folder" << info.isDir();
        if (!info.exists()) return Unknown;
//        qDebug() << "if (!info.exists()) return Unknown;";
        if (info.isDir()) return LocalFolder;
//        qDebug() << "if (if (info.isDir()) return LocalFolder;";
        if (info.isFile()) return LocalFile;
    }

    // 2. Проверка MinIO (основана на структуре URL)
    // Обычно формат: http://minio-server:9000/bucket-name/object-name
    if (url.scheme() == "http" || url.scheme() == "https") {
        QString pathStr = url.path();
        if (pathStr.startsWith("/")) pathStr = pathStr.mid(1); // убираем первый слеш
        QStringList parts = pathStr.split('/', Qt::SkipEmptyParts);
        if (parts.count() == 1) return MinioBucket; // Только имя бакета
        qDebug() << "after minio check 4";
        if (parts.count() > 1) return MinioFile;    // Бакет + путь к объекту
        qDebug() << "after minio check 5";
    }
    return Unknown;
}

Q_INVOKABLE bool FileHelper::writeToFile(const QString &fileUrl, const QString &content) {
    // Преобразуем QML URL в локальный путь
    QString localPath = QUrl(fileUrl).toLocalFile();

    QFile file(localPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << content;
        file.close();
        return true;
    }
    return false;
}

Q_INVOKABLE bool FileHelper::saveFilesToFolder(const QString &folderUrl, const QStringList &fileUrls) {
    // Превращаем URL папки в обычный путь
    QString destDir = QUrl(folderUrl).toLocalFile();
    QDir dir(destDir);

    if (!dir.exists()) return false;

    bool allOk = true;
    for (const QString &fileUrl : fileUrls) {
        QString srcPath = QUrl(fileUrl).toLocalFile();
        QFileInfo fileInfo(srcPath);

        // Формируем новый путь: папка_назначения / имя_файла
        QString destPath = dir.absoluteFilePath(fileInfo.fileName());

        // Если файл уже существует, удаляем его (или можно пропустить)
        if (QFile::exists(destPath)) {
            QFile::remove(destPath);
        }

        if (!QFile::copy(srcPath, destPath)) {
            allOk = false;
        }
    }
    return allOk;
}
