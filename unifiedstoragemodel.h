#ifndef UNifIEDSTORAGEMODEL_H
#define UNifIEDSTORAGEMODEL_H

#include <QAbstractListModel>
#include <QDir>
#include <QObject>
#include "websocketclient.h"

enum PathType {
    Unknown,
    LocalFile,
    LocalFolder,
    MinioFile,
    MinioBucket
};

struct StorageItem {
    QString name;
    QString path;
    bool isDirectory;
    bool isMinio; // Флаг для отличия локального от облачного
    PathType pt;
};

class UnifiedStorageModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles { NameRole = Qt::UserRole + 2, PathRole, IsDirRole, IsMinioRole, PathType };

    explicit UnifiedStorageModel(WebSocketClient *wsc, QObject *parent = nullptr);
    // Основные методы модели
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Логика навигации
    Q_INVOKABLE void loadRoot();          // Показать выбор: "Local" и "MinIO"
    Q_INVOKABLE void enterLocal(QString path); // Зайти в локальную папку
    Q_INVOKABLE void enterMinio(QString path);        // Зайти в список бакетов MinIO
//    Q_INVOKABLE void enterMinio2(const QString &path);        // Зайти в список бакетов MinIO
    Q_INVOKABLE void enterMinioBucket(const QString &path);
    Q_INVOKABLE void minioPathsToQML(const QList<QStringList> &paths);
    Q_INVOKABLE void minioBucketsToQML(const QStringList &paths);
    Q_INVOKABLE QVariantMap get(int row) const;
//    QString fileName = QUrl(urlString).fileName();

private:
    QVector<StorageItem> m_items;
    WebSocketClient *wsclient;
};

#endif // UNifIEDSTORAGEMODEL_H
