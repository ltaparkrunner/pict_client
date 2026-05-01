#include "filehelper.h"

FileHelper::FileHelper(WebSocketClient *wsc, QObject *parent) : QObject(parent), wsclient(wsc) {}

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

Q_INVOKABLE int FileHelper::processWritePathsLocal(const QStringList &ls, const QString &path){
    qDebug() << "int processWritePathsLocal(const QStringList &ls, const QString &path)";
    return 0;
}

Q_INVOKABLE int FileHelper::processWritePathsMinio(const QStringList &ls, const QString &path){
//    qDebug() << "int processWritePathsMinio(const QStringList &ls, const QString &path)";
    for(const QString &file : ls){
            wsclient->addFileRequest(file, path);
    }
    return 0;
}

Q_INVOKABLE int FileHelper::processDeleteFolderLocal(const QString &folderPath){
    qDebug() << "int processDeleteFolderLocal(const QString &path)";
    QDir directory(folderPath);

    if (!directory.exists()) {
        qDebug() << "Папка не найдена:" << folderPath;
        return -1;
    }

    // Устанавливаем фильтры по расширениям
    QStringList filters;
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp" << "*.webp";

    // Получаем список имен файлов, соответствующих фильтрам (только файлы, без папок)
    QStringList imageFiles = directory.entryList(filters, QDir::Files);

    int deletedCount = 0;

    for (const QString &fileName : std::as_const(imageFiles)) {
        // Формируем полный путь к файлу
        QString filePath = directory.absoluteFilePath(fileName);

        if (QFile::remove(filePath)) {
            deletedCount++;
        } else {
            qDebug() << "Не удалось удалить:" << fileName;
//            return -2;
        }
    }
    qDebug() << "Удаление завершено. Удалено файлов:" << deletedCount;
    return deletedCount;
}

Q_INVOKABLE int FileHelper::processDeleteFolderMinio(const QString &path){
    qDebug() << "int processDeleteFolderMinio(const QString &path)";
    return 0;
}

Q_INVOKABLE int FileHelper::processDeleteFileLocal(const QString &filePath){
    qDebug() << "int processDeletePathLocal(const QString &path)";
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower(); // Получаем расширение в нижнем регистре

    // Список разрешенных расширений
    QStringList validExtensions = { "jpg", "jpeg", "png", "gif", "bmp", "webp" };

    if (!validExtensions.contains(ext)) {
        qDebug() << "Ошибка: Файл не является поддерживаемым изображением (." << ext << ")";
        return -3;
    }

    // Проверяем, существует ли файл перед удалением
    if (QFile::exists(filePath)) {
        if (QFile::remove(filePath)) {
            qDebug() << "Файл успешно удален:" << filePath;
        } else {
            qDebug() << "Не удалось удалить файл. Возможно, он занят другим процессом.";
            return -2;
        }
    } else {
        qDebug() << "Файл не найден по пути:" << filePath;
        return -1;
    }
    return 0;
}

Q_INVOKABLE int FileHelper::processDeleteFileMinio(const QString &path){
    qDebug() << "int processDeletePathMinio(const QString &path)";
    wsclient->deleteFileFromBucketRequest(path);
    return 0;
}

Q_INVOKABLE int FileHelper::deleteMinioBuckets(const QStringList &paths) {
    wsclient->deleteMinioBucketsRequest(paths);
    return 0;
}
