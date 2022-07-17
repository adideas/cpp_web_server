#include <iostream>
#include "Server/server.cpp"
#include <string>

using std::string;
using server::app;
using server::Request;
using server::Response;

int main(int argc, const char * argv[]) {
    
    string a = "sdf";
    
    auto method1 = [](Request test, Response res) {
        res.raw("Hello my frend " + test.query()["t"]);
        // res.exception("it's the end of the world!");
        return res;
    };
    
    app()
    // .debug(false)
    .addListen("127.0.0.1", 80)
    .registerHandler("POST:/", method1)
    .registerHandler("GET:/test/getInfo", method1)
    .run();
    
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
