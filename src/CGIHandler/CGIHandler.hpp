#ifndef CGI_HPP
#define CGI_HPP 

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <cstring>
#include <sstream>
#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <cstdlib>
#include <list>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <cstring>
#include <signal.h>
#include <fcntl.h>
#include <cerrno>

#include "Response.hpp"
#include "Request.hpp"
#include "Location.hpp"

class CGIHandler {
    private:
        char **_envvar;
		char **_args;
        std::string _exec;
		std::string _realUri;
        std::string extension;
        std::string outputCGI;
        std::string outputContentType;
        std::string path;
        std::string _root_dir;
        Request		_req;

        void parentProcess(int pipe_out[2], int pipe_in[2], pid_t pid);
        void childProcess(int pipe_out[2], int pipe_in[2]);

        void parseGetOutput();
        CGIHandler();

    public:
        CGIHandler(Request & req, Location location, std::string path);
		~CGIHandler();
        std::string _req_body;

		bool        executeCGI();


        std::string getOutputCGI() {return outputCGI;};
        std::string getOutputContentType() {return outputContentType;};

};

#endif