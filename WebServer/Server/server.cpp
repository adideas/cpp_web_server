#include <string>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <map>

#include "request.cpp"
#include "response.cpp"

/**
 1) create a socket - Get the file descriptor!
 2) bind to an address -What port am I on?
 3) listen on a port, and wait for a connection to be established.
 4) accept the connection from a client.
 5) send/recv - the same way we read and write for a file.
 6) shutdown to end read/write.
 7) close to releases data.
 */

using std::string;
using std::map;
using std::function;

namespace server {

typedef function<Response(Request, Response)> callable;

class Server {
private:
    static const int BUFFER = 30000;
    int port = 80;
    int socketDescriptor = 0;
    struct sockaddr_in socketAddress;
    bool debuging = false;
    map<string, callable> routes;
    
    // create a socket - Get the file descriptor!
    int createInSocket() {
        int soket_id = socket(AF_INET, SOCK_STREAM, 0);
        if (soket_id == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        
        int opt = 1;
        if (setsockopt(soket_id, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        
        return soket_id;
    };
    
    // create socket address
    struct sockaddr_in createAddress() {
        struct sockaddr_in address_out;
        address_out.sin_family = AF_INET;
        address_out.sin_addr.s_addr = htonl(INADDR_ANY); // is default
        //address_out.sin_addr = address;
        address_out.sin_port = htons(port);
        memset(address_out.sin_zero, '\0', sizeof address_out.sin_zero);
        return address_out;
    };
    
    // bind to an address -What port am I on?
    void bindAddress(int socketDescriptor, struct sockaddr_in _address) {
        if (bind(socketDescriptor, (struct sockaddr*)&_address, sizeof(_address)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
    };
    
    // listen on a port, and wait for a connection to be established.
    void listenAddress(int socketDescriptor) {
        if (listen(socketDescriptor, 5) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
    };
    
    // accept the connection from a client.
    int pendingAccept(int socketDescriptor, struct sockaddr_in _address) {
        int addrlen = sizeof(_address);
        int socket_id = accept(socketDescriptor, (struct sockaddr *)&_address, (socklen_t*)&addrlen);
        if (socket_id < 0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        return socket_id;
    };
    
    // close to releases data.
    void endPendingAccept(int pendingAccept) {
        close(pendingAccept);
    };
    
    string request(int pendingAccept) {
        char buffer[BUFFER] = {0};
        read(pendingAccept, buffer, BUFFER);
        string str(buffer);
        return str;
    };
    
    void response(int pendingAccept, const char* text) {
        write(pendingAccept, text, strlen(text));
    };
    
    void response(int pendingAccept, string text) {
        response(pendingAccept, text.c_str());
    };
    
public:
    Server() {
        socketDescriptor = createInSocket();
        socketAddress = createAddress();
        bindAddress(socketDescriptor, socketAddress);
        listenAddress(socketDescriptor);
    };
    
    Server addListen(const char* host, int _port) {
        // without ip address
        // inet_pton(AF_INET, "127.0.0.1", &address);
        // without port
        // port = _port;
        return * this;
    };
    
    Server registerHandler(const string &pathPattern, callable &&function) {
        routes[pathPattern] = function;
        return * this;
    };
    
    Server debug(bool _debug) {
        debuging = _debug;
        return * this;
    };

    string what(const std::exception_ptr &eptr = std::current_exception())
    {
        if (!eptr) { throw std::bad_exception(); }

        try { std::rethrow_exception(eptr); }
        catch (const std::exception &e) { return e.what()   ; }
        catch (const std::string    &e) { return e          ; }
        catch (const char           *e) { return e          ; }
        catch (...){return "who knows";}
    }
    
    bool run() {
        while(1) {
            if (debuging)
            std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n";
            
            int new_socket = pendingAccept(socketDescriptor, socketAddress);
            Request data = Request(request(new_socket));
            string route_name = data.getRoute();
            
            if (debuging)
            std::cout << data.getRaw() << std::endl;
            
            if (routes[route_name]) {
                callable func = routes[route_name];
                Response res;
                try {
                    res = func(data, res);
                    response(new_socket, res.toString());
                } catch (...) {
                    response(new_socket, Response::page500(what()));
                }
            } else {
                response(new_socket, Response::page404(""));
            }
            
            if (debuging)
            std::cout << "------------------Hello message sent-------------------\n";
            
            endPendingAccept(new_socket);
        }
    }
};

inline Server app()
{
    return Server();
};

class Response;
class Request;
};
