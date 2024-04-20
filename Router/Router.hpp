#include "Server.hpp"
#include "Request.hpp"

Server *routeRequestToServer(Request* request, std::vector<Server*> servers);

Location *routeRequestToLocation(Request* request, std::map <std::string, Location*> Locations);