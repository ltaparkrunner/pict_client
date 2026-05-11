#ifndef AUXILARY_H
#define AUXILARY_H
#include <QString>
#include <QUrl>
#include <QUrlQuery>

QString cleanNetworkFilePath(QString networkFilePath);
QString cleanLocalFilePath(QString localFilePath);
QString cleanFilePath(QString filePath, bool isNetwork);

#endif // AUXILARY_H
