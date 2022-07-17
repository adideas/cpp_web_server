#include <string>
#include <sstream>
#include <map>

using std::string;
using std::endl;
using std::stringstream;
using std::map;

namespace server {

class Response {
private:
    map<int, string> statusText = {
        {100, "Continue"},{101, "Switching Protocols"},{200, "OK"},{201, "Created"},{202, "Accepted"},{203, "Non-Authoritative Information"},
        {204, "No Content"},{205, "Reset Content"},{206, "Partial Content"},{300, "Multiple Choices"},{301, "Moved Permanently"},{302, "Found"},
        {303, "See Other"},{304, "Not Modified"},{305, "Use Proxy"},{307, "Temporary Redirect"},{400, "Bad Request"},{401, "Unauthorized"},
        {402, "Payment Required"},{403, "Forbidden"},{404, "Not Found"},{405, "Method Not Allowed"},{406, "Not Acceptable"},
        {407, "Proxy Authentication Required"},{408, "Request Timeout"},{409, "Conflict"},{410, "Gone"},{411, "Length Required"},
        {412, "Precondition Failed"},{413, "Request Entity Too Large"},{414, "Request-URI Too Long"},{415, "Unsupported Media Type"},
        {416, "Requested Range Not Satisfiable"},{417, "Expectation Failed"},{500, "Internal Server Error"},{501, "Not Implemented"},
        {502, "Bad Gateway"},{503, "Service Unavailable"},{504, "Gateway Timeout"},{505, "HTTP Version Not Supported"}
    };
    map<string, string> headers = {
        {"Content-Type", "text/html; charset=utf-8"}, {"Server", "No server"}, {"Cache-Control", "private, max-age=0"}
    };
    string body;
    int statusCode = 200;
    
public:
    Response() {};
    void set(string nameHeader, string valueHeader) {
        headers[nameHeader] = valueHeader;
    };
    void set(map<string, string> new_headers) {
        for (const auto& hs : new_headers) {
            headers[hs.first] = hs.second;
        }
    };
    Response status(int status) {
        statusCode = status;
        return * this;
    };
    void send(int status) {
        body.clear();
        statusCode = status;
        raw("");
    };
    void send(string text) {
        body.clear();
        raw(text);
    };
    void raw(string _raw) {
        body = _raw;
    };
    string toString() {
        stringstream headers_str;
        for (const auto& hs : headers) {
            headers_str << hs.first << ": " << hs.second << endl;
        }
        
        stringstream res;
        res << "HTTP " << statusCode << " " << getStatusText(statusCode) << endl;
        res << "Content-Length: " << body.length() << endl;
        res << headers_str.str();
        res << endl;
        res << body;
        return res.str();
    };
    
    string getStatusText(int code) {
        if (statusText.count(code) < 1) {
            return "Not supported status code.";
        }
        return statusText[code];
    };
    
    void exception(string error) {
        exception(error, 500);
    };
    
    void exception(string error, int status) {
        body.clear();
        statusCode = status;
        raw(error);
    };
    
    static string page500(string error) {
        Response response = Response();
        stringstream res;
        res << "HTTP 500 " << response.getStatusText(500) << endl;
        res << "Content-Length: " << error.length() << endl;
        res << endl;
        res << error;
        
        return res.str();
    };
    
    static string page404(string text) {
        Response response = Response();
        stringstream res;
        res << "HTTP 404 " << response.getStatusText(404) << endl;
        res << "Content-Length: " << text.length() << endl;
        res << endl;
        res << text;
        
        return res.str();
    };
};
};
