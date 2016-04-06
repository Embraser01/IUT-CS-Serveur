#ifndef TP_JOUEUR_H
#define TP_JOUEUR_H

#include "Global.h"

using namespace std;

class Partie;

class Joueur {

private:

    // PLAYER ATTRIBUTES

    string pseudo;
    int score;
    int socket;

    Partie *partie_en_cours;


    // PARSER

    string *buffer;

    Reponse *new_partie();

    Reponse *abort_partie();

    Reponse *join_partie();

    Reponse *play_partie();

    Reponse *get_grid();

    Reponse *get_state();

    Reponse *is_started();

    Reponse *is_ended();

    Reponse *get_score();

    Reponse *login();


public:

    // CONSTRUCTOR

    Joueur(int socket);


    // PARSER (PUBLIC)

    Reponse *leave();

    Reponse *negotiate(string *req); // Determine l'action à effectuer


    // GETTER AND SETTER

    const string &getPseudo() const { return pseudo; }

    int getScore() const { return score; }

    int getSocket() const { return socket; }

    Partie *getPartie_en_cours() const { return partie_en_cours; }


    // DELETE CURRENT GAME OF THE PLAYER

    void deleteCurrent(bool is_cancel);


    // SEND REPONSE TO THE SOCKET BEFORE RETURNING DATA

    Reponse *sendRes(Reponse *reponse);
};


#endif //TP_JOUEUR_H