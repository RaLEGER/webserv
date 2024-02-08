#include "Request.hpp"
#include "CustomError/CustomError.hpp"

#include <fstream>
#include <algorithm>
#include <vector>

// todo : move this to utils
std::vector<std::string> splitWithSep(std::string line, char sep)
{
	std::vector<std::string> res;
	std::istringstream s(line);

	while (std::getline(s, line, sep))
		if (!line.empty())
			res.push_back(line);
	
	return res;
}

Request::Request(int clientSocket, int serverSocket) : clientSocket(clientSocket), serverSocket(serverSocket) {
    // std::cout << "Request created" << std::endl;
    // readingHeader = false;
    // readingBody = false;
    // chunkedBody = false;
    // curChunkSize = -1;
    // contentLength = -1;
    // ready2send = false;
    // lastActivityTime = ft_now();
    // curRequestTime = 0;
    // _config = NULL;
}

Request::Request(std::string &rawRequest) {
    // std::cout << "Request Constructor with RawRequest" << std::endl;
    requestString = rawRequest;
    parseRequest();
}

        /* Constructors & Destructors */
Request::Request()
{
    
}


Request::~Request() {
    std::cout << "Request destroyed" << std::endl;
}

// This function parses the request string and fills the request object
bool Request::parseRequest(){

    // lastActivityTime = ft_now();

    // Extract the first line of the request
    std::string firstLine = requestString.substr(0, requestString.find("\r\n"));
    std::vector<std::string> tokens = splitWithSep(firstLine, ' ');
   
    // Check that the first line has the correct number of tokens (3) 
    if (tokens.size() != 3) 
        throw CustomError(400, "Invalid number of tokens in first line of request");

    // Parse the method, URI, HTTP version and headers
    parseMethodToken(tokens[0]);
    parseURI(tokens[1]);
    parseHTTPVersion(tokens[2]);
    parseHeaders();

    // if the method is POST, PUT or DELETE, parse the body
    if (method == "POST" || method == "PUT" || method == "DELETE")
        parseBody();
        
    // Route the request to the correct virtual server and location 
    // todo : do this in a separate function / other part of the code
    // getRequestConfig();
    // if (_config == NULL)
    // {    
    //     // _response.setError(404, ": virtual server configuration not found");
    //     return false;
    // }

    // Set the complete path to the resource (including root and index)
    // if (path == "/") {
    //     if (!_config->getRoot().empty()){
    //         path = path + _config->getRoot() + "/";
    //     }
    //     path = path + _config->getIndex();
    // }
    // path = "." + path;

    // WTF is this step ?
    // if(path.find(_config->getName()) != std::string::npos && _config->getType() == "std") {
    //     path.replace(path.find(_config->getName()), _config->getName().length(), _config->getRoot());
    //     if (!::fileExists(path) && method == "GET" && !_config->getRoot().empty())
    //     {
    //             path = path + "/" + _config->getIndex();
    //     }
    // }

    // std::cout << " ---------- HEADER VALUES ---------- " << std::endl;
    // std::cout << "Method: " << method << std::endl;
    // std::cout << "Path: " << path << std::endl;
    // std::cout << "Protocol: " << protocol << std::endl;
    // std::cout << "Query: " << query << std::endl;
    return true;
}

// This function 
// 1. checks the method token validity
// 2. sets the method attribute of the request object
void Request::parseMethodToken(const std::string& token)
{
	std::string methods[5] = {"GET", "HEAD", "POST", "DELETE", "PUT"};
	
	for (int i = 0; i < 5; ++i)
	{
		if (!token.compare(0, methods[i].size(), methods[i]) &&
				token.size() == methods[i].size())
		{
			method = methods[i];
			return ;
		}
	}
    throw CustomError(400, "Unknown HTTP method");
}

// This function 
// 1. checks the URI validity
// 2. sets the path and query attributes of the request object
bool Request::parseURI(std::string token)
{
    if (token.size() > MAX_URI_LEN)
        throw CustomError(414, "URI too long");

    if (token[0] != '/')
        throw CustomError(400, "URI must begin with a /");
  
    query = "";

	size_t querryChar = token.find("?");
	if (querryChar != std::string::npos)
	{
		path = token.substr(0, querryChar);
		query = token.substr(querryChar + 1, token.size());
	}
	else
		path = token;
    return true;
}


//  Check that the HTTP version is correct and set the protocol attribute of the request object
bool Request::parseHTTPVersion(const std::string& token)
{
	if (token.size() < 7 || token.compare(0, 5, "HTTP/") || token.compare(6, 1, ".") || 
			!isdigit(static_cast<int>(token[5])) || !isdigit(static_cast<int>(token[7])))
    {
        throw CustomError(400, "Invalid HTTP version");
    }
	protocol = token;
    return true;
}

void Request::addHeader(std::string key, std::string value)
{
    // Check if the header already exists
    if (headers.count(key) == 1)
        throw CustomError(400, "Duplicate headers are not allowed");

    headers[key] = value;
}

