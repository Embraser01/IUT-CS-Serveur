//
// Created by user01 on 22/03/16.
//

#ifndef TP_JOUEUR_H
#define TP_JOUEUR_H

#include "Global.h"

using namespace std;

class Partie;

class Joueur {

private:
    string pseudo;

    int score;

    int socket;

    Partie* partie_en_cours;


    /*===== PARSER =====*/

    string* buffer;

    Reponse* new_partie();
    Reponse* abort_partie();
    Reponse* join_partie();
    Reponse* play_partie();
    Reponse* get_grid();
    Reponse* get_state();
    Reponse* is_started();
    Reponse* is_ended();
    Reponse* leave();
    Reponse* get_score();
    Reponse* login();


public:

    Reponse* negotiate(string* req); // Determine l'action à effectuer


    Joueur(int socket);

    const string &getPseudo() const {
        return pseudo;
    }

    Joueur* setPseudo(const string &pseudo) {
        this->pseudo = pseudo;
        return this;
    }

    int getScore() const {
        return score;
    }

    Joueur* setScore(int score) {
        this->score = score;
        return this;
    }

    int getSocket() const {
        return socket;
    }

    Joueur* setSocket(int socket) {
        this->socket = socket;
        return this;
    }

    Partie* getPartie_en_cours() const {
        return partie_en_cours;
    }

    Joueur* setPartie_en_cours(Partie *partie_en_cours) {
        this->partie_en_cours = partie_en_cours;
        return this;
    }
};


#endif //TP_JOUEUR_H