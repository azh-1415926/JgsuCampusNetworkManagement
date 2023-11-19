#ifndef SETTING_READER_H
#define SETTING_READER_H

#include "httpReader.hpp"

class settingReader
{
private:

public:
    /* 导入 json 对象，返回各个主机的关键字及对应的存储着 http 报文( json 对象)列表 */
    static QList<QPair<QString,QList<QJsonObject>>> load(const QJsonObject& json)
    {
        /* 存储配置文件中的所有主机的关键字及想发送的报文 */
        QList<QPair<QString,QList<QJsonObject>>> hosts;
        if(json.isEmpty())
            return hosts;
        /* 获取配置文件中各个主机的关键字 */
        for(const auto& i : json.keys())
        {
            /* 获取第 i 个关键字对应的值 */
            const QJsonValue& value=json.value(i);
            /* 若不为对象，则当前主机的配置文件有误，转到下一个 */
            if(!value.isObject())
                continue;
            /* json 为该主机的配置信息的对象 */
            const QJsonObject& json=value.toObject();
            /* HostInfo 是包含该主机的 ip 和端口号的对象 */
            const QJsonObject& HostInfo=json.value("HostInfo").toObject();
            /* host 为该主机的 ip、port 为该主机的端口号 */
            const QString& host=HostInfo.value("host").toString();
            int port=HostInfo.value("port").toInt();
            /* HttpMessages 为该想向主机的发送的所有报文( json 对象)，单个报文为对象，多个则为数组 */
            const QJsonValue& HttpMessages=json.value("HttpMessage");
            /* list 为存储向该主机发送报文( json 对象)的列表 */
            QList<QJsonObject> list;
            /* 若不止一个报文 */
            if(HttpMessages.isArray())
            {
                /* 重复使用 list，清空 list */
                list.clear();
                /* array 为 json 对象数组，存储着若干个报文对象 */
                QJsonArray array=HttpMessages.toArray();
                /* 遍历所有报文对象，用 j 指向其中一个 */
                for(auto j : array)
                {
                    /* message 为 j 转化为的 json 对象 */
                    QJsonObject message=j.toObject();
                    /* fields 为报文中的字段 */
                    QJsonObject fields=message.value("fields").toObject();
                    /* 往 fields 中插入 Host 字段 */
                    fields.insert("Host",host+":"+QString::number(port));
                    /* 并刷新 message 中的 fields */
                    message.insert("fields",fields);
                    /* 将最后得到的报文插入列表 */
                    list<<message;
                }
                /* 将当前主机的关键字及所有报文的列表插入 host */
                hosts<<QPair<QString,QList<QJsonObject>>(i,list);
            }
            /* 若只有一个报文 */
            else if(HttpMessages.isObject())
            {
                /* message 为报文转化为的 json 对象 */
                QJsonObject message=HttpMessages.toObject();
                /* fields 为报文中的字段 */
                QJsonObject fields=message.value("fields").toObject();
                /* 往 fields 中插入 Host 字段 */
                fields.insert("Host",host+":"+QString::number(port));
                /* 并刷新 message 中的 fields */
                message.insert("fields",fields);
                /* 将最后得到的报文插入列表 */
                list<<message;
                /* 将当前主机的关键字及所有报文的列表插入 host */
                hosts<<QPair<QString,QList<QJsonObject>>(i,list);
            }
        }
        return hosts;
    }
    static QList<QPair<QString,QStringList>> load(const QString& filePath)
    {
        /* 读取 filePath 位置的配置文件 */
        settingFile setting(filePath);
        QList<QPair<QString,QStringList>> messages;
        if(!(setting.isLoad()))
            return messages;
        /* 取出其中的 json 对象，并导出其中所有的主机对应关键字及报文列表 */
        for(const auto& i : settingReader::load(setting.toJson()))
        {
            /* list 为存储报文字符串的列表 */
            QStringList list;
            /* 将所有报文对象转化为字符串形式，并插入 list 中 */
            for(const auto& j : i.second)
                list<<httpReader::load(j);
            /* 将当前主机的关键字及所有报文的列表插入 messages */
            messages<<QPair<QString,QStringList>(i.first,list);
        }
        return messages;
    }

private:
    settingReader();
};

#endif