#ifndef TP_PARTIE_H
#define TP_PARTIE_H

#include "Global.h"

using namespace std;

class Joueur;

class Partie {

private:
    // THREAD UTILS

    static void *start_helper(void *context) { return ((Partie *) context)->start(); }

    void *start();

    // PARTIE ATTRIBUTES

    int id;

    int taille_plateau;
    int state;
    int nb_joueur;
    int nb_pion_a_aligner;

    bool stop;


    // FILE DESCRIPTOR

    fd_set original_fds;
    fd_set read_fds;


    // PARTIE THREAD

    pthread_t thread;


    // LISTS PLAYERS

    vector<Joueur *> joueurs;
    vector<string> wait_for;


    // NEXT PLAYER

    Joueur *next_round;


    // GAME BOARD

    Joueur ***plateau;


public:

    // CONSTRUCTORS

    Partie(int taille_plateau, int nb_joueur, int nb_pion_a_aligner, vector<string> wait_for);

    Partie(int taille_plateau, int nb_joueur, int nb_pion_a_aligner, vector<string> wait_for, Joueur *creator);


    // GETTER AND SETTER

    int getId() const { return id; }

    int getState() const { return state; }

    int getTaille_plateau() const { return taille_plateau; }

    int getNb_joueur() const { return nb_joueur; }

    int getNb_pion_a_aligner() const { return nb_pion_a_aligner; }

    const vector<Joueur *> &getJoueurs() const { return joueurs; }

    Joueur *getNext_round() const { return next_round; }


    // GET STATE OF A CASE

    bool getState(int x, int y) { return this->plateau[x][y] != NULL; }


    // TIC TAC TOE LOGIC

    Joueur *checkWin();

    Reponse *play(int x, int y, Joueur *);


    // GAME UTILS

    Reponse *addPlayer(Joueur *joueur);

    Reponse *getPlateau();


    // LIFE CYCLE

    bool begin();

    void abort();


    // TESTS

    void testIA();


};


#endif //TP_PARTIE_H
