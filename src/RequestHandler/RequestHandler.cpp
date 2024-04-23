#include "RequestHandler.hpp"

#include "Server.hpp"
#include "Router.hpp"

// CANONICAL 
RequestHandler::RequestHandler()
{
    std::cout << "RequestHandler default constructor called" << std::endl;
}

RequestHandler::RequestHandler(int clientSocket)
{
    isValidHeaders = false;
    isChunkedRequest = false;
    isRequestFullyRead = false;
    _request = Request();
    _clientSocket = clientSocket;
    std::cout << "RequestHandler constructor called" << std::endl;
    // _request = request;
}

RequestHandler::~RequestHandler()
{
    std::cout << "RequestHandler destructor called" << std::endl;
}

// PARSING METHODS 

bool RequestHandler::parseHeaders(std::string headers)
{
    try {
        _request.parseHeaders(headers);
        isValidHeaders = true;
        isChunkedRequest = _request.isChunked();
        return true;
    } catch (const CustomError &e) {
        // if headers parsing failed, set a response with the error code
        std::cerr << e.what() << std::endl;
        _response = Response(e.getErrorCode(), e.what());
        return false;
    }
}

void RequestHandler::setBody(std::string body)
{
    _request.setBody(body);
}


// HANDLING METHODS

void RequestHandler::buildFinalPath() 
{


    std::cout << " ---------- CURRENT LOCATION VALUES ---------- " << std::endl;
    // std::cout << "Location Name: " << _config->getName() << std::endl;
    // std::cout << "Location Root: " << _config->getRoot() << std::endl;
    // std::cout << "Location Index: " << _config->getIndex() << std::endl;

    // 1. Replace the location name by the root of the location
    // if(path.find(_config->getName()) != std::string::npos && _config->getType() == "std") 
    //     path.replace(path.find(_config->getName()), _config->getName().length(), _config->getRoot());

    // 2. Set the path to the index if the path is a directory
    // if (path == "/") {
    //     if (!_config->getRoot().empty()){
    //         path = path + _config->getRoot() + "/";
    //     }
    //     path = path + _config->getIndex();
    // }

    // 3. If path is a directory without index file , send list of the directory
    // std::string myPath = _request.getPath();
    // DIR *dir = opendir(myPath.c_str());
    // if (dir)
    // {
    //     if(myPath.find("//") != std::string::npos)
    //         myPath.replace(myPath.find("//"), 2, "/");
    //     if(!::fileExists(myPath))
    //     {
    //         if(!_config->isAutoIndex()) {
    //             closedir(dir);
    //             return(_response.setError(403, "Forbidden"));
    //         }
    //         closedir(dir);
    //         return(listDirectoryResponse());
    //     }
    //     closedir(dir);
    // }

    // add . at the beginning of the path
    path =  _location->getRootDirName() + _request.getPath();
    std::cout << "Request path : " << _request.getPath() << std::endl;
    std::cout << "Final path : " << path << std::endl;
}

void RequestHandler::process(std::vector<Server *> servers)
{
    try
    {
        // Root the request 
        _server = routeRequestToServer(&_request, servers);
        _location = routeRequestToLocation(&_request, _server);
        _location->print();

        // build final path
        buildFinalPath();

        // Handle the request
        handleRequest();

        // Set the response headers
        setResponseHeaders();
    }
    catch(const CustomError &e)
    {
        // In case of error, set the response to a standard error response
        _response = Response(e.getErrorCode(), e.what());
    }
}

void RequestHandler::handleRequest() 
{
    // Check that the method is allowed for this location
    if (!_location->isMethodAllowed(_request.getMethod()))
        throw CustomError(405, "Method Not Allowed");

    // Handle redirection
    if(_location->getReturn() != "")
    {
        redirect(_location->getReturn());
        return;
    }

    std::cout << "Path before method routing :" << path << std::endl;

    // Handle listDirectory

    
    // CGI Handling
    // if(true)
    // {
    //     std::cout << "CGI Handling" << std::endl;
    //     _cgiHandler = new CGIHandler(_request);
    //     _cgiHandler->executeCGI();
    //     _response.setBody(_cgiHandler->getOutputCGI());  
    //     _response.setDefaultSuccess();
    //     return;  
    // }

    // Method routing

    std::string method = _request.getMethod();
    if (method == "GET")
    {
        Get();
    }
    else if(method == "POST" || method == "PUT")
    {
        Post();
    }
    else if(method == "DELETE")
    {
        Delete();
    }
    else if (method == "HEAD")
    {
        throw CustomError(405, "Method Not Allowed");
    }
    else 
    {
        throw CustomError(405, "Method Not Implemented");
    }
}

// Function to redirect the client to an other http address
void RequestHandler::redirect(std::string address)
{
    std::cout << "-------- Redirection -------" << std::endl;
    std::string redirectPage = getRedirectionHTML(address);
    _response.setStatusCode("303");
    _response.setStatusText("Other");
    _response.setContentType("text/html");
    _response.setProtocol("HTTP/1.1");
    _response.setBody(redirectPage);
    return;
}

// Function to send an html page which lists the content of a directory (autoindex)
void RequestHandler::listDirectory()
{
    std::cout << "LIST DIRECTORY" << std::endl;

    std::vector<std::string> fileList = getFileList(path);

    std::stringstream ss;

    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html>" << std::endl;
    ss << "<head>" << std::endl;
    ss << "<title>Directory listing</title>" << std::endl;
    ss << "</head>" << std::endl;
    ss << "<body>" << std::endl;
    ss << "<h1>Directory listing</h1>" << std::endl;
    ss << "<ul>" << std::endl;
    while (!fileList.empty())
    {
        std::string url = fileList.back();
        std::cout << "URL : " << url << std::endl;
        if (fileList.back() != ".." && fileList.back() != ".")
            ss << "<li><a href=\"" << url << "\">" << fileList.back() << "</a></li>" << std::endl;
        fileList.pop_back();
    }
    ss << "</ul>" << std::endl;
    ss << "</body>" << std::endl;
    ss << "</html>" << std::endl;

    // todo : only body to set here, the rest is to be set in handleRequest() if success
    _response.setBody(ss.str());
    _response.setDefaultSuccess();
    _response.setContentType("text/html");
}

