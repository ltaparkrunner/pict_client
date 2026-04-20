#include "filehelper.h"

FileHelper::FileHelper(QObject *parent) : QObject(parent) {}

Q_INVOKABLE bool FileHelper::fileExists(const QString &path) {
    // Преобразуем QML URL (file:///...) в обычный путь, если нужно
    QString localPath = path;
    if (localPath.startsWith("file:///")) {
        localPath = QUrl(path).toLocalFile();
    }
    return QFile::exists(localPath);
}

Q_INVOKABLE bool FileHelper::exists(const QString &path) {
    if (path.isEmpty()) return false;

    // Превращаем QML URL (file:///...) в путь для файловой системы
    QString localPath = QUrl(path).isLocalFile()
                            ? QUrl(path).toLocalFile()
                            : path;

    return QFile::exists(localPath);
}

Q_INVOKABLE int FileHelper::checkPathType(const QString &path) {
    if (path.isEmpty()) return Unknown;

    QUrl url(path);

    // 1. Проверка локальной файловой системы
    if (url.isLocalFile() || path.startsWith("/") || (path.size() > 1 && path[1] == ':')) {
        QString localPath = url.isLocalFile() ? url.toLocalFile() : path;
        QFileInfo info(localPath);

        qDebug() << "localPath" << localPath << " is " << info.exists()
                 << " is local file " << info.isFile() << " is local folder" << info.isDir();
        if (!info.exists()) return Unknown;
        if (info.isDir()) return LocalFolder;
        if (info.isFile()) return LocalFile;
    }

    // 2. Проверка MinIO (основана на структуре URL)
    // Обычно формат: http://minio-server:9000/bucket-name/object-name
    if (url.scheme() == "http" || url.scheme() == "https") {
        QString pathStr = url.path();
        if (pathStr.startsWith("/")) pathStr = pathStr.mid(1); // убираем первый слеш

        QStringList parts = pathStr.split('/', Qt::SkipEmptyParts);

        if (parts.count() == 1) return MinioBucket; // Только имя бакета
        if (parts.count() > 1) return MinioFile;    // Бакет + путь к объекту
    }

    return Unknown;
}
