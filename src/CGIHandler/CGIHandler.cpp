#include "CGIHandler.hpp"

// Constructor for CGI class, initializes environment variables for CGI execution
CGIHandler::CGIHandler(Request & req, Location location, std::string path) : _req(req)
{
    // Retrieve root directory from configuration
    _root_dir = location.getRootDirName();

    // Allocate memory for environment variables
    if ((_envvar = new char*[10]) == NULL)
        throw CustomError(500, "Malloc failed during environment variable allocation\n");

    // Set up environment variables
    int i = 0;
    _envvar[i++] = strdup("SERVER_PROTOCOL=HTTP/1.1");
    // _envvar[i++] = strdup(("PATH_INFO="+ req.getPath()).c_str());
    // _envvar[i++] = strdup(("SCRIPT_FILENAME=" + current_dir + req.getPath().substr(1)).c_str());
    // _envvar[i++] = strdup(("SCRIPT_NAME=" + req.getPath()).c_str());
    // _envvar[i++] = strdup(("REDIRECT_STATUS=200"));
    // _envvar[i++] = strdup(("DOCUMENT_ROOT=" + current_dir).c_str());
    // _envvar[i++] = strdup("REQUEST_METHOD=" + req.getMethod().c_str());

    // Set up environment variables based on request method
    if (req.getMethod() == "GET"){
        _envvar[i++] = strdup(("QUERY_STRING=" + req.getQuery()).c_str());
        //std::cout << "Query : " << _envvar[i - 1] << std::endl;
    }
    else if (req.getMethod() == "POST"){
        _envvar[i++] = strdup(("CONTENT_TYPE=" + _req.getHeaders()["Content-Type"]).c_str());
        _envvar[i++] = strdup(("CONTENT_LENGTH=" + _req.getHeaders()["Content-Length"]).c_str());
    }
    _envvar[i++] = NULL;

    // Allocate memory for arguments
    if ((_args = new char*[3]) == NULL)
        throw std::runtime_error("Error on a cgi malloc\n");

    // Set up arguments for executing CGI script
    _args[0] = strdup("/usr/bin/python3"); // Why need to set absolute path here?
    _args[1] = strdup(path.c_str());
    _args[2] = NULL;

    //std::cout << "CGIHandler iniated with arguments: " << _args[0] << " " << _args[1] << std::endl;
    //std::cout << "CGIHandler iniated with environment variables: " << std::endl;
}

// Destructor for CGI class
CGIHandler::~CGIHandler()
{
    // Free memory allocated for environment variables
    int i = -1;
    while (_envvar[++i]){
        free(_envvar[i]);
        _envvar[i] = NULL;
    }
    delete[] _envvar;
    
    // Free memory allocated for arguments
    i = 0;
    while (_args[i]){
        free(_args[i]);
        _args[i] = NULL;
        i++;
    }
    delete[] _args;
}

// Signal handler to ignore SIGALRM
void ignore_alarm(int) {
    //std::cout << "Parent : alarm signal, child is terminated." << std::endl;
    // Do nothing
}

// Signal handler to handle SIGALRM by terminating the process
void exit_on_alarm(int) {
    //std::cout << "Child : Process terminated due to timeout alarm signal." << std::endl;
    exit(EXIT_FAILURE);
}

// Execute CGI script
bool CGIHandler::executeCGI()
{
    int pipe_in[2];
    int pipe_out[2];

    // Create pipes for communication between parent and child processes
    if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0) {
        
        close (pipe_in[1]);
        close (pipe_in[0]);
        throw CustomError(500, "CGI pipe creation failed");
        return false;
    }

    // Write request body to pipe for POST method
    if (_req.getMethod() == "POST") {
        if(write(pipe_in[1], _req.getBody().c_str(), _req.getBody().length()) < 0)
            return false;
    }

    // Set alarm for 5 seconds
    alarm(5);

    // Fork a child process for CGI execution
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        close (pipe_in[1]);
        close (pipe_in[0]);
        close (pipe_out[1]);
        close (pipe_out[0]);
        throw CustomError(500, "CGI fork failed");
        return false;
    }
    else if (pid == 0)  // Child process
    {
        childProcess(pipe_out, pipe_in);
    }
    else {  // Parent process
        parentProcess(pipe_out, pipe_in, pid);
    }

    return true;
}

