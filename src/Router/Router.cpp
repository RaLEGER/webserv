#include "Router.hpp"

Server *routeRequestToServer(Request* request, std::vector<Server*> servers)
{

    std::cout << "Routing request to server" << std::endl;
    std::cout << "number of servers : " << servers.size() << std::endl;
    // Loop through the servers and match the request host name 
    // We consider that host:port is already matched by the server farm
    std::vector<Server*>::iterator it;
    for (it = servers.begin(); it < servers.end(); it++)
    {
        std::cout << "Server name: " << (*it)->getServerName() << std::endl;
        if ((*it)->getServerName() == request->getHostname())
        {
            return *it;
        }
    }

    // 2. If no match, return the first server
    return servers[0];
}

Location *routeRequestToLocation(Request* request, Server* server)
{
    std::string path = request->getPath();
    std::map <std::string, Location*> locations = server->getLocations();

    // 1. Try to match the exact request uri with the location uri
    if(locations.count(request->getPath()) == 1)
        return locations[request->getPath()];

    // 2. If no exact match, return the first location whose uri is a prefix of the request uri
    std::map <std::string, Location*>::iterator it;
    for (it = locations.begin(); it != locations.end(); it++)
    {
        if (path.find(it->first) == 0)
        {
            return it->second;
        }
    }

    // 3. If no match, return the first location
    return server->getDefaultLocation();
}
