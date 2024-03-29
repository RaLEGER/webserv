#include "CGIHandler.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <cstring>
#include <signal.h>
#include <fcntl.h>
#include <cerrno>

// Constructor for CGI class, initializes environment variables for CGI execution
CGIHandler::CGIHandler(Request & req) : _req(req)
{
    // Retrieve current working directory
    std::string current_dir = get_current_dir_name();

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

    // // Set up environment variables based on request method
    // if (req.getMethod() == "GET"){
    //     _envvar[i++] = strdup(("QUERY_STRING=" + req.getQuery()).c_str());
    //     std::cout << "Query : " << _envvar[i - 1] << std::endl;
    // }
    // else if (req.getMethod() == "POST"){
    //     _envvar[i++] = strdup(("CONTENT_TYPE=" + getContentInfo(req, "Content-Type: ")).c_str());
    //     _envvar[i++] = strdup(("CONTENT_LENGTH=" + getContentInfo(req, "Content-Length: ")).c_str());
    // }
    _envvar[i++] = NULL;

    // Allocate memory for arguments
    if ((_args = new char*[3]) == NULL)
        throw std::runtime_error("Error on a cgi malloc\n");

    // Set up arguments for executing CGI script
    _args[0] = strdup("/usr/bin/python3"); // Why need to set absolute path here?
    _args[1] = strdup("./test/cgi_scripts/hello_world.py");
    _args[2] = NULL;

    std::cout << "CGIHandler iniated with arguments: " << _args[0] << " " << _args[1] << std::endl;
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
    // Do nothing
}

// Signal handler to handle SIGALRM by terminating the process
void exit_on_alarm(int) {
    std::cerr << "Process terminated due to alarm signal." << std::endl;
    exit(EXIT_FAILURE);
}

// Execute CGI script
bool CGIHandler::executeCGI()
{
    int pipe_in[2];
    int pipe_out[2];

    // Create pipes for communication between parent and child processes
    if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0) {
        perror("pipe failed");
        close (pipe_in[1]);
        close (pipe_in[0]);
        // TODO : Throw an exception if pipe creation fails ? 
        return false;
    }

    // Write request body to pipe for POST method
    // if (req.getMethod() == "POST") {
    //     WebServ::addFd2Select(pipe_in[1]);
    //     if(write(pipe_in[1], req.getBody().c_str(), req.getBody().length()) < 0)
    //         return false;
    //     WebServ::delFd2Select(pipe_in[1]);
    // }

    // Fork a child process for CGI execution
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        close (pipe_in[1]);
        close (pipe_in[0]);
        close (pipe_out[1]);
        close (pipe_out[0]);
        // TODO : Throw an exception if fork fails ?
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

    // Execute CGI script
    int ret = execve(_args[0], _args, _envvar);
    if(ret < 0){
        std::cout << "execve failed with error code: " << ret << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        throw CustomError(500, "execve failed");
    }

    // Terminate child process if execve fails
    // TODO : throw an exception if execve fails ?
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
            perror("waitpid failed");
            // TODO : Throw an exception if waitpid fails ?
        } else if (wait_result == 0) {
            // Child process is still running, continue reading from pipe_out
            bytesRead = read(pipe_out[0], buffer, sizeof(buffer));
            if (bytesRead > 0) {
                outputCGI.append(buffer, bytesRead);
            }
        }
    } while (wait_result == 0);

    // Cancel the alarm
    alarm(0);

    // Remove signal handler for alarm signal
    signal(SIGALRM, SIG_DFL);

    // Remove Content-Type header from CGI output
    // outputCGI = removeContentTypeHeader(outputCGI);

    // Check child process exit status
    if (WIFEXITED(status)) {
        int exitStatus = WEXITSTATUS(status);
        if (exitStatus == 0) {
            std::cout << "CGI execution was successful." << std::endl;
            std::cout << "Output from CGI script: " << std::endl;
            std::cout << outputCGI << std::endl;
            // TODO 
        } else {
            std::cout << "CGI execution failed with exit status: " << exitStatus << std::endl;
            std::cout << "Output from CGI script: " << std::endl;
            std::cout << outputCGI << std::endl;
            // TODO 
        }
    } else {
        std::cout << "CGI execution failed due to an unknown reason." << std::endl;
        std::cout << "Output from CGI script: " << std::endl;
        std::cout << outputCGI << std::endl;
            // TODO 
    }
}