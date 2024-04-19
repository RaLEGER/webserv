
#include "Response.hpp"


Response::Response()
{
    // std::cout << "Response default constructor" << std::endl;
    // Initialize on success 200 OK
    setProtocol("HTTP/1.1");
    setStatusCode("200");
    setStatusText("OK");
    setContentType("");
}

Response::Response(int statusCode, std::string error_msg)
{
    // std::cout << "Error Response constructor with status code" << std::endl;
    setError(statusCode, error_msg);
}

Response::~Response()
{
    // std::cout << "Response destroyed" << std::endl;
}

/* Getters & Setters */

std::string Response::getResponse(){return response;}
std::string Response::getHeader(){return header;}
std::string Response::getBody(){return body;}
std::string Response::getProtocol(){return protocol;}
std::string Response::getStatusCode(){return statusCode;}
std::string Response::getStatusText(){return statusText;}
std::string Response::getContentType(){return contentType;}
std::string Response::getFilename(){return filename;}
std::string Response::getExtension(){return extension;}
std::string Response::getContentDisposition(){return contentDisposition;}

void Response::setProtocol(std::string protocol){this->protocol = protocol;}
void Response::setStatusCode(std::string statusCode){this->statusCode = statusCode;}
void Response::setStatusText(std::string statusText){this->statusText = statusText;}
void Response::setContentType(std::string contentType){this->contentType = contentType;}
void Response::setFilename(std::string filename){this->filename = filename;}
void Response::setExtension(std::string extension){this->extension = extension;}
void Response::setContentDisposition(std::string contentDisposition){this->contentDisposition = contentDisposition;}
void Response::setContentLength(std::string contentLength){this->contentLength = contentLength;}
void Response::setBody(std::string content){this->body = content;}
void Response::setRequest(Request *request){this->request = request;}

/* Methods */

void Response::buildHeader()
{
    std::stringstream ss;
    ss << body.length();
    std::string contentLength;
    ss >> contentLength;

    // todo : content disposition, filename, extension only for GET
    header = protocol + " " + statusCode + " " + statusText + "\r\n";
    if(!contentDisposition.empty())
        header += "Content-Disposition: " + contentDisposition + "\r\n";
    if(!contentType.empty())
        header += "Content-Type: " + contentType + "\r\n";
    if(!contentLength.empty())
        header += "Content-Length: " + contentLength + "\r\n";
    header += "\r\n";
}

std::string Response::getSerializedResponse()
{
    buildHeader();

    return header + body;
}

// TODO : move this in RequestHandler ? 
// This function reads a file and stores it in the body
void Response::loadFileContent(const std::string& path)
{
    std::cout << "Sending file " << path << std::endl;
    std::ifstream       file(path.c_str());
    // check file status
    if (!file.good())
    {
        setError(500, ": Error when opening file " + path);
        return;
    }

    std::string         fileContent;
    std::stringstream   buffer;
    buffer << file.rdbuf();
    fileContent = buffer.str();
    file.close();
    
    setBody(fileContent);  
}

void Response::setError(int statusCode, std::string error_msg)
{
    std::cout <<"----- ERROR ------" << std::endl;
    std::cout << "(⊙…⊙ ) " << statusCode << " " << error_msg << std::endl;
    std::cout <<"------------------" << std::endl;
    
    // check if custom error page exists
    // if (request && request->getConfig()
    //     && request->getConfig()->getErrorPages().count(statusCode)
    //     && fileExists(request->getConfig()->getErrorPages()[statusCode]))
    // {
    //     errorPagePath = request->getConfig()->getErrorPages()[statusCode];
    // }

    std::stringstream ss;
    ss << statusCode;
    std::string errorPagePath = "./data/default/" + ss.str() + ".html";

    setProtocol("HTTP/1.1");
    setContentType("text/html");
    setExtension("html");
    setContentDisposition("inline");

    setStatusCode(ss.str());
    setFilename(ss.str() + ".html");
    loadFileContent(errorPagePath);

    switch(statusCode)
    {
        case 301:
            setStatusText("Moved Permanently");
            break;
        case 400:
            setStatusText("Bad Request");
            break;
        case 403:
            setStatusText("Forbidden");
            break;
        case 404: 
            setStatusText("Not Found");
            break;
        case 405:
            setStatusText("Method Not Allowed"); // TODO : add Allow header
            break;
        case 408:
            setStatusText("Request Timeout");
            break;
        case 413:
            setStatusText("Content Too Large");
            break;
        case 414:
            setStatusText("URI Too Long");
            break;
        case 431:
            setStatusText("Request Header Fields Too Large");
            break;
        case 500:
            setStatusText("Internal Server Error");
            break;
        case 501:
            setStatusText("Method Not Implemented");
            break;
        case 505:
            setStatusText("HTTP Version Not Supported");
            break;
        default: 
            std::cout << "ERROR: unknown status code, sent status 500" << std::endl;
            statusCode = 500;
            setStatusText("Internal Server Error");
            errorPagePath = "./data/default/500.html";
    }

}

void Response::setDefaultSuccess()
{
    setProtocol("HTTP/1.1");
    setStatusCode("200");
    setStatusText("OK");
}

void Response::clear(void)
{
    protocol.clear();
    statusCode.clear();
    statusText.clear();
    contentType.clear();
    contentLength.clear();
    filename.clear();
    extension.clear();
    contentDisposition.clear();
    header.clear();
    body.clear();
    response.clear();
}
