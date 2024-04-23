#ifndef REQUEST_HPP
#define REQUEST_HPP 

#include <map>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <cstdlib>
#include <list>

#include "Utils.hpp"

// TYPES 
#define GET 1
#define POST 2
#define DELETE 3
#define UNKNOWN 0

#define MAX_URI_LEN 100
#define MAX_HEADER_LEN 5000

#define CGI_BUFFER_SIZE 2046

class Request {
    private:
        /* Header */
        std::string hostname;
        std::string method;
        std::string path;
        std::string originalPath;
        std::string protocol;
        std::string headersString;
        std::string body;
        std::string query;
        std::string responseString;
        std::string fileContent;
        std::map<std::string, std::string> headers;

        int methodCode;

        // Location*       _config;
        // Response            _response;  
        int clientSocket;
        int serverSocket;

    public:

        std::string header;
        std::string executable_path;
        std::string script_path;

        // bool                    chunkedBody;
        // bool                    readingBody;
        // bool                    readingHeader;
        // std::string             requestBodyString;
        // std::string             requestHeaderString;
        // std::list<std::string>  requestBodyList;
        // int                     curChunkSize; 
        int                     contentLength;
	    // unsigned long	        curRequestTime;
	    // unsigned long	        lastActivityTime;

        // bool                    ready2send;

        /* Constructors & Destructors */
        Request();
        Request(std::string &rawRequest);
        Request(int clientSocket, int serverSocket);
        ~Request();

        /* Getters & Setters */
        std::string         getPath();
        std::string         getProtocol();
        std::string         getMethod();
        std::string         getBody();
        std::string         getQuery();
        std::string         getHostname();
        int                 getMethodCode();
        int                 getClientSocket();
        int                 getServerSocket();
        std::string         getResponseString();
        std::string         getHeader(std::string key);
        void                addHeader(std::string key, std::string value);
        std::map<std::string, std::string>        &getHeaders();
        void                getRequestConfig();

        void                setBody(std::string &body);
        void                appendToBody(std::string &body);
        void                setFileContent(std::string &fileContent);
        void                setResponseString(std::string &response);
        bool                isChunked();

        /* Methods */

        // handling methods
        // void routingGet();
        // void routingPost();
        // void routingDelete();
        // void routingCGI();
        // void handleRequest(); 
        // void buildResponse();
        // void listDirectoryResponse();

        // parsing methods
        bool parse();
        void parseMethodToken(const std::string& token);
        bool parseHTTPVersion(const std::string& token);
        bool parseHeaders(std::string headersString);
        bool parseHeadersList(std::string headersString);
        bool parseBody();
        bool parseURI(std::string token);
        
        // utils
        bool fileExists();
        void clear();
        int  getChunkedBodySize();
};

std::ostream& operator<<(std::ostream& o, Request& req);

#endif
