//
// Created by user01 on 22/03/16.
//

#include "Partie.h"


Partie::Partie(int taille_plateau, int nb_joueur, int nb_pion_a_aligner, vector<string> wait_for)
        : taille_plateau(taille_plateau), nb_joueur(nb_joueur), nb_pion_a_aligner(nb_pion_a_aligner),
          wait_for(wait_for) {
    FD_ZERO(&original_fds);
    stop = false;

    parties.push_back(this);
    id = (int) parties.size();
    next_round = NULL;
}


Partie::Partie(int taille_plateau, int nb_joueur, int nb_pion_a_aligner, vector<string> wait_for, Joueur *creator)
        : taille_plateau(taille_plateau), nb_joueur(nb_joueur), nb_pion_a_aligner(nb_pion_a_aligner) {
    FD_ZERO(&original_fds);

    parties.push_back(this);
    id = (int) parties.size();
    stop = false;

    this->addPlayer(creator);
    this->wait_for = wait_for;
    next_round = NULL;
}


Joueur *Partie::checkWin() {

    Joueur *tmp = NULL;
    Joueur *win = NULL;

    for (int i = 0; i < this->taille_plateau - nb_pion_a_aligner + 1 && win == NULL; i++) {
        for (int j = 0; j < taille_plateau - nb_pion_a_aligner + 1 && win == NULL; j++) {
            tmp = plateau[i][j];
            for (int k = 0; k < nb_pion_a_aligner; k++) {
                if (plateau[i][j + k] != tmp) break;
                if (plateau[i + k][j] != tmp) break;
                if (plateau[i + k][j + k] != tmp) break;

                if (i > nb_pion_a_aligner && plateau[i - k][j + k] != tmp) break;
                if (j > nb_pion_a_aligner && plateau[i + k][j - k] != tmp) break;

                win = plateau[i][j];
            }
        }
    }

    return win;
}

Reponse *Partie::play(int x, int y, Joueur *whoPlay) {

    if (whoPlay != next_round) return new Reponse(209);

    if (getState(x, y)) return new Reponse(203);

    plateau[x][y] = whoPlay;

    for (int i = 0; i < joueurs.size(); i++)
        if (next_round == joueurs.at((unsigned long) i))
            next_round = joueurs.at(i + 1 % joueurs.size());

    Joueur *win = checkWin();

    if (win != NULL) return new Reponse(102, win->getPseudo());

    for (int i = 0; this->joueurs.size(); i++){
        this->joueurs.at(i)->sendRes(new Reponse(100, "Au suivant !"));
        if(this->joueurs.at(i) == next_round) this->joueurs.at(i)->sendRes(new Reponse(107));
    }
    return NULL;
}


void *Partie::start() {
    char buffer[BUFF_LEN];

    plateau = new Joueur**[taille_plateau];
    for(int i = 0; i < taille_plateau; i++) plateau[i] = new Joueur*[taille_plateau];


    while (!this->stop) {
        read_fds = original_fds;
        // Select doit être parametre avec le numero max du descripteur, +1
        if (select(max_fd, &read_fds, NULL, NULL, &time_now) == -1) {
            perror("Pb select");
            pthread_exit(NULL);
        }

        Joueur *tmp;
        for (int i = 0; i < joueurs.size(); i++) {
            tmp = joueurs.at(i);
            if (FD_ISSET(tmp->getSocket(), &read_fds)) {

                recv(tmp->getSocket(), buffer, BUFF_LEN, 0);
                printf("Thread partie : %d, message : %s \n", this->id, joueurs.at(i)->negotiate(bufferToString(buffer))->build());
            }
        }
    }
    for (int i = 0; this->joueurs.size(); i++) this->joueurs.at(i)->deleteCurrent();
    pthread_exit(NULL);
}


bool Partie::begin() {
    if (this->nb_joueur != joueurs.size()) return false;
    if(this->state == 1) return false;

    this->state = 1;

    if (pthread_create(&this->thread, NULL, &Partie::start_helper, this) == -1) {
        perror("Thread create ");
        exit(EXIT_FAILURE);
    }
    return true;
}

Reponse *Partie::addPlayer(Joueur *joueur) {

    if (this->nb_joueur == (int) this->joueurs.size()) return new Reponse(204);


    if (this->wait_for.size() > 0) {
        bool is_in = false;
        for (int i = 0; i < wait_for.size() && !is_in; i++)
            if (wait_for.at(i).compare(joueur->getPseudo()) == 0)
                is_in = true;

        if (!is_in) return new Reponse(206);
    }

    joueurs.push_back(joueur);
    FD_SET(joueur->getSocket(), &this->original_fds);

    if (this->nb_joueur == this->joueurs.size()) this->begin();

    return new Reponse(100, "Bien ajouté à la partie");

}

Reponse *Partie::getPlateau() {
    string plateau = "";

    plateau += to_string(this->taille_plateau);

    Joueur *area;

    for (int i = 0; i < taille_plateau; i++) {
        for (int j = 0; j < taille_plateau; j++) {
            plateau += " ";

            area = this->plateau[i][j];

            plateau += area != NULL ? area->getPseudo() : "0";
        }
    }

    return new Reponse(108, plateau);
}

void Partie::abort() {
    this->stop = true;
}






