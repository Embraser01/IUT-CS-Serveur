#include <iostream>
#include <netinet/in.h>

#include <unistd.h>
#include <sstream>
#include "Global.h"

using namespace std;


/*===== Variables globales =====*/

bool stop = false;
int server_socket;

fd_set original_login;
fd_set read_login;

vector<Joueur *> joueurs;
vector<Partie *> parties;


/*===== Wait list access =====*/

fd_set original_wait_list;
fd_set read_wait_list;


/*===== MISC =====*/

struct timeval time_now;
int max_fd;


/*===== Fonctions d'aide =====*/

std::string boolToString(bool value){
    return value ? "true" : "false";
}

std::string *bufferToString(char *buffer) {
    std::string *ret = new string(buffer, strlen(buffer));
    return ret;
}

unsigned int split(string txt, vector<string> &strs, char ch) {
    stringstream ss(txt); // Turn the string into a stream.
    string tok;
    strs.clear();

    while(getline(ss, tok, ch)) {
        strs.push_back(tok);
    }

    return (unsigned int) strs.size();
}


/*===== Thread de connexion =====*/

void *new_client(void *data) {

    // Initialisation
    unsigned int sin_size = sizeof(struct sockaddr);
    struct sockaddr_in client_address;
    int tmp_socket;

    Joueur *tmp_joueur;

    while (!stop) {

        if ((tmp_socket = accept(server_socket, (struct sockaddr *) &client_address, &sin_size)) == -1) {
            perror("ERROR --> Thread connexion ");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        printf("Thread new client : %d \n", tmp_socket);

        tmp_joueur = new Joueur(tmp_socket);
        joueurs.push_back(tmp_joueur);

        if (max_fd <= tmp_socket) max_fd = tmp_socket + 1;

        FD_SET(tmp_socket, &original_login);
    }
    pthread_exit(NULL);
}


/*===== Thread de login =====*/

void *login(void *data) {

    // Initialisation
    FD_ZERO(&original_login);
    FD_ZERO(&read_login);
    char buffer[BUFF_LEN];


    while (!stop) {
        read_login = original_login;

        if (select(max_fd, &read_login, NULL, NULL, &time_now) == -1) {
            perror("ERROR --> Thread login ");
            pthread_exit(NULL);
        }

        for (unsigned int i = 0; i < joueurs.size(); i++) {
            if (FD_ISSET(joueurs.at(i)->getSocket(), &read_login)) {

                if(recv(joueurs.at(i)->getSocket(), buffer, BUFF_LEN, 0) == -1){
                    FD_CLR(joueurs.at(i)->getSocket(), &original_login);
                    joueurs.erase(joueurs.begin() + i - 1);
                } else {
                    Reponse *reponse = joueurs.at(i)->negotiate(bufferToString(buffer));
                    if (reponse != NULL) printf("Thread login : %d, message : %s \n", i, reponse->build().c_str());

                    // Move to the wait_list thread
                    if (joueurs.at(i)->getPseudo().compare("") != 0) {
                        FD_CLR(joueurs.at(i)->getSocket(), &original_login);
                        FD_SET(joueurs.at(i)->getSocket(), &original_wait_list);
                    }
                }
                memset(buffer, 0, BUFF_LEN);
            }
        }
    }
    pthread_exit(NULL);
}


/*===== Thread d'attente =====*/

void *wait_list(void *data) {

    // Initialisation
    FD_ZERO(&original_wait_list);
    FD_ZERO(&read_wait_list);
    char buffer[BUFF_LEN];


    while (!stop) {
        read_wait_list = original_wait_list;

        if (select(max_fd, &read_wait_list, NULL, NULL, &time_now) == -1) {
            perror("ERROR --> Thread wait_list ");
            pthread_exit(NULL);
        }

        for (unsigned int i = 0; i < joueurs.size(); i++) {
            if (FD_ISSET(joueurs.at(i)->getSocket(), &read_wait_list)) {

                if(recv(joueurs.at(i)->getSocket(), buffer, BUFF_LEN, 0) == -1){
                    FD_CLR(joueurs.at(i)->getSocket(), &original_wait_list);
                    joueurs.erase(joueurs.begin() + i - 1);
                } else {
                    Reponse *reponse = joueurs.at(i)->negotiate(bufferToString(buffer));
                    if (reponse != NULL) printf("Thread wait_list : %d, message %s\n", i, reponse->build().c_str());

                    if (joueurs.at(i)->getPartie_en_cours() != NULL)
                        FD_CLR(joueurs.at(i)->getSocket(), &original_wait_list);
                }
                memset(buffer, 0, BUFF_LEN);
            }
        }
    }
    pthread_exit(NULL);
}


int main() {

    // Initialisation

    struct sockaddr_in server_address;
    int yes = 1;


    // Main threads

    pthread_t connection_t;
    pthread_t login_t;
    pthread_t wait_list_t;

    // Time out & max socket

    time_now.tv_sec = 2;
    time_now.tv_usec = 0;
    max_fd = 0;

    // TESTS

    vector<string> tmp;

    Partie* test1 = new Partie(3,2,3, tmp);
    test1->testIA();

    return 0;

    // Point de connexion

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Creation socket : ");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("Options socket : ");
        exit(EXIT_FAILURE);
    }


    // Configuration de l'adresse de transport

    server_address.sin_family = AF_INET;         // Type de la socket
    server_address.sin_port = htons(MY_PORT);     // Port, converti en reseau
    server_address.sin_addr.s_addr = INADDR_ANY; // Adresse, devrait être converti en reseau mais est egal à 0
    bzero(&(server_address.sin_zero), 8);        // Mise a zero du reste de la structure


    // Creation du point de connexion : on ajoute l'adresse de transport dans la socket

    if ((bind(server_socket, (struct sockaddr *) &server_address, sizeof(struct sockaddr))) == -1) {
        perror("Creation point connexion");
        exit(EXIT_FAILURE);
    }


    // Attente sur le point de connexion

    if (listen(server_socket, BACKLOG) == -1) {
        perror("Listen socket : ");
        exit(EXIT_FAILURE);
    }


    // Lancement des differents threads
    string error = "";

    if (pthread_create(&connection_t, NULL, new_client, NULL) == -1) error += "ERROR --> Thread create connexion ";
    if (pthread_create(&login_t, NULL, login, NULL) == -1) error +=  "ERROR --> Thread create login ";
    if (pthread_create(&wait_list_t, NULL, wait_list, NULL) == -1) error += "ERROR --> Thread create wait_list ";

    if (pthread_join((pthread_t) connection_t, NULL) == -1) error += "Thread join ";
    if (pthread_join((pthread_t) login_t, NULL) == -1) error += "Thread join ";
    if (pthread_join((pthread_t) wait_list_t, NULL) == -1) error += "Thread join ";


    if(error.compare("") != 0){
        perror(error.c_str());
        return EXIT_FAILURE;
    }
    return 0;
}