// Child process
void CGIHandler::childProcess(int pipe_out[2], int pipe_in[2])
{
    alarm(5);

    // Install signal handler to exit on alarm signal
    signal(SIGALRM, exit_on_alarm);

    // Redirect stdout to pipe_out
    if(dup2(pipe_out[1], STDOUT_FILENO) < 0){
        throw CustomError(500, "dup2 pipe_out failed");
    }
    close(pipe_out[0]);
    close(pipe_out[1]);

    // Redirect stdin to pipe_in
    if(dup2(pipe_in[0], STDIN_FILENO) < 0){
        throw CustomError(500, "dup2 pipe_in failed");
    }
    close(pipe_in[0]);
    close(pipe_in[1]);

    // Change working directory to root directory
    chdir(_root_dir.c_str());

    // Execute CGI script
    int ret = execve(_args[0], _args, _envvar);
    if(ret < 0){
        //std::cout << "execve failed with error code: " << ret << std::endl;
        throw CustomError(500, "execve failed");
    }

    // Terminate child process if execve fails
    exit(EXIT_FAILURE);
}

//  parent process
void CGIHandler::parentProcess(int pipe_out[2], int pipe_in[2], pid_t pid)
{
    // Install signal handler to ignore alarm signal
    signal(SIGALRM, ignore_alarm);

    // Close unnecessary pipe ends
    close(pipe_out[1]);
    close(pipe_in[1]);
    close(pipe_in[0]);

    int status = 0;

    ssize_t bytesRead;
    char buffer[10];

    // Wait for the child process to exit or data to be available in the pipe_out
    int wait_result;
    do {
        wait_result = waitpid(pid, &status, WNOHANG);
        if (wait_result == -1) {
            // Handle error in waitpid
            throw CustomError(500, "waitpid failed");
        } else if (wait_result == 0) {
            // Child process is still running, continue reading from pipe_out
            bytesRead = read(pipe_out[0], buffer, sizeof(buffer));
            if (bytesRead > 0) {
                outputCGI.append(buffer, bytesRead);
            }
        }
    } while (wait_result == 0);

    // Debugging purposes
    //std::cout << "Output from CGI script: " << std::endl;
    //std::cout << outputCGI << std::endl;

    // Cancel the alarm
    alarm(0);

    // Remove signal handler for alarm signal
    signal(SIGALRM, SIG_DFL);

    // Get Content-Type header from CGI output
    if(_req.getMethod() == "GET")
        parseGetOutput();

    // Check child process exit status
    if (WIFEXITED(status)) {
        int exitStatus = WEXITSTATUS(status);
        if (exitStatus == 0) {
            //std::cout << "CGI execution was successful." << std::endl;
        } else {
            //std::cout << "CGI execution failed with exit status: " << exitStatus << std::endl;
            throw CustomError(500, "CGI execution failed with known exit status");
        }
    } else {
        throw CustomError(500, "CGI execution failed due to an unknown reason.");
    }
}

std::string removeContentTypeHeader(std::string &outputCGI)
{
    size_t pos = outputCGI.find("Content-Type: ");
    if (pos != std::string::npos) {
        size_t endPos = outputCGI.find("\n", pos);
        outputCGI.erase(pos, endPos - pos + 1);
    }
    return outputCGI;
}

void  CGIHandler::parseGetOutput()
{
    size_t pos = outputCGI.find("Content-Type: ");
    if (pos != std::string::npos) {
        size_t endPos = outputCGI.find("\n", pos);
        outputContentType = outputCGI.substr(pos + 14, endPos - pos - 14);
        //std::cout << "Content-type is: " << outputContentType << std::endl;

        // Remove Content-Type header from CGI output
        outputCGI = removeContentTypeHeader(outputCGI);
    }
    else {
        throw CustomError(500, "CGI output does not contain Content-Type header");
    }

}