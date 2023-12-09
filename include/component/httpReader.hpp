#ifndef HTTP_READER_H
#define HTTP_READER_H

#include "settingFile.hpp"
#include "httpMessage.hpp"

class httpReader
{
private:

public:
    /* 读取指定 QJsonObject 文件中的 http报文 */
    static QString load(const QJsonObject& json)
    {
        /* json 为空便返回 */
        if(json.isEmpty())
            return QString();
        /* httpMessage 用于拼接报文 */
        httpMessage message;
        message.setMethod(json.value("method").toString().toStdString());
        message.url()=json.value("url").toString().toStdString();
        /* 对应 field 属性和 params 属性需要特殊处理 */
        const QJsonObject& fields=json.value("fields").toObject();
        const QJsonObject& params=json.value("params").toObject();
        bool hasCookie=json.value("Cookie").toBool();
        for(const auto& i:fields.keys())
            if(!hasCookie&&i!="Cookie"||hasCookie&&i=="Cookie")
                message.addField(i.toStdString(),fields.value(i).toString().toStdString());
        for(const auto& i:params.keys())
            message.addParam(i.toStdString(),params.value(i).toString().toStdString());
        return QString::fromStdString(message.createMessage());
    }
    /* 读取指定 json 文件内的 http 报文 */
    static QString load(const QString& filePath)
    {
        settingFile setting(filePath);
        if(!(setting.isLoad()))
            return QString();
        httpMessage message;
        message.setMethod(setting.value("method").toString().toStdString());
        message.url()=setting.value("url").toString().toStdString();
        const QJsonObject& fields=setting.value("fields").toObject();
        const QJsonObject& params=setting.value("params").toObject();
        for(const auto& i:fields.keys())
            message.addField(i.toStdString(),fields.value(i).toString().toStdString());
        for(const auto& i:params.keys())
            message.addParam(i.toStdString(),params.value(i).toString().toStdString());
        return QString::fromStdString(message.createMessage());
    }

private:
    httpReader();
};

#endif