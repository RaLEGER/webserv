/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 14:46:25 by rleger            #+#    #+#             */
/*   Updated: 2024/01/24 18:39:45 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <stack>

// Function to trim leading and trailing whitespaces from a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// Function to parse the nginx-like configuration file
std::map<std::string, std::map<std::string, std::string> > parseConfig(const std::string& filename) {
    std::ifstream file(filename.c_str());
    std::map<std::string, std::map<std::string, std::string> > config;
    std::map<std::string, std::string> currentSection;
    std::stack<std::string> sectionStack;
	std::cout << "line1" << std::endl;

    if (file.is_open()) {
        std::string line;
		std::string	prev_line;
		
		prev_line = "";
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
		std::cout << "line4" << std::endl;
            size_t bracePos = line.find('{');
            if (bracePos != std::string::npos) {
                // Start of a new section
                std::string sectionName = trim(line.substr(0, bracePos));
		std::cout << sectionName << std::endl;
                currentSection.clear();
                sectionStack.push(sectionName);
            }
		std::cout << "line5" << std::endl;

            // Parse key-value pairs in the current section
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                std::string key = trim(line.substr(0, equalPos));
                std::string value = trim(line.substr(equalPos + 1, line.size() - 1));
                currentSection[key] = value;
            }

            size_t closeBracePos = line.find('}');
            if (closeBracePos != std::string::npos) {
                // End of the current section
                if (!sectionStack.empty()) {
                    std::string sectionName = sectionStack.top();
                    sectionStack.pop();
                    config[sectionName] = currentSection;
                    currentSection.clear();
                }
            }
			prev_line = line;
        }

        file.close();
    }

    return config;
}

int main() {
    std::string filename = "/Users/rleger/Desktop/42off/webserv/default.conf"; // Replace with your actual file name
    std::map<std::string, std::map<std::string, std::string> > config = parseConfig(filename);

    // Print the parsed configuration dictionary
    for (std::map<std::string, std::map<std::string, std::string> >::const_iterator it = config.begin(); it != config.end(); ++it) {
        std::cout << "Section: " << it->first << std::endl;
        for (std::map<std::string, std::string>::const_iterator inner_it = it->second.begin(); inner_it != it->second.end(); ++inner_it) {
            std::cout << "  " << inner_it->first << " = " << inner_it->second << std::endl;
        }
    }

    return 0;
}