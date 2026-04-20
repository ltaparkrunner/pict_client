#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QObject>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QDir>

class FileHelper : public QObject {
    Q_OBJECT
public:
    enum PathType {
        Unknown,
        LocalFile,
        LocalFolder,
        MinioFile,
        MinioBucket
    };
    Q_ENUM(PathType) // Регистрируем enum в системе метаданных Qt

    explicit FileHelper(QObject *parent = nullptr);
    Q_INVOKABLE bool exists(const QString &path);
    Q_INVOKABLE bool fileExists(const QString &path);
    Q_INVOKABLE int checkPathType(const QString &path);
    Q_INVOKABLE bool writeToFile(const QString &fileUrl, const QString &content);
    Q_INVOKABLE bool saveFilesToFolder(const QString &folderUrl, const QStringList &fileUrls);
};

#endif // FILEHELPER_H
