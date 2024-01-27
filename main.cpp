#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Request/Request.hpp"
#include "RequestHandler/RequestHandler.hpp"

int main(int argc, char** argv) {
    (void) argc;
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength;
    char buffer[1024];

    // Get port from command line arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    int port = atoi(argv[1]);

    // Création du socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Erreur lors de la création du socket." << std::endl;
        return 1;
    }

    // Configuration de l'adresse du serveur
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port); // htons : converts bytes from host byte order to network byte order

    // Attachement du socket à l'adresse du serveur
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Erreur lors de l'attachement du socket à l'adresse." << std::endl;
        close(serverSocket);
        return 1;
    }

    // Ecoute des connexions entrantes
    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Erreur lors de l'écoute des connexions entrantes." << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Le serveur est en attente de connexions sur le port " << port << std::endl;

    while (true) {
        // Accepter une nouvelle connexion
        clientAddressLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            std::cerr << "Erreur lors de l'acceptation de la connexion." << std::endl;
            close(serverSocket);
            return 1;
        }

        // Lire la requête du client
        ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead == -1) {
            std::cerr << "Erreur lors de la lecture de la requête du client." << std::endl;
            close(clientSocket);
            close(serverSocket);
            return 1;
        }

        // Vérifier si la méthode est GET (mais il faudra aussi gerer POST et DELETE)
        std::string rawRequest(buffer, bytesRead);
        std::string response;
        std::cout << "----------- RAW REQUEST RECEIVED ---------------- "<< std::endl;
        std::cout  << rawRequest << std::endl;
        std::cout << "---------------------------------------- "<< std::endl;

        Request *request = new Request(rawRequest);
        RequestHandler *requestHandler = new RequestHandler(*request);
        requestHandler->handleRequest();
        std::string rawResponse = requestHandler->getResponse().getSerializedResponse();

        std::cout << "----------- RAW RESPONSE TO SEND ---------------- "<< std::endl;
        std::cout  << rawResponse << std::endl;
        std::cout << "---------------------------------------- "<< std::endl;

        // Envoyer la réponse au client
        ssize_t bytesSent = write(clientSocket, rawResponse.c_str(), rawResponse.length());
        if (bytesSent == -1) {
            std::cerr << "Erreur lors de l'envoi de la réponse au client." << std::endl;
            close(clientSocket);
            close(serverSocket);
            return 1;
        }

        // Fermer la connexion avec le client
        close(clientSocket);
    }

    // Fermer le socket du serveur
    close(serverSocket);

    return 0;
}
