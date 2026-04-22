#ifndef LISTSTRINGMODEL_H
#define LISTSTRINGMODEL_H

// ImageModel.h
#include <QAbstractListModel>
#include <QStringList>
#include <QUrl>

class ImageModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum ImageRoles {
        PathRole = Qt::UserRole + 1
    };

    explicit ImageModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    // 1. Return number of items
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_imagePaths.count();
    }

    // 2. Provide data for a specific row and "role"
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() < 0 || index.row() >= m_imagePaths.count())
            return QVariant();

        if (role == PathRole)
            return m_imagePaths.at(index.row());

        return QVariant();
    }

    // 3. Map integer roles to string names used in QML
protected:
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[PathRole] = "imagePath";
        return roles;
    }

public:
    // Helper to add data and notify the view
    Q_INVOKABLE void addImagePath(const QString &path) {
        QUrl url = QUrl::fromLocalFile(path);
        QString pathForQml = url.toString();
        beginInsertRows(QModelIndex(), m_imagePaths.count(), m_imagePaths.count());
        m_imagePaths.append(pathForQml);
        endInsertRows(); // This triggers the QML view update
    }

    Q_INVOKABLE QString resolvePath(const QString &path) {
        QUrl url = QUrl::fromLocalFile(path);
        return url.toString();
    }
private:
    QStringList m_imagePaths;
};

#endif // LISTSTRINGMODEL_H
