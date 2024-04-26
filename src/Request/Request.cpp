#include "Request.hpp"

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
    contentLength = -1;
}

/* Constructors & Destructors */
Request::Request()
{
    
}


Request::~Request() {
    std::cout << "Request destroyed" << std::endl;
}

// This function parses the request string and fills the request object
bool Request::parseHeaders(std::string headersString) {

    // lastActivityTime = ft_now();

    // Extract the first line of the request
    std::string firstLine = headersString.substr(0, headersString.find("\r\n"));
    std::vector<std::string> tokens = splitWithSep(firstLine, ' ');
   
    // Check that the first line has the correct number of tokens (3) 
    if (tokens.size() != 3) 
        throw CustomError(400, "Invalid number of tokens in first line of request");

    // Parse the method, URI, HTTP version and headers
    parseMethodToken(tokens[0]);
    parseURI(tokens[1]);
    parseHTTPVersion(tokens[2]);
    parseHeadersList(headersString);

    // Check the existence of the Content-Length header
    if (headers.count("Content-Length") == 0)
        contentLength = -1;
    else 
    {
        contentLength = 0;
        std::stringstream temp(headers["Content-Length"]);
        temp >> contentLength;
    }
        
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
bool Request::parseHeadersList(std::string headersString)
{

    std::cout << "Parsing headers" << std::endl;
    std::cout << "headersString: " << headersString << std::endl;

    // if (requestHeaderString.size() > MAX_HEADER_LEN)
    //     throw CustomError(431, "Header too long");

    std::string delimiter = "\r\n";
    std::string separator = ": ";
    std::string tmpHeadersString = headersString;

    // If the request string does not contain a double line break, there are no headers
    size_t sepPos = tmpHeadersString.find(separator);
    if (sepPos == std::string::npos) 
        throw CustomError(400, "No headers found in request. Host header is required.");

    // Iterate over the request string line by line
    size_t delimiterPos = tmpHeadersString.find(delimiter);
    while (delimiterPos != std::string::npos)
    {
        // extract the current line and remove it from the request string
        std::string line = tmpHeadersString.substr(0, delimiterPos);
        
        tmpHeadersString.erase(0, delimiterPos + delimiter.length());

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
        delimiterPos = tmpHeadersString.find(delimiter);
    }

    //Check that Host header is present
    if (headers.count("Host") == 0)
        throw CustomError(400, "Host header is required");
    hostname = headers["Host"];

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
    if (body.size() > maxBodySize) 
        throw CustomError(413, "Body exceeds size limit");

    // Find the position of the first double line break
    // size_t bodyStartPos = body.find("\r\n\r\n");
    // if (bodyStartPos == std::string::npos) 
    // {
    //     std::cout << "NO DOUBLE LINE BREAKS" << body << std::endl; 
    //     // Double line break not found, body is empty
    //     return true;
    // }

    // Extract the body from the requestString
    // body = requestString.substr(bodyStartPos + 4);

    return true;
}


bool    Request::isChunked()
{
    return headers.count("Transfer-Encoding") && headers["Transfer-Encoding"] == "chunked";

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

int Request::getClientSocket() {
    return clientSocket;
}

int Request::getServerSocket() {
    return serverSocket;
}

std::string Request::getHeader(std::string key) {
    if (headers.count(key) == 1)
        return headers[key];
    return "";
}

std::string Request::getHostname()
{
    return hostname;
}


int Request::getContentLength()
{
    return contentLength;
}

// SETTERS

void    Request::setBody(std::string &body)
{
    this->body = body;
}

void Request::clear(void)
{
    method.clear();
    path.clear();
    originalPath.clear();
    protocol.clear();
    headersString.clear();
    body.clear();
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
