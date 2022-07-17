#include <string>
#include <map>
#include <iostream>

using std::string;
using std::map;

namespace server {
enum RequestContentType {
    JSON,
    FORM_DATA,
    FORM_URL_ENCODED
};

class Request {
private:
    string RAW = "";
    string METHOD = "";
    string ROUTE = "";
    string TYPE = "";
    string BODY = "";
    map <string, string> QUERY;
    map <string, string> HEADERS;
    RequestContentType requestContentType;
    
    string split(string & raw, string delimiter) {
        size_t pos = raw.find(delimiter);
        string value = "";
        if (pos != string::npos) {
            value = raw.substr(0, pos);
            raw.erase(0, pos + delimiter.length());
        } else {
            pos = raw.length();
            value = raw.substr(0, pos);
            raw.erase(0, raw.length());
        }
        return value;
    };
    void parseHeader(string raw) {
        do {
            string value = split(raw, "\r\n");
            string key = split(value, ":");
            if (value.front() == ' ') {
                value = value.erase(0, 1);
            }
            HEADERS[key] = value;
        } while(raw.length() > 0);
    };
    void parseQuery(string raw) {
        do {
            
            string queryItem = split(raw, "&");
            string delimiter = "=";
            size_t pos = queryItem.find(delimiter);
            string key = queryItem.substr(0, pos);
            string value = queryItem.substr(pos + 1, queryItem.length());
            QUERY[key] = value;
        } while(raw.length() > 0);
    };
    void parseContentType(string Content_Type) {
        for (auto & c: Content_Type) c = toupper(c);
        Content_Type = split(Content_Type, ";");
        if (Content_Type == "MULTIPART/FORM-DATA") {
            requestContentType = RequestContentType::FORM_DATA;
            return;
        }
        if (Content_Type == "APPLICATION/JSON") {
            requestContentType = RequestContentType::JSON;
            return;
        }
        if (Content_Type == "APPLICATION/X-WWW-FORM-URLENCODED") {
            requestContentType = RequestContentType::FORM_URL_ENCODED;
            return;
        }
        if (Content_Type[0] == 'A' && Content_Type[10] == 'N') {
            if (Content_Type[12] == 'J' && Content_Type[15] == 'N') {
                requestContentType = RequestContentType::JSON;
                return;
            }
            if (Content_Type[12] == 'X' && Content_Type[18] == 'F') {
                requestContentType = RequestContentType::FORM_URL_ENCODED;
                return;
            }
        }
        if (Content_Type[0] == 'M' && Content_Type[8] == 'T' && Content_Type[10] == 'F' && Content_Type[18] == 'A') {
            requestContentType = RequestContentType::FORM_DATA;
            return;
        }
    };
public:
    Request(string raw) {
        RAW = raw;
        METHOD = split(raw, " ");
        for (auto & c: METHOD) c = toupper(c);
        string route = split(raw, " ");
        ROUTE = split(route, "?");
        parseQuery(route);
        TYPE = split(raw, "\r\n");
        string headers = split(raw, "\r\n\r\n");
        BODY = raw;
        parseHeader(headers);
        if (HEADERS["Content-Type"].length()) {
            parseContentType(HEADERS["Content-Type"]);
        }
    };
    string getRaw() {
        return RAW;
    };
    string getRoute() {
        return METHOD + ":" + ROUTE;
    };
    map<string, string> query() {
        return QUERY;
    };
    map<string, string> headers() {
        return HEADERS;
    };
    string body() {
        return BODY;
    };
    string method() {
        return METHOD;
    }
};
};
