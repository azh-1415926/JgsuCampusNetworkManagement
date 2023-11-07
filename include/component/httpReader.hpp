#ifndef HTTP_READER_H
#define HTTP_READER_H

#include "settingFile.h"
#include "httpMessage.h"

class httpReader
{
private:

public:
    static QString load(const QString& filePath)
    {
        settingFile setting;
        setting.load(filePath);
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