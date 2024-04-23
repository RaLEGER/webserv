/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:45:38 by rleger            #+#    #+#             */
/*   Updated: 2024/04/23 12:23:26 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <list>
#include <stdexcept>
#include <string>

class CustomError : public std::exception {
    public:
        CustomError(int errorCode, const char* message = "")
            : errorCode(errorCode), customMessage(message) {}

        virtual ~CustomError() throw() {}

        virtual const char* what() const throw() {
            return customMessage.empty() ? "Custom exception occurred" : customMessage.c_str();
        }

        int getErrorCode() const {
            return errorCode;
        }

    private:
        int errorCode;
        std::string customMessage;
};

bool fileExists(std::string path);
bool fileIsReadable(std::string path);
bool fileIsWritable(std::string path);
bool fileIsExecutable(std::string path);
std::vector<std::string> getFileList(std::string path);
std::string getRedirectionHTML(std::string url);
std::string getFileExtension(std::string filename);
#endif // UTLS_HPP