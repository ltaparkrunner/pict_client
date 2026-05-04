#ifndef LISTSTRINGMODEL_H
#define LISTSTRINGMODEL_H

// ImageModel.h
#include <QAbstractListModel>
#include <QStringList>
#include <QUrl>
#include <QDir>

#include "websocketclient.h"

struct ImageItem {
    QString name;
    QString path;
    bool isNetwork;               // Флаг для отличия локального от облачного
    bool isDir;
    QString mongoId;
};

class ImageModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum ImageRoles {
        ImageNameRole = Qt::UserRole + 16, ImagePathRole, ImageIsNetworkRole, ImageIsDirRole, ImageMongoIdRole
    };

    explicit ImageModel(WebSocketClient *wsc, QObject *parent = nullptr) : QAbstractListModel(parent), wsclient (wsc) {}

    // 1. Return number of items
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_imageItems.count();
    }

    // 2. Provide data for a specific row and "role"
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() < 0 || index.row() >= m_imageItems.count())
            return QVariant();

        // if (role == ImagePathRole)
        //     return m_imageItems.at(index.row());

        // return QVariant();
        const ImageItem &item = m_imageItems.at(index.row());
        switch (role) {
        case ImageNameRole:
            return item.name;
        case ImagePathRole:
            return item.path;
        case ImageIsNetworkRole:
            return item.isNetwork;
        case ImageIsDirRole:
            return item.isDir;
        case ImageMongoIdRole:
            return item.mongoId;
        default:
            return QVariant();
        }
    }

    // 3. Map integer roles to string names used in QML
protected:
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[ImageNameRole] = "name";
        roles[ImagePathRole] = "path";
        roles[ImageIsNetworkRole] = "isNetwork";
        roles[ImageIsDirRole] = "isDir";
        roles[ImageMongoIdRole] = "mongoId";
        return roles;
    }

