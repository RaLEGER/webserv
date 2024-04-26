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
        RequestHandler(int clientSocket);
        ~RequestHandler();

        // Parsing
        bool parseHeaders(std::string headers,  std::vector<Server *> servers);
        void setBody(std::string body);

        // Handling 
        void process(std::vector<Server *> servers);
        void handleRequest();
        void setResponseHeaders();
        void buildFinalPath();

        // Getters & Setters
        int getContentLength();
        bool getIsBodyComplete();
        void setIsBodyComplete(bool value);
        std::string getResponseString();
        Response &getResponse();
        int _clientSocket;
        bool getIsChunkedRequest();
        bool getIsValidHeaders();

    private:
        Response    _response;
        Request     _request;
        Server      *_server;
        Location    *_location;

        bool        isBodyComplete;
        bool        isChunkedRequest;
        bool        isValidHeaders;

        //CGIHandler *_cgiHandler;
        std::string path;

        // Handling methods
        void Get();
        void Post();
        void Delete();
        void CGI();
        void redirect(std::string address);
        void listDirectory();

        // Helpers
        void getFileContent(std::string filepath);
        void handleGetDirectory();
};

#endif // REQUESTHANDLER_HPP