bool isPathDirectory(std::string path)
{
    struct stat s;
    if (stat(path.c_str(), &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
            return true;
    }
    return false;
}

void RequestHandler::Get() 
{
    std::cout << "HANDLING METHOD GET" << std::endl;
    std::cout << "Requesting ressource at path : " << path << std::endl;

    // Directory handling
    // if path leads to a directory 
    if (isPathDirectory(path))
        return handleGetDirectory(); 

    // Get file content
    getFileContent(path);

}

void RequestHandler::handleGetDirectory()
{
    std::cout << "Handling directory" << std::endl;

    // If the path does not end with a /, redirect to the same path with a /
    if(path.at(path.length() - 1) != '/')
    {
        redirect(_request.getPath() + "/");
        return;
    }

    // If there is an index, try to get the index file
    if(_location->getIndex() != "")
    {
        std::string indexPath = path + _location->getIndex();
        try {
            if(fileExists(indexPath))
            {
                path = indexPath;
                getFileContent(indexPath);
                return;
            }
        }
        catch(const CustomError &e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    // If no index file, and listDirectory is possible, do it
    if(_location->getAutoIndex() == "on" && path.at(path.length() - 1) == '/' )
    {
        listDirectory();
        return;
    }

    // If no valid index file and no directory listing, respond 404
    throw CustomError(404, "Ressource is a directory, but no index and directory listing not allowed");
}

void RequestHandler::getFileContent(std::string filepath)
{
    if(!::fileExists(filepath))
        throw CustomError(404, "File does not exists");
    else if(!::fileIsReadable(filepath))
        throw CustomError(403, "File is not readable");
    else
    {
        _response.loadFileContent(filepath);
        _response.setDefaultSuccess();
    }
}

void RequestHandler::Post() 
{
    std::ifstream my_file(path.c_str());

    std::string body = _request.getBody();
    std::cout << body << std::endl;

    // Check that body is not empty
    if (body.size() == 0)
        throw CustomError(405, "Body size is 0");

    // Check if file exist
    if (!my_file.good())
    {
        std::cout << "File don't exist" << std::endl;
        _response.setProtocol("HTTP/1.1");
        _response.setStatusCode("201");
        _response.setStatusText("Created");
    }
    else {
        std::cout << "File exist" << std::endl;
        // Check that file is writeable 
        if (!fileIsWritable(path))
            throw CustomError(403, "File not writable");
        else 
            _response.setDefaultSuccess();
    }

    // Open file
	std::fstream postFile;
	postFile.open(path.c_str(), std::ios::app);

    // Check if file is open correctly
	if (!postFile.is_open())
        throw CustomError(500, "Failed to open file in post method");

    // Write in file
	postFile << body;

    // Close the file
    postFile.close();  // Close the file after writing
}

void RequestHandler::Delete() 
{
    std::cout << "DELETE" << std::endl;
    std::ifstream my_file(path.c_str());

    if (!my_file.good())
        throw CustomError(404, "File not found");

    std::cout << "Requesting ressource at path : " << path << std::endl;
    if(!remove(path.c_str()))
    {
        std::cout << "File deleted" << std::endl;
        _response.setDefaultSuccess();
    }
    else
        throw CustomError(500, "Error deleting file");
}

void RequestHandler::CGI()
{

}

void RequestHandler::setResponseHeaders()
{
    // if the response has no body, return 
    if(_response.getBody() == "")
        return;

    // If the response content type has already been set, return
    if(_response.getContentType() != "")
        return;
    
    // Determine file name and extension
    std::string extension = _response.getExtension();
    if(extension == "")
        extension = getFileExtension(path);
    std::string filename = path.substr(path.find_last_of("/") + 1);

    // Default values
    std::string contentType = "text/plain";
    std::string contentDisposition = "inline";

    //  Determine content type and disposition depending on extension
    if(extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "gif" || extension == "bmp" || extension == "ico") {
        contentType = "image/" + extension;
    } else if (extension == "pdf") {
        contentType = "application/pdf";
    } else if (extension == "css") {
        contentType = "text/css";
    } else if (extension == "js") {
        contentType = "application/javascript";
    } else if (extension == "txt") {
        contentType = "text/plain";
    } else if (extension == "html") {
        contentType = "text/html";
    } else {
        //  if the extension is not recognized, set to application/octet-stream and attachment (download)
        contentType = "application/octet-stream";
        contentDisposition = "attachment";
    }

    // Build response headers
    _response.setFilename(filename);
    _response.setExtension(extension);
    _response.setContentType(contentType);
    _response.setContentDisposition(contentDisposition);
}


Response &RequestHandler::getResponse()
{
    return _response;
}

std::string RequestHandler::getResponseString()
{
    
    return _response.getSerializedResponse();
}

bool RequestHandler::getIsChunkedRequest()
{
    return isChunkedRequest;
}

bool RequestHandler::getIsValidHeaders()
{
    return isValidHeaders;
}


bool RequestHandler::getIsBodyComplete()
{
    return isBodyComplete;
}

void RequestHandler::setIsBodyComplete(bool value)
{
    isBodyComplete = value;
}