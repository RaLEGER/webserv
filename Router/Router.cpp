#include "Router.hpp"

Server *routeRequestToServer(Request* request, std::vector<Server*> servers)
{
    // 1. Loop through the servers and match the request host name and host:port pair
    std::vector<Server*>::iterator it;
    for (it = servers.begin(); it < servers.end(); it++)
    {
        if ((*it)->getServerName() == request->getHost() && (*it)->getPort() == request->getPort())
        {
            return *it;
        }
    }
    return NULL;

Request
    // 2. If no match, return the default server
    // TODO : default server

    //
}

Location *routeRequestToLocation(Request* request, Server* server)
{
    // 1. Loop through the locations and match the exact request uri
    // std::vector<Location*>::iterator it;
    // for (it = server->getLocations().begin(); it < server->getLocations().end(); it++)
    // {
    //     if (request->getUri().find((*it)->getRootDirName()) != std::string::npos)
    //     {
    //         return *it;
    //     }
    // }
    // return NULL;

    // 2. If no match, return the default location
    // TODO : default location
}
