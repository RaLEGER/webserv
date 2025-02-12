#ifndef RESPONSE_HPP
#define RESPONSE_HPP 

#include "../Request/Request.hpp"

#include <string> 
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

class Response {
    private:
        std::string protocol;
        std::string statusCode;
        std::string statusText;
        std::string contentType;
        std::string contentLength;
        std::string filename;
        std::string extension;
        std::string contentDisposition;

        std::string header;
        std::string body;
        std::string response;

        Request *request;

    public:
        Response();
        Response(int statusCode, std::string error_msg); // Constructor for error response
        
        ~Response();

        /* Getters & Setters */
        void setProtocol(std::string protocol);
        void setStatusCode(std::string statusCode);
        void setStatusText(std::string statusText);
        void setContentType(std::string contentType);
        void setFilename(std::string filename);
        void setExtension(std::string extension);
        void setContentDisposition(std::string contentDisposition);
        void setContentLength(std::string contentLength);
        void setBody(std::string content);
        void setRequest(Request *request);
        void setDefaultSuccess();
        std::string getSerializedResponse();
        std::string getResponse();
        std::string getHeader();
        std::string getBody();
        std::string getProtocol();
        std::string getStatusCode();
        std::string getStatusText();
        std::string getContentType();
        std::string getFilename();
        std::string getExtension();
        std::string getContentDisposition();

        void buildHeader();
        void setError(int statusCode, std::string error_msg);
        void loadFileContent(const std::string& path);
        void clear();

};

#endif