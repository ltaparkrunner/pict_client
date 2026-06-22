#include "unifiedstoragemodel.h"
#include "filedownloader.h"
#include "auxilary.h"

UnifiedStorageModel::UnifiedStorageModel(WebSocketClient *wsc, MsgHandler *svrHndlr, QObject *parent)
    : QAbstractListModel{parent}
    , wsclient (wsc)
    , msghandler (svrHndlr)
{
    connect(msghandler, &MsgHandler::pathsReceived, this, &UnifiedStorageModel::minioPathsToQML);
    connect(wsclient, &WebSocketClient::errReceived, this, [=](){
        qDebug() << " errReceived";
        beginResetModel();
        m_items.clear();
        endResetModel();
    });
    connect(msghandler, &MsgHandler::writeUrlsToLocal, this, &UnifiedStorageModel::writeUrlsToLocal);
    connect(msghandler, &MsgHandler::pathInfoResp, this, &UnifiedStorageModel::openNetStoreDialog);
    connect(msghandler, &MsgHandler::bucketsReceived, this, &UnifiedStorageModel::minioBucketsToQML);
}

void UnifiedStorageModel::enterLocal(const QString &path) {
    qDebug() << "UnifiedStorageModel::enterLocal: " << path << "m_parentItem" << m_parentItem.path;

    beginResetModel();
    m_items.clear();
    QDir dir{path};
    if(!dir.exists()){
        QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        dir = QDir{defaultPath};
        m_parentItem = {dir.dirName(), defaultPath, defaultPath, true, false, false, false, ""};
    }
    else m_parentItem = {dir.dirName(), dir.absolutePath(), dir.absolutePath(), true, false, false, false, ""};
    QStringList filters;
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp" << "*.webp";

    QFileInfoList files = dir.entryInfoList(filters, QDir::AllEntries | QDir::NoDot, QDir::DirsFirst);
//    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDot, QDir::DirsFirst);
    QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDot, QDir::DirsFirst);
    QFileInfoList fullList = dirs + files;
//    for (auto &info : dir.entryInfoList(QDir::AllEries | QDir::NoDot)) {
    for (const QFileInfo &info : std::as_const(fullList)) {
        m_items.append({info.fileName(), info.absoluteFilePath(), info.absoluteFilePath(), info.isDir(), false, false, false});
    }
    endResetModel();
}

void UnifiedStorageModel::enterNetStore(QString path) {
        msghandler->getBucketsListRequest();
}

void UnifiedStorageModel::enterMinioBucket(const QString &path) {
    qDebug() << "UnifiedStorageModel::enterMinioBucket and request   m_parentItem.path: " << m_parentItem.path << "path" << path;
    msghandler->getFilesFoldersListfromBucketRequest(m_parentItem.path, "" /*, usmodel*/);
}

void UnifiedStorageModel::minioBucketsToQML(const QStringList &buckets) {
    qDebug() << "UnifiedStorageModel::minioBucketsToQML: " << buckets[0]<< " " << buckets[1];
    beginResetModel();
    m_items.clear();

    for (int i = 0; i < buckets.size(); i += 2) {
        m_items.append({buckets[i], buckets[i+1], buckets[i+1], true, true, true, false});
    }
    endResetModel();
}

void UnifiedStorageModel::minioPathsToQML(const QList<QStringList> &paths) {
    beginResetModel();
    m_items.clear();
    qDebug() << "void UnifiedStorageModel::minioPathsToQML(const QList<QStringList> &paths): " << m_parentItem.path;
    int symbs = m_parentItem.path.count('/');
    if(symbs <=2) { m_items.append({"..", "http://minio:9000/", "http://minio:9000/", true, true, true, false});
        qDebug() << "path for ..  " << "http://minio:9000/";
    }
    else if(symbs <=4) { m_items.append({"..", "http://minio:9000/", "http://minio:9000/", true, true, true, false});
        qDebug() << "path for ..  " << "http://minio:9000/";
    }
    else {
        // if (m_parentItem.path.endsWith('/')) {
        //     // Ищем второй слэш с конца, начиная поиск перед последним символом
        //     int prevSlashIdx = m_parentItem.path.lastIndexOf('/', -2);

        //     if (prevSlashIdx != -1) {
        //         m_parentItem.path.truncate(prevSlashIdx + 1); // Оставит '/' в конце
        //     }
        // }
        int prevSlashIdx = m_parentItem.path.lastIndexOf('/', -2);
        if (prevSlashIdx != -1) {
            m_items.append({"..", m_parentItem.path.left(prevSlashIdx + 1), m_parentItem.path.left(prevSlashIdx + 1), true, true, false, false});
            qDebug() << "m_parentItem.path.left(lastSlashIdx + 1);" << m_parentItem.path.left(prevSlashIdx + 1); // Оставит '/' в конце
        }
        else {  m_items.append({"..", "http://minio:9000/",  "http://minio:9000/", true, true, true, false});
            qDebug() << "path for ..  " << "http://minio:9000/";
        }
    }
    for (const QStringList& image : paths) {
        if(image[2] != "folder") m_items.append({image[0], image[1],  cleanNetworkFilePath(image[1]), false, true, false, false, image[3]});
        else m_items.append({image[0], image[1],  image[1], true, true, false, false, image[3]});
    }
    endResetModel();
}

