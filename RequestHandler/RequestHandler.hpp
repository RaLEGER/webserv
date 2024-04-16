#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "../Response/Response.hpp"
#include "../Request/Request.hpp"

class RequestHandler {
public:

    // Canonical
    RequestHandler();
    RequestHandler(Request &request);
    ~RequestHandler();

    // Handling methods
    void listDirectory();
    void process();
    void handleRequest(std::string requestString, int clientSocket);
    void setResponseHeaders();
    void getFinalPath();

    // Getters & Setters
    void getRequest(Request &request);
    Response &getResponse();

private:
    Response _response;
    Request _request;
    std::string path;

    // Handling methods
    void Get();
    void Post();
    void Delete();
    void CGI();

    std::string getSerializedResponse();
};

#endif // REQUESTHANDLER_HPP