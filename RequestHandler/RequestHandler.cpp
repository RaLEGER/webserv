#include "RequestHandler.hpp"

// CANONICAL 
RequestHandler::RequestHandler()
{
    std::cout << "RequestHandler default constructor called" << std::endl;
}

RequestHandler::RequestHandler(std::string requestString, int clientSocket)
{
    (void) requestString;
    (void) clientSocket;
    std::cout << "RequestHandler constructor called" << std::endl;
    // _request = request;
}

RequestHandler::~RequestHandler()
{
    std::cout << "RequestHandler destructor called" << std::endl;
}


void RequestHandler::getFinalPath() 
{
    // temp : we set this variable by hand, but they will depend on the Server and Location
    std::string root =  "/home/teliet/webserv-2/test";
    std::string index = "index.html";
    std::string type = "none";


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
    // path = "." + path;s
}


void RequestHandler::handleRequest() 
{
    // Handle redirection
    // if(type == "http")
    // {
    //     std::cout << "-------- Redirection -------" << std::endl;
    //     fileContent = getRedirectionHTML(_config->getPath());
    //     _response.setStatusCode("303");
    //     _response.setStatusText("Other");
    //     _response.setContentType("text/html");
    //     _response.setProtocol("HTTP/1.1");
    //     _response.setBody(fileContent);
    //     _response.send();
    //     return;
    // }

    // Check that the method is allowed for this location
    // if ((method == "GET" && !_config->isGetAllowed()) || (method == "POST" && !_config->isPostAllowed()))
    // {    
    //     // _response.setError(405, ": Method not allowed");
    //     return false;
    // }

    // TEMPORARY : we set the path manually
    path = "/mnt/nfs/homes/teliet/dev/new-webserv/test" + _request.getPath();

    std::cout << "Path before method routing :" << path << std::endl;

    // Method routing

    // if (getFileExtension(path) == "php" || getFileExtension(path) == "py") {
    //     if (_config->getType() == "cgi")
    //         this->routingCGI();
    //     else {
    //         _response.setError(405, "Don't match config file");
    //         return;
    //     }
    // }

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
        if (fileList.back() != ".." && fileList.back() != ".")
            ss << "<li><a href=\"" << path << fileList.back() << "\">" << fileList.back() << "</a></li>" << std::endl;
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

void RequestHandler::Get() 
{
    std::cout << "HANDLING METHOD GET" << std::endl;
    std::cout << "Requesting ressource at path : " << path << std::endl;
    if(!::fileExists(path))
        throw CustomError(404, "File does not exists");
    else if(!::fileIsReadable(path))
        throw CustomError(403, "File is not readable");
    else
    {
        _response.loadFileContent(path);
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

void RequestHandler::process()
{

}

void RequestHandler::setResponseHeaders()
{
    // if the response has no body, return 
    if(_response.getBody() == "")
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