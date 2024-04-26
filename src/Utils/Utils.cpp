/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teliet <teliet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:46:40 by rleger            #+#    #+#             */
/*   Updated: 2024/04/26 17:55:56 by teliet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Utils.hpp"
// Check if a file exists
bool fileExists(std::string path) {
    struct stat fileInfo;
    if (stat(path.c_str(), &fileInfo) != 0) {
        // Failed to retrieve file information
        return false;
    }
    return S_ISREG(fileInfo.st_mode);
}

// Check if a file is readable
bool fileIsReadable(std::string path) {
    return (access(path.c_str(), R_OK) != -1);
}

// Check if a file is writable
bool fileIsWritable(std::string path) {
    return (access(path.c_str(), W_OK) != -1);
}

// Check if a file is executable
bool fileIsExecutable(std::string path) {
    return (access(path.c_str(), X_OK) != -1);
}

// Get a list of files in a directory
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

// Generate HTML code for redirection to a given URL
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

// Get the file extension from a filename
std::string getFileExtension(std::string filename)
{
    std::string extension = filename.substr(filename.find_last_of(".") + 1);
    return extension;
}
