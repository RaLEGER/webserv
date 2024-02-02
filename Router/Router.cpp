// This function determines the correct server and location for this request 
// as set in the config file
void     Request::getRequestConfig()
{
    std::cout << "------- Config Routing ----------" << std::endl;

    std::cout << "request socket fd :" << getServerSocket() << std::endl;

    VirtualServer *server = NULL;

    // STEP 1 : Search matching server
    // check if hostname is an alias
    if (Config::getHostsMap().count(getHeader("Host")))
        server = Config::getHostsMap()[getHeader("Host")];
    else // Search matching socket
    {
        std::vector <VirtualServer*>::iterator    it = Config::getVirtualServers().begin();
        while (it != Config::getVirtualServers().end())
        {
            if ((*it)->getFd() == getServerSocket())
            {
                server = *it; // first by default
                break;
            }
            it++;
        }
    }
    if (server == NULL)
    {
        std::cout << "No matching server found" << std::endl;
        std::cout << "|-------- End of Config Routing ---------|" << std::endl;
        setConfig(NULL);
        return;
    }
    
    // STEP 2 : Search matching location
    setConfig(server->getLocations()[0]); // first by default
    // TODO :  move this into the server class ? Or a router class ?
    if (server->getLocations().size() > 1)
    {
        std::vector <Location*>             locations   = server->getLocations();
        std::vector <Location*>::iterator   locIt       = locations.begin();
        std::vector <Location*>::iterator   locEnd      = locations.end();
        std::string                         path        = getPath();
        std::string                         extension   = getFileExtension(path);
        std::string                         location_path;

        // check if CGI
        bool isCGI = false;
        while(locIt != locEnd)
        {
            if ((*locIt)->getType() == "cgi")
            {
                // std::cout <<  (**locIt) << std::endl;
                if ((*locIt)->getExtension() == extension && (*locIt)->isAllowed(method))
                {
                    setConfig(*locIt);
                    isCGI = true;
                    break;
                }

            }
            locIt++;
        }
        
        // not CGI
        if (!isCGI)
        {
            locIt = locations.begin();
            while(locIt != locEnd)
            {
                location_path = (**locIt).getName();
                if (path.find(location_path) == 0)
                {
                    setConfig(*locIt);
                    break;
                }
                locIt++;
            }
        }
    }

    std::cout << "seleted location for path " << path << std::endl;
    std::cout <<  *_config << std::endl;

    std::cout << "|-------- End of Config Routing ---------|" << std::endl;
}
