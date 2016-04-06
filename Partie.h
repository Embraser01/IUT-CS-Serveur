//
// Created by user01 on 22/03/16.
//

#ifndef TP_PARTIE_H
#define TP_PARTIE_H

#include "Global.h"

using namespace std;

class Joueur;

class Partie {

private:
    // Helper to start pthread with a class
    static void *start_helper(void *context) {
        return ((Partie *) context)->start();
    }

    int id;

    int taille_plateau;
    int state;
    int nb_joueur;
    int nb_pion_a_aligner;

    bool stop;


    fd_set original_fds;
    fd_set read_fds;

    pthread_t thread;
    timeval time_now;

    vector<Joueur *> joueurs;
    vector<string> wait_for;

    Joueur *next_round;

    Joueur ***plateau;

    void *start();

public:

    Partie(int taille_plateau, int nb_joueur, int nb_pion_a_aligner, vector<string> wait_for);

    Partie(int taille_plateau, int nb_joueur, int nb_pion_a_aligner, vector<string> wait_for, Joueur *creator);

    int getId() const {
        return id;
    }
this->joueurs.at(i)->sendRes(new Reponse(100, "Au suivant !"));
    int getState() const {
        return state;
    }

    int getTaille_plateau() const {
        return taille_plateau;
    }

    int getNb_joueur() const {
        return nb_joueur;
    }

    int getNb_pion_a_aligner() const {
        return nb_pion_a_aligner;
    }

    const vector<Joueur *> &getJoueurs() const {
        return joueurs;
    }

    Joueur *getNext_round() const {
        return next_round;
    }

    bool getState(int x, int y) { return this->plateau[x][y] != NULL; }


    Joueur *checkWin();

    Reponse *play(int x, int y, Joueur *);

    bool begin();

    Reponse *addPlayer(Joueur *joueur);

    Reponse *getPlateau();


    void abort();
};


#endif //TP_PARTIE_H