public:
    // Helper to add data and notify the view
    Q_INVOKABLE void addImagePath(const QString &path) {
        QUrl url = QUrl::fromLocalFile(path);
        QString pathForQml = url.toString();
        QFileInfo fileInfo(pathForQml);
        QString fileName = fileInfo.fileName();
        beginInsertRows(QModelIndex(), m_imageItems.count(), m_imageItems.count());
        m_imageItems.append({fileName, pathForQml, false, false, ""});
        endInsertRows(); // This triggers the QML view update
    }

    // Q_INVOKABLE void addImageMinioPath(const QString &path) {
    //     QUrl url = QUrl::fromUserInput(path);
    //     QString pathForQml = url.toString();
    //     beginInsertRows(QModelIndex(), m_imageItems.count(),m_imageItems.count());
    //     m_imageItems.append(pathForQml);
    //     endInsertRows(); // This triggers the QML view update
    // }

    Q_INVOKABLE QString resolvePath(const QString &path) {
        QUrl url = QUrl::fromLocalFile(path);
        return url.toString();
    }

    Q_INVOKABLE QString addImagesFromFolder(const QString &folderPath) {
        QDir dir(folderPath);

        // 1. Устанавливаем фильтры: только файлы, игнорируем "." и ".."
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

        // 2. Устанавливаем маски для изображений
        QStringList filters;
        filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.gif";
        dir.setNameFilters(filters);

        // 3. Получаем список имен файлов (без вложенных папок)
        QStringList fileList = dir.entryList();

        // 4. Добавляем каждый файл в модель
        for (const QString &fileName : std::as_const(fileList)) {
            // Формируем полный путь
            QString fullPath = dir.absoluteFilePath(fileName);

            // Вызываем вашу функцию (убедитесь, что она внутри вызывает beginInsertRows)
            addImagePath(fullPath);
        }
        if(!fileList.isEmpty()) {
            QUrl url = QUrl::fromLocalFile(dir.absoluteFilePath(fileList.first()));
            return url.toString();
        }
        return {};
    }

    Q_INVOKABLE QString addMinioImagePath(const QString &path) {
        QUrl url = QUrl::fromUserInput(path);
        QString pathForQml = url.toString();
        beginInsertRows(QModelIndex(), m_imageItems.count(), m_imageItems.count());
        m_imageItems.append({"img1", pathForQml, true, false, ""});  // TODO:
        endInsertRows(); // This triggers the QML view update
        return pathForQml;
    }

    Q_INVOKABLE QStringList addImagesFromMinioBucket(const QString &path) {
        QString fileName = QUrl(path).fileName();
        connect(wsclient, &WebSocketClient::filesReceived, this, &ImageModel::minioPathsToQML);
        wsclient->getFilesOnlyListfromBucketRequest(fileName/*, imodel*/);
        return {};
    }

    Q_INVOKABLE QString  minioPathsToQML(const QList<QStringList> &files) {
        for (const QStringList &image : std::as_const(files)) {
//            addImageMinioPath(image[1]);
            QUrl url = QUrl::fromUserInput(image.at(1));
            QString pathForQml = url.toString();
            beginInsertRows(QModelIndex(), m_imageItems.count(),m_imageItems.count());
            m_imageItems.append({image.at(0), pathForQml, true, false, image.at(3)});
            endInsertRows(); // This triggers the QML view update
        }
        if(!files.isEmpty()) {
            QUrl url = QUrl::fromUserInput(files.first()[1]);
            minioImageToQML(url.toString());
            return url.toString();
        }
        return {};
    }

    Q_INVOKABLE QVariantMap get(int row) const {
        // Проверка границ, чтобы избежать падения
        if (row < 0 || row >= m_imageItems.count()) {
            return QVariantMap();
        }

//        const QString &item = m_imageItems.at(row);
        const ImageItem &item = m_imageItems.at(row);
        QVariantMap res;

        res["name"] = item.name;
        res["path"] = item.path;
        res["isNetwork"] = item.isNetwork;
        res["isDir"] = item.isDir;
        res["mongoId"] = item.mongoId;
        return res;
    }

    void getImageFromUdsm(QString name, QString path, bool isNet, bool isDir, QString mongoID) {
        qDebug() << "getImageFromUdsm";
        if(isNet && !isDir){
            QUrl url = QUrl::fromUserInput(path);
            QString pathForQml = url.toString();
            beginInsertRows(QModelIndex(), m_imageItems.count(), m_imageItems.count());
            m_imageItems.append({name, pathForQml, isNet, isDir, mongoID});
            qDebug() << "m_imageItems.append isNet name: " << name << "  path: " << path;
            endInsertRows(); // This triggers the QML view update
            return;
        }
        if(!isNet && !isDir){
            QUrl url = QUrl::fromUserInput(path);
            QString pathForQml = url.toString();
            beginInsertRows(QModelIndex(), m_imageItems.count(), m_imageItems.count());
            m_imageItems.append({name, pathForQml, isNet, isDir, mongoID});
            qDebug() << "m_imageItems.append name: " << name << "  path: " << path;
            endInsertRows(); // This triggers the QML view update
            return;
        }

    }
    Q_INVOKABLE int insertImage(const QVariantMap &map) {
        qDebug() << "insertImage(const QVariantMap &map)";
        bool isDir = map["isDir"].toBool();
        bool isNetwork = map["isNetwork"].toBool();
        if(isNetwork && !isDir){
            QUrl url = QUrl::fromUserInput(map["path"].toString());
            QString pathForQml = url.toString();
            beginInsertRows(QModelIndex(), m_imageItems.count(), m_imageItems.count());
            m_imageItems.append({map["name"].toString(), pathForQml, isNetwork, isDir, map["mongoId"].toString()});
            qDebug() << "m_imageItems.append isNet name: " << map["name"].toString() << "  pathForQml: " << pathForQml;
            endInsertRows(); // This triggers the QML view update
            return 0;
        }
        if(!isNetwork && !isDir){
            QUrl url = QUrl::fromUserInput(map["path"].toString());
            QString pathForQml = url.toString();
            beginInsertRows(QModelIndex(), m_imageItems.count(), m_imageItems.count());
            m_imageItems.append({map["name"].toString(), pathForQml, isNetwork, isDir, map["mongoId"].toString()});
            qDebug() << "m_imageItems.append name: " << map["name"].toString() << "  pathForQml: " << pathForQml;
            endInsertRows(); // This triggers the QML view update
            return 0;
        }
        return 0;
    }
signals:
    void minioImageToQML(const QString &path);

private:
//    QStringList m_imagePaths;
    QVector<ImageItem> m_imageItems;
    WebSocketClient *wsclient;
};

#endif // LISTSTRINGMODEL_H
