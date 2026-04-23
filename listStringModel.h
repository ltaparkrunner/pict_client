#ifndef LISTSTRINGMODEL_H
#define LISTSTRINGMODEL_H

// ImageModel.h
#include <QAbstractListModel>
#include <QStringList>
#include <QUrl>
#include <QDir>

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
        beginInsertRows(QModelIndex(), m_imagePaths.count(), m_imagePaths.count());
        m_imagePaths.append(pathForQml);
        endInsertRows(); // This triggers the QML view update
        return pathForQml;
    }

private:
    QStringList m_imagePaths;
};

#endif // LISTSTRINGMODEL_H
