#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <string>
#include <vector>

struct messageField
{
    std::string key;
    std::string value;
    messageField(const std::string& key,const std::string& value)
    {
        this->key=key;
        this->value=value;
    }
};

class httpMessage
{
private:
    std::string m_Method;
    std::string m_Url;
    std::string m_Version;
    std::vector<messageField> m_Fields;
    std::vector<messageField> m_Params;
    std::string m_Content;
    std::string m_Host;
    int m_Port;

public:
    httpMessage(const std::string& method="GET",const std::string& url="",const std::string& version="HTTP/1.1")
        :m_Method(method),m_Url(url),m_Version(version){ }
    ~httpMessage(){}
    inline bool setMethod(const std::string& method)
    {
        if(isInvaildForMethod(method))
        {
            m_Method=method;
            return true;
        }
        return false;
    }
    inline void setUrl(const std::string& url){ m_Url=url; }
    inline std::string& url(){ return m_Url; }
    inline void setContent(const std::string& content){ m_Content=content; }
    inline std::string& content(){ return m_Content; }
    inline void setHost(const std::string& host){ m_Host=host; }
    inline std::string& host(){ return m_Host; }
    inline void setPort(int port){ if(port>0&&port<65535) m_Port=port; }
    inline int port(){ return m_Port; }
    inline void addParam(const std::string& key,const std::string& value){ m_Params.push_back(messageField(key,value)); }
    inline void addField(const std::string& key,const std::string& value){ m_Fields.push_back(messageField(key,value)); }
    std::string removeField(const std::string& key)
    {
        std::vector<messageField>::iterator i;
        for(i=m_Fields.begin();i!=m_Fields.end();i++)
            if(i->key==key)
                return i->value;
        return std::string();
    }
    std::string createMessage()
    {
        std::string message;
        std::string params;
        for(int i=0;i<m_Params.size();i++)
        {
            params.append(m_Params[i].key+"="+m_Params[i].value);
            if(i!=m_Params.size()-1)
                params.append("&");
        }
        if(m_Method=="GET")
            m_Url+="?"+params;
        message.append(m_Method+" "+m_Url+" "+m_Version+"\n\r");
        for(int i=0;i<m_Fields.size();i++)
            message.append(m_Fields[i].key+":"+m_Fields[i].value+"\n\r");
        if(m_Method=="POST")
            message.append("\n\r"+params+"\n\r");
        return message;
    }

private:
    bool isInvaildForMethod(const std::string& method)
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
};

#endif