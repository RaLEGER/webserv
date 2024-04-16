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

#include "CGIHandler.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Utils.hpp"

class RequestHandler {
    public:

        // Canonical
        RequestHandler();
        RequestHandler(std::string requestString, int clientSocket);
        ~RequestHandler();

        // Handling methods
        void listDirectory();
        void process();
        void handleRequest();
        void setResponseHeaders();
        void getFinalPath();

        // Getters & Setters
        std::string getResponseString();
        Response &getResponse();

    private:
        Response _response;
        Request _request;
        // CGIHandler _cgiHandler;
        int _clientSocket;
        std::string path;

        // Handling methods
        void Get();
        void Post();
        void Delete();
        void CGI();

        std::string getSerializedResponse();
};

#endif // REQUESTHANDLER_HPP