// 1. Return number of items
int UnifiedStorageModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
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
    case CleanPathRole:
        return item.cleanPath;
    case IsDirRole:
        return item.isDirectory;
    case IsMinioRole:
        return item.isMinio;
    case IsMinioBucketRole:
        return item.isMinioBucket;
    case IsVirtualDirRole:
        return item.isVirtualDir;
    case MongoIdRole:
        return item.mongoId;
    default:
        return QVariant();
    }
}

// 3. Map integer roles to string names used in QML
QHash<int, QByteArray> UnifiedStorageModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[PathRole] = "path";
    roles[CleanPathRole] = "cleanPath";
    roles[IsDirRole] = "isDir";
    roles[IsMinioRole] = "isMinio";
    roles[IsMinioBucketRole] = "isMinioBucket";
    roles[IsVirtualDirRole] = "isVirtualDir";
    roles[MongoIdRole] = "mongoId";
    return roles;
}

void UnifiedStorageModel::loadRoot() {
    qDebug() << "UnifiedStorageModel::loadRoot()";
    beginResetModel();
    m_items.clear();

    // Добавляем две виртуальные "папки"
    m_items.append({"Локальные файлы", "/",  "/", true, false, false, false});
    m_items.append({"Облако MinIO", "minio_root", "minio_root", true, true, true, false});

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
    res["cleanPath"] = item.cleanPath;
    res["isDir"] = item.isDirectory;
    res["isMinio"] = item.isMinio;
    res["isMinioBucket"] = item.isMinioBucket;
    res["isVirtualDir"] = item.isVirtualDir;
    res["mongoId"] = item.mongoId;
    return res;
}

Q_INVOKABLE int UnifiedStorageModel::addVirtual(const QString &virtFolderName, const QString &currPath){
    qDebug() << "UnifiedStorageModel::addVirtual currPath: "<< virtFolderName << "m_parentItem: " << m_parentItem.path
             << "  parentName: " << m_parentItem.name << "m_parentItem.isMinioBucket: " << m_parentItem.isMinioBucket;
    beginResetModel();
    if(m_parentItem.isMinioBucket) m_items.append({virtFolderName, currPath+virtFolderName + "/", currPath+virtFolderName + "/",
                        true, true, false, true, ""});
    // Добавляем две виртуальные "папки"
//    else m_items.append({m_parentItem.name+"/"+virtFolderName, currPath+virtFolderName + "/", true, true, false, true, ""});
//    else m_items.append({m_parentItem.name+"/"+virtFolderName, currPath+virtFolderName + "/",  currPath+virtFolderName + "/",
    else m_items.append({virtFolderName, currPath+virtFolderName + "/",  currPath+virtFolderName + "/",

                        true, true, false, true, ""});

    endResetModel();
    qDebug() << "Создаем папку с именем:" << virtFolderName << "in the folder: " << currPath;
    return 0;
}

