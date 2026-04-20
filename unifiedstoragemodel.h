#ifndef UNIFIEDSTORAGEMODEL_H
#define UNIFIEDSTORAGEMODEL_H

#include <QAbstractListModel>
#include <QDir>
#include <QObject>
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

    explicit UnifiedStorageModel(QObject *parent = nullptr);
    // Основные методы модели
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Логика навигации
    Q_INVOKABLE void loadRoot();          // Показать выбор: "Local" и "MinIO"
    Q_INVOKABLE void enterLocal(QString path); // Зайти в локальную папку
    Q_INVOKABLE void enterMinio(QString path);        // Зайти в список бакетов MinIO

private:
    QVector<StorageItem> m_items;
};

#endif // UNIFIEDSTORAGEMODEL_H