// Check headers validity, parse and set them
bool Request::parseHeaders()
{

    std::cout << "Parsing headers" << std::endl;
    std::cout << "RequestString: " << requestString << std::endl;

    // if (requestHeaderString.size() > MAX_HEADER_LEN)
    //     throw CustomError(431, "Header too long");

    std::string delimiter = "\r\n";
    std::string separator = ": ";
    std::string tmpRequestString = requestString;

    // If the request string does not contain a double line break, there are no headers
    size_t sepPos = tmpRequestString.find(separator);
    if (sepPos == std::string::npos) 
        throw CustomError(400, "No headers found in request. Host header is required.");

    // Iterate over the request string line by line
    size_t delimiterPos = tmpRequestString.find(delimiter);
    while (delimiterPos != std::string::npos)
    {
        // extract the current line and remove it from the request string
        std::string line = tmpRequestString.substr(0, delimiterPos);
        std::cout << "Parsing line: " << line << std::endl;
        tmpRequestString.erase(0, delimiterPos + delimiter.length());

        // If the line is empty, the headers are finished
        if (line.empty()) 
            break;

        // If the line contains a header separator, add header
        sepPos = line.find(separator);

        if(sepPos != std::string::npos) {

            // Extract the key and value from the line
            std::string key = line.substr(0, sepPos);
            std::string value = line.substr(sepPos + 2);

            addHeader(key, value);
        }
        delimiterPos = tmpRequestString.find(delimiter);
    }

    //Check that Host header is present
    if (headers.count("Host") == 0)
        throw CustomError(400, "Host header is required");

    //get contentLength if any
    if (headers.count("Content-Length") == 1)
    {
        char*               endPtr      = NULL;
        contentLength = static_cast<unsigned int>(strtoul(headers["Content-Length"].c_str(), &endPtr, 0));
    }
    
    return true;
}

bool Request::parseBody()
{
    std::size_t maxBodySize = 1000000; // 1MB, to set in config

    std::cout << "Parsing body" << std::endl;
    
    // Check that the body size does not exceed the limit
    if (requestString.size() > maxBodySize) 
        throw CustomError(413, "Body exceeds size limit");

    // Find the position of the first double line break
    size_t bodyStartPos = requestString.find("\r\n\r\n");
    if (bodyStartPos == std::string::npos) 
    {
        std::cout << "NO DOUBLE LINE BREAKS" << body << std::endl; 
        // Double line break not found, body is empty
        return true;
    }

    // Extract the body from the requestString
    body = requestString.substr(bodyStartPos + 4);

    return true;
}

// GETTERS

std::string Request::getPath() {
    return path;
}

std::string Request::getProtocol() {
    return protocol;
}

std::string Request::getBody() {
    return body;
}

std::string Request::getQuery() {
    return query;
}

std::string Request::getMethod() {
    return method;
}

std::map<std::string, std::string>  &Request::getHeaders()
{
    return headers;
}

int Request::getMethodCode() {
    return methodCode;
}

std::string Request::getRequestString() {
    return requestString;
}

int Request::getClientSocket() {
    return clientSocket;
}

int Request::getServerSocket() {
    return serverSocket;
}

// std::string Request::getResponseString() {
//     return _response.getResponse();
// }

std::string Request::getHeader(std::string key) {
    if (headers.count(key) == 1)
        return headers[key];
    return "";
}

// SETTERS 

// void Request::setFileContent(std::string &fc)
// {
//     std::cout << "Setting file content" << std::endl;
//     // std::cout << fc << std::endl;
//     fileContent = fc;
//     _response.setBody(fileContent);
// }

void Request::setResponseString(std::string &rs)
{
    responseString = rs;
}

void Request::setRequestString(std::string &rs)
{
    requestString = rs;
}

void Request::appendRequestString(std::string rs)
{
    requestString = requestString + rs;
}

void Request::clear(void)
{
    method.clear();
    path.clear();
    originalPath.clear();
    protocol.clear();
    requestString.clear();
    body.clear();
    responseString.clear();
    fileContent.clear();
    headers.clear();
    // std::cout << "CLIENT SOCKET CLEARED: fd - " << getClientSocket() << ", curRequestTime=" << curRequestTime << std::endl;
}

std::ostream& operator<<(std::ostream& o, Request& req)
{
    o << " +---------- REQUEST DISPLAY ----------+ " << std::endl;
    o << "Method: " << req.getMethod() << std::endl;
    o << "Path: " << req.getPath() << std::endl;
    o << "Protocol: " << req.getProtocol() << std::endl;
    o << "Query: " << req.getQuery() << std::endl;
    // Only the first 100 characters of the request string are displayed
    // o << "Raw Request: " << req.getRequestString().substr(0, 20) << "..." << std::endl;

    o << "Headers: " << std::endl;
    std::map<std::string, std::string>::iterator it = req.getHeaders().begin();
    while (it != req.getHeaders().end())
    {
        o << "  " << it->first << ": " << it->second << std::endl;
        it++;
    }
    o << " +-------------------------------------+ " << std::endl;

    return o;
}