Q_INVOKABLE int UnifiedStorageModel::openFolderImages(int indx){  // show folder files Images in Main window
    qDebug() << "int UnifiedStorageModel::openFolderImages(int indx): " << m_items[indx].path <<
        "m_items[indx].isMinio" << m_items[indx].isMinio << "m_items[indx].isDir" << m_items[indx].isDirectory;
    if(indx < m_items.size()) m_parentItem = m_items[indx];
    else return -1;
    qDebug() << " m_parentItem: " << m_parentItem.path << "  isMinioBucket: " << m_parentItem.isMinioBucket;
    if(!m_parentItem.isMinio && m_parentItem.isDirectory){ // Local Directory
        beginResetModel();
        m_items.clear();
        QDir dir(m_parentItem.path);

        QStringList filters;
        filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp" << "*.webp";

        QFileInfoList files = dir.entryInfoList(filters, QDir::AllEntries | QDir::NoDot, QDir::DirsFirst);
        //    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDot, QDir::DirsFirst);
        QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDot, QDir::DirsFirst);
        QFileInfoList fullList = dirs + files;
        //    for (auto &info : dir.entryInfoList(QDir::AllEntries | QDir::NoDot)) {
        for (const QFileInfo &info : std::as_const(fullList)) {
            m_items.append({info.fileName(), info.absoluteFilePath(), info.absoluteFilePath(), info.isDir(), false, false, false});
        }
        endResetModel();
        return 0;
    }
    else if(m_parentItem.isMinio && m_parentItem.isMinioBucket) { // Minio Bucket
        qDebug() << " m_parentItem: " << m_parentItem.path;
        msghandler->getFilesFoldersListfromBucketRequest(m_parentItem.path, "" /*, usmodel*/);
        return 0;
    }
    else if(m_parentItem.isMinio && !m_parentItem.isMinioBucket && m_parentItem.isDirectory && !m_parentItem.isVirtualDir) {
        qDebug() << " m_parentItem: " << m_items[indx].path << "UnifiedStorageModel::openFolderImages";
        msghandler->getFilesFoldersListfromBucketRequest(m_parentItem.path, m_parentItem.name  /*, usmodel*/);
        return 0;               // Minio simple folder
    }
    else if(m_parentItem.isMinio && !m_parentItem.isMinioBucket && m_parentItem.isDirectory && m_parentItem.isVirtualDir) {
        qDebug() << "Virtual Minio Folder path: " << m_parentItem.path << "  name: " << m_parentItem.name;
        beginResetModel();
        m_items.clear();
        endResetModel();
        return 0;   // Minio simple folder
    }
    return 0;
}

Q_INVOKABLE int UnifiedStorageModel::enterFolder(int indx){ // Open folder in File/Folder Dialog
    qDebug() << "int UnifiedStorageModel::enterFolder(StorageItem item): " << m_items[indx].path << "  name: " << m_items[indx].name <<
        "m_items[indx].isMinio" << m_items[indx].isMinio << "m_items[indx].isDir" << m_items[indx].isDirectory;
    StorageItem prevprevItem = m_parentItem;
    if(indx < m_items.size()) m_parentItem = m_items[indx];
    else return -1;
    qDebug() << " m_parentItem: " << m_parentItem.path << "  isMinioBucket: " << m_parentItem.isMinioBucket;
    // if(m_items[indx].name == "..") {

    // }

    if(!m_parentItem.isMinio && m_parentItem.isDirectory){ // Local Directory
        beginResetModel();
        m_items.clear();
        QDir dir(m_parentItem.path);

        QStringList filters;
        filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp" << "*.webp";

        QFileInfoList files = dir.entryInfoList(filters, QDir::AllEntries | QDir::NoDot, QDir::DirsFirst);
        //    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDot, QDir::DirsFirst);
        QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDot, QDir::DirsFirst);
        QFileInfoList fullList = dirs + files;
        //    for (auto &info : dir.entryInfoList(QDir::AllEntries | QDir::NoDot)) {
        for (const QFileInfo &info : std::as_const(fullList)) {
            m_items.append({info.fileName(), info.absoluteFilePath(),  info.absoluteFilePath(), info.isDir(), false, false, false});
        }
        endResetModel();
        return 0;
    }
    else if(m_parentItem.isMinio && m_parentItem.isMinioBucket) { // Minio Bucket
        qDebug() << " m_parentItem: " << m_parentItem.path << "first enter";
        if(m_parentItem.path == "http://minio:9000/") msghandler->getBucketsListRequest();
        else msghandler->getFilesFoldersListfromBucketRequest(m_parentItem.path, "" /*, usmodel*/);
        return 0;
    }
    else if(m_parentItem.isMinio && !m_parentItem.isMinioBucket && m_parentItem.isDirectory && !m_parentItem.isVirtualDir) {
        qDebug() << " m_parentItem: " << m_parentItem.path << "UnifiedStorageModel::enterFolder";
        msghandler->getFilesFoldersListfromBucketRequest2(m_parentItem.path, true  /*, usmodel*/);
        return 0;               // Minio simple folder
    }
    else if(m_parentItem.isMinio && !m_parentItem.isMinioBucket && m_parentItem.isDirectory && m_parentItem.isVirtualDir) {
        qDebug() << "Virtual Minio Folder path: " << m_parentItem.path << "  name: " << m_parentItem.name;
        beginResetModel();
        m_items.clear();
        m_items.append({"..", prevprevItem.path,  prevprevItem.path, prevprevItem.isDirectory, prevprevItem.isMinio,
                        prevprevItem.isMinioBucket, prevprevItem.isVirtualDir});
        qDebug() << "Virtual dir, m_parentItem.path: " << prevprevItem.path;
        //m_items.append({"..", prevprevItem.path, prevprevItemtrue, true, false, true});
        endResetModel();
        return 0;   // Minio simple folder
    }
    return 0;
}

