#ifndef SETTING_READER_H
#define SETTING_READER_H

#include "httpReader.hpp"

class settingReader
{
private:

public:
    static QList<QPair<QString,QList<QJsonObject>>> load(const QJsonObject& json)
    {
        QList<QPair<QString,QList<QJsonObject>>> hosts;
        if(json.isEmpty())
            return hosts;
        const QStringList& keysOfHosts=json.keys();
        for(const auto& i : keysOfHosts)
        {
            const QJsonValue& value=json.value(i);
            if(!value.isObject())
                continue;
            const QJsonObject& json=value.toObject();
            const QJsonObject& HostInfo=json.value("HostInfo").toObject();
            const QString& host=HostInfo.value("host").toString();
            int port=HostInfo.value("port").toInt();
            const QJsonValue& HttpMessages=json.value("HttpMessage");
            QList<QJsonObject> list;
            if(HttpMessages.isArray())
            {
                list.clear();
                QJsonArray array=HttpMessages.toArray();
                for(auto j : array)
                {
                    QJsonObject message=j.toObject();
                    message.insert("Host",host+":"+QString::number(port));
                    list<<message;
                }
                hosts.push_back(QPair<QString,QList<QJsonObject>>(i,list));
            }
            else if(HttpMessages.isObject())
            {
                QJsonObject message=HttpMessages.toObject();
                QJsonObject fields=message.value("fields").toObject();
                fields.insert("Host",host+":"+QString::number(port));
                message.insert("fields",fields);
                list<<message;
                hosts.push_back(QPair<QString,QList<QJsonObject>>(i,list));
            }
        }
        return hosts;
    }
    static QList<QPair<QString,QStringList>> load(const QString& filePath)
    {
        settingFile setting;
        QList<QPair<QString,QStringList>> messages;
        setting.load(filePath);
        if(!(setting.isLoad()))
            return messages;
        const QStringList& keysOfHosts=setting.keys();
        for(const auto& i : keysOfHosts)
        {
            const QJsonValue& value=setting.value(i);
            if(!value.isObject())
                continue;
            const QJsonObject& json=value.toObject();
            const QJsonObject& HostInfo=json.value("HostInfo").toObject();
            const QString& host=HostInfo.value("host").toString();
            int port=HostInfo.value("port").toInt();
            const QJsonValue& HttpMessages=json.value("HttpMessage");
            if(HttpMessages.isArray())
            {
                QStringList list;
                QJsonArray array=HttpMessages.toArray();
                for(auto j : array)
                {
                    QJsonObject HttpMessage=j.toObject();
                    HttpMessage.insert("Host",host+":"+QString::number(port));
                    list<<httpReader::load(HttpMessage);
                }
                messages.push_back(QPair<QString,QStringList>(i,list));
            }
            else if(HttpMessages.isObject())
            {
                QJsonObject HttpMessage=HttpMessages.toObject();
                HttpMessage.insert("Host",host+":"+QString::number(port));
                messages.push_back(QPair<QString,QStringList>(i,httpReader::load(HttpMessage)));;
            }
        }
        return messages;
    }

private:
    settingReader();
};

#endif