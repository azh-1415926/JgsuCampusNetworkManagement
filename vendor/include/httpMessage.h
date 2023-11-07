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

public:
    httpMessage(const std::string& method="GET",const std::string& url="",const std::string& version="HTTP/1.1");
    ~httpMessage();
    bool setMethod(const std::string& method);
    void setUrl(const std::string& url);
    std::string& url();
    void setContent(const std::string& content);
    std::string& content();
    void addParam(const std::string& key,const std::string& value);
    void addField(const std::string& key,const std::string& value);
    std::string removeField(const std::string& key);
    std::string createMessage();

private:
    bool isInvaildForMethod(const std::string& method);
};

#endif