/*
Q_INVOKABLE int UnifiedStorageModel::writeToFolder(const QStringList &ls){
    qDebug() << "int UnifiedStorageModel::writeToFolder";
    if(!m_parentItem.isMinio && m_parentItem.isDirectory){

        return 0;
    }
    else if(m_parentItem.isMinio && m_parentItem.isMinioBucket) {
        for(const QString &file : ls){
            wsclient->addFileRequest(file, m_parentItem.path);
        }
        return 0;
    }
    else if(m_parentItem.isMinio && !m_parentItem.isMinioBucket && m_parentItem.isDirectory) {
        qDebug() << "int UnifiedStorageModel::writeToFolder 2";
        for(const QString &file : ls){
            wsclient->addFileRequest(file, m_parentItem.path);
        }
        return 0;
    }
    return 0;
}
*/

Q_INVOKABLE int UnifiedStorageModel::deleteIndices(const QList<int> &indxs){
    qDebug() << "int UnifiedStorageModel::deleteIndices";
    for(int indx : indxs){
        if(m_items[indx].isMinio && !m_items[indx].isDirectory){
            QStringList sl;
            sl.append(m_items[indx].name);

            QUrl netUrl(m_items[indx].path);
            QString path = netUrl.path(); // Вернет "/photos/holiday/sun.jpg"
            qDebug() << "Path: " << path;
            if (path.startsWith('/')) {
                path.remove(0, 1);
            }

            QStringList parts = path.split('/');
            QString bucket = parts.takeFirst();

            sl.append(bucket);
            sl.append(m_items[indx].mongoId);
            sl.append("Ivon");
            qDebug() << " name: " << m_items[indx].name << "  bucket: " << bucket << " mongoId: " << m_items[indx].mongoId;
            connect(msghandler, &MsgHandler::resultSuccess, this, &UnifiedStorageModel::successToQML);
            msghandler->deleteFileFromServerRequest(sl);
        }
        if(!m_items[indx].isMinio && !m_items[indx].isDirectory){
            QFileInfo fileInfo(m_items[indx].path);
            QString dirPath = fileInfo.absolutePath();
            qDebug() << "Directory path: " << dirPath;
            QDir dir{dirPath};
            bool success = dir.remove(m_items[indx].name);
            if(!success) qDebug() << "int UnifiedStorageModel::deleteIndices something went wrong";
        }
    }
    return 0;
}

QStringList UnifiedStorageModel::getBacketNameFromPath(const QString &path){
    qDebug() << "getBacketNameFromPath(const QString &path)" << path;
    return {};
}

QString UnifiedStorageModel::resolveImageIndex(int indx) {
    if(indx < m_items.size()){
        StorageItem item = m_items[indx];
        emit udsmToIm(item.name, item.path, item.isMinio, item.isDirectory, item.mongoId);
        qDebug() << "udsmToIm name: " << item.name << "  path: " << item.path;
        return item.path;
    }
    return "";
}

