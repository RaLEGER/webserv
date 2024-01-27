#include "RequestHandler.hpp"

// -----------------------------------------------

// TEMPORARY UTILS , TO MOVE INTO A UTILS CLASS


bool fileExists(std::string path) {
    struct stat fileInfo;
    if (stat(path.c_str(), &fileInfo) != 0) {
        // Failed to retrieve file information
        return false;
    }

    return S_ISREG(fileInfo.st_mode);
}

bool fileIsReadable(std::string path) {
    return (access(path.c_str(), R_OK) != -1);
}

bool fileIsWritable(std::string path) {
    return (access(path.c_str(), W_OK) != -1);
}

bool fileIsExecutable(std::string path) {
    return (access(path.c_str(), X_OK) != -1);
}

std::vector<std::string> getFileList(std::string path) {
    DIR             *dir;
    struct dirent   *entry;
    std::vector<std::string> fileList;

    if ((dir = opendir(path.c_str())) == NULL)
        perror("opendir() error"); // todo: throw error
    else {
        while ((entry = readdir(dir)) != NULL)
            fileList.push_back(entry->d_name);
        closedir(dir);
    }
    return fileList;
}

std::string getRedirectionHTML(std::string url)
{
    std::stringstream ss;
    std::string str;

    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html>" << std::endl;
    ss << "<head>" << std::endl;
    ss << "<title>Redirection</title>" << std::endl;
    ss << "<meta http-equiv=\"refresh\" content=\"0; url=" << url << "\" />" << std::endl;
    ss << "</head>" << std::endl;
    ss << "<body>" << std::endl;
    ss << "<p>Redirection vers <a href=\"" << url << "\">" << url << "</a></p>" << std::endl;
    ss << "</body>" << std::endl;
    ss << "</html>" << std::endl;

    str = ss.str();
    return str;
}

std::string getFileExtension(std::string filename)
{
    std::string extension = filename.substr(filename.find_last_of(".") + 1);
    return extension;
}

// -----------------------------------------------


// CANONICAL 
RequestHandler::RequestHandler()
{
    std::cout << "RequestHandler default constructor called" << std::endl;
}

