#include "Router.hpp"

Server *routeRequestToServer(Request* request, std::vector<Server*> servers)
{
    // Loop through the servers and match the request host name 
    // We consider that host:port is already matched by the server farm
    std::vector<Server*>::iterator it;
    for (it = servers.begin(); it < servers.end(); it++)
    {
        if ((*it)->getServerName() == request->getHostname())
        {
            return *it;
        }
    }


    // 2. If no match, return the first server
    return servers[0];
}

Location *routeRequestToLocation(Request* request, std::map <std::string, Location*> Locations)
{
    std::string path = request->getPath();

    // 1. Try to match the exact request uri with the location uri
    if(Locations.count(request->getPath()) == 1)
        return Locations[request->getPath()];

    // 2. If no exact match, return the first location whose uri is a prefix of the request uri
    std::map <std::string, Location*>::iterator it;
    for (it = Locations.begin(); it != Locations.end(); it++)
    {
        if (path.find(it->first) == 0)
        {
            return it->second;
        }
    }

    // 3. If no match, return the first location
    // TODO : use defloc
    return Locations.begin()->second;
}