Q_INVOKABLE QVariantMap UnifiedStorageModel::getData(int indx){
    QVariantMap map;
    StorageItem item = m_items[indx];
    // map["name"] = item.name;
    // map["path"] = item.path;
    // map["cleanPath"] = item.cleanPath;
    // map["mongoId"] = item.mongoId;
    // map["isNetwork"] = item.isMinio;
    // map["isDir"] = item.isDirectory;
    // map["isV"]
    map["name"] = item.name;
    map["path"] = item.path;
    map["cleanPath"] = item.cleanPath;
    map["isDir"] = item.isDirectory;
    map["isMinio"] = item.isMinio;
    map["isMinioBucket"] = item.isMinioBucket;
    map["isVirtualDir"] = item.isVirtualDir;
    map["mongoId"] = item.mongoId;
    return map;
}

// write files to Net or Local
Q_INVOKABLE int UnifiedStorageModel::writeImagesToFolder(const QVariantList &lf, QString path){
    qDebug() << "int UnifiedStorageModel::writeImagesToFolder: " << m_parentItem.path <<
        "  isMinio: " << m_parentItem.isMinio << "  isDir: " << m_parentItem.isDirectory;
    if(!m_parentItem.isMinio && m_parentItem.isDirectory){  //  to local directory
        qDebug() << "!m_parentItem.isMinio && m_parentItem.isDirectory";
        QStringList paths;
        QStringList ids;
        // QStringList paths2;
        for(const QVariant &v : lf){
            QVariantMap item = v.toMap();
            //  qDebug() << item["path"].toString() << "  " << item["mongoId"].toString();
            if(item["isNetwork"].toBool() && !item["isDir"].toBool()){  // from Network to local
                paths.append(item["path"].toString());
                ids.append(item["mongoId"].toString());
            }
            else if(!item["isNetwork"].toBool() && !item["isDir"].toBool()){  // from local to local
                copyFileWithUniqueName(item["path"].toString(), m_parentItem.cleanPath);
            }
        }
        msghandler->getFilesRequest(paths, ids, m_parentItem.path);
        return 0;
    }
    else if(m_parentItem.isMinio && m_parentItem.isMinioBucket) {   //  to Net bucket
        qDebug() << "m_parentItem.isMinio && m_parentItem.isMinioBucket";
        QStringList paths;
        for(const QVariant &v : lf){
            QVariantMap item = v.toMap();
            qDebug() << item["path"].toString() << "  " << item["mongoId"].toString();
            if(!item["isNetwork"].toBool() && !item["isDir"].toBool()) // from local to network
                msghandler->addFileRequest(m_parentItem.path, item["path"].toString(), item["mongoId"].toString());
            else if(item["isNetwork"].toBool() && !item["isDir"].toBool()) {// from network to network
                msghandler->rewriteFileRequest(m_parentItem.path, item["path"].toString(), item["mongoId"].toString());
                //  paths.append(item["path"].toString());
            }
        }
        return 0;
    }
    else if(m_parentItem.isMinio && !m_parentItem.isMinioBucket && m_parentItem.isDirectory) {  //  to Net Directory
        qDebug() << "int UnifiedStorageModel::writeImagesMinioDirectory";
        QStringList paths;
        for(const QVariant &v : lf){
            QVariantMap item = v.toMap();if(!item["isNetwork"].toBool() && !item["isDir"].toBool())
            qDebug() << item["path"].toString() << "  " << item["mongoId"].toString() << " target folder: " << m_parentItem.path;
            if(!item["isNetwork"].toBool() && !item["isDir"].toBool()) // from local to network
                msghandler->addFileRequest(m_parentItem.path, item["path"].toString(), item["mongoId"].toString());
            else if(item["isNetwork"].toBool() && !item["isDir"].toBool()) {// from network to network
                msghandler->rewriteFileRequest(m_parentItem.path, item["path"].toString(), item["mongoId"].toString());
                //  paths.append(item["path"].toString());
            }
        }
        return 0;
    }
    return 0;
}

Q_INVOKABLE void UnifiedStorageModel::successToQML(const QString &msg){
    qDebug() << "Executed successfully";
}

Q_INVOKABLE void UnifiedStorageModel::errorToQML(const QString &msg){
    qCritical() << "Error when executing command";
}

#include <QString>
#include <QDir>
#include <QFileInfo>