RequestHandler::RequestHandler(Request &request)
{
    std::cout << "RequestHandler constructor called" << std::endl;
    _request = request;
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

    // Todo : understand and reimplement this
    // if(path.find(_config->getName()) != std::string::npos && _config->getType() == "std") {
    //     path.replace(path.find(_config->getName()), _config->getName().length(), _config->getRoot());
    //     if (!::fileExists(path) && _request.getMethod() == "GET")
    //         path = path + "/" + index;
    // }



    // If path is a directory without index file and autoIndex is on
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

    // TEMPORARY : we set the path manually
    path = "/home/teliet/webserv-2/test" + _request.getPath();

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
        Get();
    else if(method == "POST" || method == "PUT")
        Post();
    else if(method == "DELETE")
        Delete();
    else if (method == "HEAD")
    {
        std::cout << "HEAD" << std::endl;
        // _response.setError(405, "Method Not Allowed");
        return;
    }
    else {
        std::cout << "Unknown method" << std::endl;
        // _response.setError(405, "Not Implemented");
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
    _response.setStatusCode("200");
    _response.setProtocol("HTTP/1.1");
    _response.setStatusText("OK");
    _response.setContentType("text/html");
    // _response.send();
}

void RequestHandler::Get() 
{
    std::cout << "HANDLING METHOD GET" << std::endl;
    std::cout << "Requesting ressource at path : " << path << std::endl;
    if(!::fileExists(path))
        _response.setError(404, "Not Found");
    else if(!::fileIsReadable(path))
        _response.setError(403, "Forbidden");
    else
    {
        _response.setProtocol("HTTP/1.1");
        _response.setStatusCode("200");
        _response.setStatusText("OK");
        _response.loadFileContent(path);
    }
}

void RequestHandler::Post() 
{
    std::ifstream my_file(path.c_str());

    //std::cout << body << std::endl;

    // TODO :understand what it does and reimplement it
    // if (body.size() == 0 && readingBody == false)
    // {
    //     std::cout << "Problem with BODY" << path << std::endl;
    //     std::cout << "Method not allowed" << std::endl;
    //     _response.setError(405, "Body size is 0");
    //     return;
    // }
    
    // if (!my_file.good())
    // {
    //     std::cout << "File don't exist" << std::endl;
    //     _response.setStatusCode("201");
    //     _response.setStatusText("Created");
    //     _response.setContentType("text/plain");
    // }
    // else {
    //     std::cout << "File exist" << std::endl;
    //     _response.setStatusCode("200");
    //     _response.setStatusText("OK");
    //     _response.setContentType("text/plain");
    //     _response.setBody(getBody());
    // }

	// std::fstream postFile;
	// postFile.open(path.c_str(), std::ios::app);
	// if (!postFile.is_open())
	// 	std::cout << "failed to open file in post method" << std::endl;
	// postFile << getBody();
    // _response.setProtocol("HTTP/1.1");
    // _response.send();
}

void RequestHandler::Delete() 
{
    std::cout << "DELETE" << std::endl;
    std::ifstream my_file(path.c_str());

    if (!my_file.good())
    {
        std::cout << "File not found" << std::endl;
        _response.setError(404, "Not Found");
    }
    else 
    {
        std::cout << "Requesting ressource at path : " << path << std::endl;
        if(!remove(path.c_str()))
        {
            std::cout << "File deleted" << std::endl;
            _response.setStatusCode("200");
            _response.setStatusText("OK");
            _response.setProtocol("HTTP/1.1");
            // _response.send();
        }
        else
        {
            std::cout << "Error deleting file" << std::endl;
            _response.setError(500, ": Internal Server Error");
        }
    }
}

void RequestHandler::CGI()
{
    // if (method == "GET") {
    //     if(!::fileExists(path)) {
    //         _response.setError(404, "Not Found");
    //         return;
    //     }
    //     else if(!::fileIsReadable(path)) {
    //         _response.setError(403, "Forbidden");
    //         return;
    //     }
    // }
    // if(getFileExtension(path) == "py" || getFileExtension(path) == "php")
    // {
    //     executable_path = _config->getPath();
    //     if (getFileExtension(path) == "py") {
    //         script_path = path.substr(2);
    //     }
    //     else if (getFileExtension(path) == "php") {
    //         script_path = "";
    //     }
    //     CGI cgi(*this);
    //     if (!cgi.executeCGI(*this)) {
    //         _response.setError(500, " : Error in CGI execution");
    //         return;
    //     }
    //     else {
    //         _response.setStatusCode("200");
    //         _response.setStatusText("OK");
    //         _response.setContentType("text/html");
    //         _response.setProtocol("HTTP/1.1");
    //         _response.setBody(cgi.getOutputCGI());
    //         _response.send();
    //         return;
    //     }
    // }
}

// TODO : implement a SuccessResponse vs ErrorResponse
void RequestHandler::buildResponse()
{
    std::string extension = _response.getExtension();
    if(extension == "")
        extension = getFileExtension(path);
    std::string filename = path.substr(path.find_last_of("/") + 1);
    std::string contentType = "text/plain";
    std::string contentDisposition = "inline";

    // Build response
    // _response.setStatusCode("200");
    // _response.setStatusText("OK"); 
    _response.setProtocol("HTTP/1.1");
    _response.setFilename(filename);
    _response.setExtension(extension);

    // if extension is not displayable, download it
    if(extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "gif" || extension == "bmp" || extension == "ico") {
        contentType = "image/" + extension;
        contentDisposition = "attachment";
    } else if (extension == "pdf") {
        contentType = "application/pdf";
        contentDisposition = "attachment";
    } else if (extension == "css") {
        contentType = "text/css";
    } else if (extension == "js") {
        contentType = "application/javascript";
    } else if (extension == "txt") {
        contentType = "text/plain";
    } else if (extension == "html") {
        contentType = "text/html";
    } else {
        contentType = "text/plain";
    }

    _response.setContentType(contentType);
    _response.setContentDisposition(contentDisposition);
    _response.buildHeader();
    _response.buildResponse();
}


Response &RequestHandler::getResponse()
{
    return _response;
}