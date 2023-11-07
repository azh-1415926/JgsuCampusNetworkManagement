#include "httpMessage.h"

httpMessage::httpMessage(const std::string& method,const std::string& url,const std::string& version)
    :m_Method(method),m_Url(url),m_Version(version)
{
}

httpMessage::~httpMessage()
{
}

bool httpMessage::setMethod(const std::string& method)
{
    if(isInvaildForMethod(method))
    {
        m_Method=method;
        return true;
    }
    return false;
}
void httpMessage::setUrl(const std::string& url)
{
    m_Url=url;
}

std::string& httpMessage::url()
{
    return m_Url;
}

void httpMessage::setContent(const std::string& content)
{
    m_Content=content;
}

std::string& httpMessage::content()
{
    return m_Content;
}

void httpMessage::addParam(const std::string& key,const std::string& value)
{
    m_Params.push_back(messageField(key,value));
}

void httpMessage::addField(const std::string& key,const std::string& value)
{
    m_Fields.push_back(messageField(key,value));
}

std::string httpMessage::removeField(const std::string& key)
{
    std::vector<messageField>::iterator i;
    for(i=m_Fields.begin();i!=m_Fields.end();i++)
        if(i->key==key)
            return i->value;
    return std::string();
}

std::string httpMessage::createMessage()
{
    std::string message;
    std::string params;
    for(int i=0;i<m_Params.size();i++)
    {
        params.append(m_Params[i].key);
        params.append("=");
        params.append(m_Params[i].value);
        if(i!=m_Params.size()-1)
            params.append("&");
    }
    if(m_Method=="GET")
        m_Url+="?"+params;
    message.append(m_Method);
    message.append(" ");
    message.append(m_Url);
    message.append(" ");
    message.append(m_Version);
    message.append("\n\r");
    for(int i=0;i<m_Fields.size();i++)
    {
        message.append(m_Fields[i].key);
        message.append(":");
        message.append(m_Fields[i].value);
        message.append("\n\r");
    }
    if(m_Method=="POST")
    {
        message.append("\n\r");
        message.append(params);
        message.append("\n\r");
    }
    return message;
}

bool httpMessage::isInvaildForMethod(const std::string& method)
{
    static std::string methods[8]=
    {
        "GET",      // 请求指定页面信息，并返回实体主体。
        "HEAD",     // 类似于get请求，只不过返回的响应中没有具体的内容，用于获取报头。
        "POST",     // 向指定资源提交数据进行处理请求（例如提交表单或上传文件），数据包含在请求体中。post请求可能会导致新的资源的建立或已有资源的修改。
        "PUT",      // 从客户端向服务器传送的数据取代指定的文档的内容。
        "DELETE",   // 请求服务器删除指定的页面。
        "CONNECT",  // HTTP/1.1协议中预留给能够将连接改为管道方式的代理服务器。
        "OPTIONS",  // 允许客户端查看服务器的性能。
        "TRACE"     // 回显服务器收到的请求，主要用于测试或诊断。
    };
    for(int i=0;i<8;i++)
        if(methods[i]==method)
            return true;
    return false;
}