bool checkAndRenameFile(const QString &folderPath, const QString &sourceFileName, const QString &targetFileName) {
    QDir dir(folderPath);

    // Проверяем существование папки и исходного файла
    if (!dir.exists() || !dir.exists(sourceFileName)) {
        return false;
    }

    QString finalName = targetFileName;

    // Если целевой файл уже существует, подбираем имя вида name(number).ext
    if (dir.exists(targetFileName)) {
        QFileInfo fileInfo(targetFileName);
        QString baseName = fileInfo.completeBaseName();
        QString extension = fileInfo.suffix();

        // Добавляем точку перед расширением, если оно есть
        if (!extension.isEmpty()) {
            extension = "." + extension;
        }

        int counter = 1;
        // Цикл работает, пока файл с новым именем существует
        while (dir.exists(baseName + "(" + QString::number(counter) + ")" + extension)) {
            counter++;
        }

        finalName = baseName + "(" + QString::number(counter) + ")" + extension;
    }

    // Переименовываем исходный файл в целевое (или уникальное) имя
    return dir.rename(sourceFileName, finalName);
}

int UnifiedStorageModel::writeUrlsToLocal(const QVector<QUrl> &paths) {
    qDebug() << "UnifiedStorageModel::writeUrlsToLocal" << paths[0];
    if(m_parentItem.isDirectory && !m_parentItem.isMinio){
        QFileInfo checkPath{m_parentItem.path};
        if(checkPath.exists() && checkPath.isDir() && checkPath.isWritable()){
            for(const QUrl &urlpath : paths ){
                FileDownloader *downloader = new FileDownloader(); // Создаем экземпляр

                // Находим стандартную папку "Загрузки" на ПК (Windows/Linux)
                // QString localDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

                // Подписываемся на результат
                QObject::connect(downloader, &FileDownloader::downloadFinished, [downloader](const QString &path) {

                    qDebug() << "Готово! Файл сохранен в:" << path;
                    downloader->deleteLater(); // Безопасно удаляем объект из памяти
                });

                QObject::connect(downloader, &FileDownloader::downloadError, [downloader](const QString &err) {
                    qCritical() << "Ошибка скачивания из Minio:" << err;
                    downloader->deleteLater();
                });

                // Запуск скачивания
                downloader->downloadFile(QUrl(urlpath), m_parentItem.path);
            }
        }
        else qCritical() << "Smth wrong with folder." << m_parentItem.path;
    }
    else qCritical() << "Smth wrong with folder." << m_parentItem.path;
    return 0;
}

Q_INVOKABLE bool UnifiedStorageModel::getNetPath(const QString &path){
    // to check if the way is a local or network dir or file and if file(local or network (http://minio:9000/))
    // then open in main window, if dir then open in customFileDialog
    // QString prefix = "http://minio:9000/";
    qDebug() << "UnifiedStorageModel::getNetPath prefix: " << prefix << "  path: " << path;
    if(path.startsWith(prefix)) {
        msghandler -> getNetStore(path);
        return true;
    }  // if exist and file, if exist and dir
    else return false;
}

// Q_INVOKABLE bool UnifiedStorageModel::getNetfolder(const QString &path){
//     // to check if the way is a local or network dir or file and if file(local or network (http://minio:9000/))
//     // then open in main window, if dir then open in customFileDialog
//     // QString prefix = "http://minio:9000/";
//     if(path.startsWith(prefix)) {
//         msghandler -> getNetStore(path);
//         return true;
//     }  // if exist and file, if exist and dir
//     else return false;
// }

Q_INVOKABLE void UnifiedStorageModel::setParent(const QString &fullPath, const QString &type){
    QFileInfo fileInfo(fullPath);
    qDebug() << "UnifiedStorageModel::setParent fileInfo.fileName():" << fileInfo.fileName();
    if(type == "mb") m_parentItem = {fileInfo.fileName(), fullPath, fullPath, true, true, true, false};
    if(type == "md") m_parentItem = {fileInfo.fileName(), fullPath, fullPath, true, true, false, false};
    if(type == "mf") m_parentItem = {fileInfo.fileName(), fullPath, fullPath, false, true, false, false};
}

Q_INVOKABLE QVariantMap UnifiedStorageModel::getParent(){
    QVariantMap res;

    //    Вручную наполняем карту данными
    res["name"] = m_parentItem.name;
    res["path"] = m_parentItem.path;
    res["cleanPath"] = m_parentItem.cleanPath;
    res["isDir"] = m_parentItem.isDirectory;
    res["isMinio"] = m_parentItem.isMinio;
    res["isMinioBucket"] = m_parentItem.isMinioBucket;
    res["isVirtualDir"] = m_parentItem.isVirtualDir;
    res["mongoId"] = m_parentItem.mongoId;
    return res;
}
