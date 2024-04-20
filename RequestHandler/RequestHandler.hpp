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
#include "Location.hpp"
#include "Request.hpp"
#include "Utils.hpp"

class Server;
class Router;

class RequestHandler {
    public:

        // Canonical
        RequestHandler();
        RequestHandler(std::string requestString, int clientSocket);
        ~RequestHandler();

        // Handling methods
        void process(std::vector<Server *> servers);
        void handleRequest();
        void setResponseHeaders();
        void buildFinalPath();

        // Getters & Setters
        std::string getResponseString();
        Response &getResponse();
        int _clientSocket;

    private:
        Response    _response;
        Request     _request;
        Server      *_server;
        Location    *_location;

        CGIHandler *_cgiHandler;
        std::string path;

        // Handling methods
        void Get();
        void Post();
        void Delete();
        void CGI();
        void redirect(std::string address);
        void listDirectory();

};

#endif // REQUESTHANDLER_HPP