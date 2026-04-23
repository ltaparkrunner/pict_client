#include "unifiedstoragemodel.h"

UnifiedStorageModel::UnifiedStorageModel(QObject *parent)
    : QAbstractListModel{parent}
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
    // Здесь вы вызываете API MinIO. Когда придет ответ:
    beginResetModel();
    m_items.clear();
    // Имитация добавления бакетов

    m_items.append({"My-Bucket-1", "s3://bucket1", true, true});
    m_items.append({"Logs-Bucket", "s3://logs", true, true});
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

/*
void UnifiedStorageModel::enterLocal(QString path) {
    // 1. Преобразуем URL в обычный путь, если нужно
    QString cleanPath = QUrl(path).isLocalFile() ? QUrl(path).toLocalFile() : path;

    beginResetModel();
    m_items.clear();

    QDir dir(cleanPath);
    // Получаем список файлов и папок
    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst);

    for (const QFileInfo &info : list) {
        m_items.append({
            info.fileName(),
            info.absoluteFilePath(),
            info.isDir(),
            false // isMinio = false
        });
    }
    endResetModel();
}

void UnifiedStorageModel::enterMinio() {
    // Вариант А: Если вы просто запрашиваете список БАКЕТОВ
    // (Логика запроса к вашему Node.js серверу через WebSocket)

    beginResetModel();
    m_items.clear();

    // Здесь должен быть запрос. Пока добавим "заглушки" для теста:
    m_items.append({"images-bucket", "bucket_id_1", true, true});
    m_items.append({"reports-2024", "bucket_id_2", true, true});

    endResetModel();
}
*/
