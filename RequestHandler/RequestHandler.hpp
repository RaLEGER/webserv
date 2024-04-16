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
    void listDirectory();
    void handleRequest();
    void Get();
    void Post();
    void Delete();
    void CGI();
    void setResponseHeaders();
    void getFinalPath();

    // Canonical
    RequestHandler();
    RequestHandler(Request &request);
    ~RequestHandler();

    // Getters & Setters
    void getRequest(Request &request);
    Response &getResponse();

private:
    Response _response;
    Request _request;
    std::string path;

    std::string getSerializedResponse();
};

#endif // REQUESTHANDLER_HPP