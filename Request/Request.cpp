#include "Request.hpp"

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
    std::cout << "Request created" << std::endl;
    // _response.setRequest(this);
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
    std::cout << "Request Constructor" << std::endl;
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

    // Check that the first line has the correct number of tokens (3)
    std::string firstLine = requestString.substr(0, requestString.find("\r\n"));
    std::vector<std::string> tokens = splitWithSep(firstLine, ' ');
    if (tokens.size() != 3) {
        // _response.setError(400, ": a field from 1st request line is missing"); 
    }

    // Parse the method, URI, HTTP version and headers
	parseMethodToken(tokens[0]);
    if (!parseURI(tokens[1]) || !parseHTTPVersion(tokens[2]) || !parseHeaders())
        return false;

    // Route the request to the correct virtual server and location 
    // todo : do this in a separate function / other part of the code
    // getRequestConfig();
    // if (_config == NULL)
    // {    
    //     // _response.setError(404, ": virtual server configuration not found");
    //     return false;
    // }

    // Check that the method is allowed for this location
    // if ((method == "GET" && !_config->isGetAllowed()) || (method == "POST" && !_config->isPostAllowed()))
    // {    
    //     // _response.setError(405, ": Method not allowed");
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
    // _response.setError(400, ": unknown method");
}

// This function 
// 1. checks the URI validity
// 2. sets the path and query attributes of the request object
bool Request::parseURI(std::string token)
{
    if (token.size() > MAX_URI_LEN) {
        // _response.setError(414, ": URI too long");
        return false;
    }

	if (token[0] != '/') {
        // _response.setError(400, ": URI must begin with a /");
        return false;
    }
  
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
		// _response.setError(400, ": HTTP version not correct");
        return false;
    }
	protocol = token;
    return true;
}

// Check headers validity, parse and set them
bool Request::parseHeaders()
{
    // if (requestHeaderString.size() > MAX_HEADER_LEN)
    //     _response.setError(431, ": Header too long");

    std::string delimiter = "\r\n";
    size_t pos = 0;
    std::string tmpRequestString = requestString;

    size_t posSC = tmpRequestString.find(":");
    if (posSC == std::string::npos) {
		// _response.setError(400, ": No semicolon");
        return false;
    }
    // check for duplicate headers
    // TODO : useless, make a addHeader function that will check whether a header already exists 
    // if (hasDuplicateKeys(requestHeaderString))
    // {
	// 	_response.setError(400, ": duplicated headers are not allowed");
    //     return false;
    // }

    while ((pos = tmpRequestString.find(delimiter)) != std::string::npos) {
        std::string line = tmpRequestString.substr(0, pos);
        tmpRequestString.erase(0, pos + delimiter.length());
        size_t pos2 = line.find(": ");
        if(pos2 != std::string::npos) {
            std::string key = line.substr(0, pos2);
            std::string value = line.substr(pos2 + 2);
            headers[key] = value;
        }
    }

    //get contentLength if any
    if (headers.count("Content-Length") == 1)
    {
        char*               endPtr      = NULL;
        contentLength = static_cast<unsigned int>(strtoul(headers["Content-Length"].c_str(), &endPtr, 0));
    }
    
    return true;
}

// Check and Parse Body
// bool Request::parseBody()
// {
//     if (chunkedBody)
//         body = concatenateList(requestBodyList);
//     else
//         body = requestBodyString;
    
//     if(body.size() > _config->getClientMaxBodySize()) 
//     {
//         _response.setError(413, ": received more octets than max body size limit");
//         return false;
//     }
//     return true;
// }

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
