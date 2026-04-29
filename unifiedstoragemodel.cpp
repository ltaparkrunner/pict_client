#include "unifiedstoragemodel.h"

UnifiedStorageModel::UnifiedStorageModel(WebSocketClient *wsc, QObject *parent)
    : QAbstractListModel{parent}
    , wsclient (wsc)
{}

void UnifiedStorageModel::enterLocal(QString path) {
    beginResetModel();
    m_items.clear();
    QDir dir(path);

    QStringList filters;
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp" << "*.webp";

    QFileInfoList files = dir.entryInfoList(filters, QDir::AllEntries | QDir::NoDot, QDir::DirsFirst);
//    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDot, QDir::DirsFirst);
    QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDot, QDir::DirsFirst);
    QFileInfoList fullList = dirs + files;
//    for (auto &info : dir.entryInfoList(QDir::AllEntries | QDir::NoDot)) {
    for (const QFileInfo &info : std::as_const(fullList)) {
        m_items.append({info.fileName(), info.absoluteFilePath(), info.isDir(), false});
    }
    endResetModel();
}

void UnifiedStorageModel::enterMinio(QString path) {
//    qDebug() << "UnifiedStorageModel::enterMinio and request";
    connect(wsclient, &WebSocketClient::bucketsReceived, this, &UnifiedStorageModel::minioBucketsToQML);
    wsclient->getBucketsListRequest();
}

void UnifiedStorageModel::enterMinioBucket(const QString &path) {
    connect(wsclient, &WebSocketClient::pathsReceived, this, &UnifiedStorageModel::minioPathsToQML);
    wsclient->getFilesFoldersListfromBucketRequest(path, usmodel);
}

// void UnifiedStorageModel::enterMinio2(const QString &path) {
//     connect(wsclient, &WebSocketClient::pathsReceived, this, &UnifiedStorageModel::minioPathsToQML);
//     wsclient->getImagesListfromBucketRequest("images");
// }

void UnifiedStorageModel::minioBucketsToQML(const QStringList &buckets) {
    beginResetModel();
    m_items.clear();
    // for (const QString &bucket : buckets) {
    //     m_items.append({bucket, bucket, true, true});
    // }
    for (int i = 0; i < buckets.size(); i += 2) {
        m_items.append({buckets[i], buckets[i+1], true, true});
    }
    endResetModel();
}

void UnifiedStorageModel::minioPathsToQML(const QList<QStringList> &paths) {
    beginResetModel();
    m_items.clear();
    for (const QStringList& image : paths) {
        m_items.append({image[0], image[1], false, true});
    }
    endResetModel();
}

// 1. Return number of items
int UnifiedStorageModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
//        qDebug() << "UnifiedStorageModel::rowCount parent.isValid() return 0";
        return 0;
    }
//    qDebug() << "UnifiedStorageModel::rowCount m_items.size(): " << m_items.size();
    return m_items.size();
}

// 2. Provide data for a specific row and "role"
QVariant UnifiedStorageModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())//m_imagePaths.count())
        return QVariant();

    const StorageItem &item = m_items.at(index.row());

    switch (role) {
    case NameRole:
        return item.name;
    case PathRole:
        return item.path;
    case IsDirRole:
        return item.isDirectory;
    case IsMinioRole:
        return item.isMinio;
    case PathType:
        return item.pt;
    default:
        return QVariant();
    }
}

// 3. Map integer roles to string names used in QML
QHash<int, QByteArray> UnifiedStorageModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[PathRole] = "path";
    roles[IsDirRole] = "isDir";
    roles[IsMinioRole] = "isMinio";
    return roles;
}

void UnifiedStorageModel::loadRoot() {
    beginResetModel();
    m_items.clear();

    // Добавляем две виртуальные "папки"
    m_items.append({"Локальные файлы", "/", true, false});
    m_items.append({"Облако MinIO", "minio_root", true, true});

    endResetModel();
}

Q_INVOKABLE QVariantMap UnifiedStorageModel::get(int row) const {
    // Проверка границ, чтобы избежать падения
    if (row < 0 || row >= m_items.count()) {
        return QVariantMap();
    }

    const StorageItem &item = m_items.at(row);
    QVariantMap res;

//    Вручную наполняем карту данными
    res["name"] = item.name;
    res["path"] = item.path;
    res["isDir"] = item.isDirectory;
    res["isMinio"] = item.isMinio;
//    res["PathType"] = item.pt;
    return res;
}
