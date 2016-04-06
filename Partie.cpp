#include <iostream>
#include "Partie.h"


/*===== CONSTRUCTORS =====*/

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


/*===== TIC TAC TOE LOGIC =====*/

Joueur *Partie::checkWin() {

    Joueur *tmp = NULL;

    // Les 4 façons de gagner (Horizontal, Vertical, Diagonale ->, Diagonale <- )
    bool good_1;
    bool good_2;
    bool good_3;
    bool good_4;

    for (int i = 0; i < taille_plateau; i++) {
        for (int j = 0; j < taille_plateau; j++) {
            tmp = plateau[i][j];
            if (tmp != NULL) {
                good_1 = true;
                good_2 = true;
                good_3 = true;
                good_4 = true;

                for (int k = 0; k < nb_pion_a_aligner; k++) {
                    if (j + k >= taille_plateau || plateau[i][j + k] != tmp) good_1 = false; // Vertical
                    if (i + k >= taille_plateau || plateau[i + k][j] != tmp) good_2 = false; // Horizontal


                    if (i + k >= taille_plateau || j + k < taille_plateau) good_3 = false;
                    if (good_3 && plateau[i + k][j + k] != tmp) good_3 = false;


                    if (j < nb_pion_a_aligner - 1 && i > taille_plateau - nb_pion_a_aligner) good_4 = false;
                    if (good_4 && plateau[i + k][j - k] != tmp) good_4 = false;

                }
                if (good_1 || good_2 || good_3 || good_4) return plateau[i][j];
            }
        }
    }
    return NULL;
}

Reponse *Partie::play(int x, int y, Joueur *whoPlay) {

    if (whoPlay != next_round) return new Reponse(209);

    if (getState(x, y)) return new Reponse(203);

    plateau[x][y] = whoPlay;

    for (unsigned int i = 0; i < joueurs.size(); i++) {
        if (next_round == joueurs.at((unsigned long) i)) {
            next_round = joueurs.at((i + 1) % joueurs.size());
            break;
        }
    }

    Joueur *win = checkWin();

    if (win != NULL) {
        stop = true;
        win->new_win();
    }

    for (unsigned int i = 0; i < this->joueurs.size(); i++) {
        if (win != NULL) {
            this->joueurs.at(i)->sendRes(new Reponse(102, win->getPseudo()));

        } else {
            this->joueurs.at(i)->sendRes(new Reponse(100, "Au suivant !"));
            if (this->joueurs.at(i) == next_round) this->joueurs.at(i)->sendRes(new Reponse(107));
        }

    }

    return NULL;
}


/*===== GAME LIFE CYCLE =====*/

void *Partie::start() {
    char buffer[BUFF_LEN];
    bool is_cancel = true;

    plateau = new Joueur **[taille_plateau];
    for (int i = 0; i < taille_plateau; i++) plateau[i] = new Joueur *[taille_plateau];
    next_round = this->joueurs.at(0);
    next_round->sendRes(new Reponse(107));

    while (!this->stop) {
        read_fds = original_fds;
        // Select doit être parametre avec le numero max du descripteur, +1
        if (select(max_fd, &read_fds, NULL, NULL, &time_now) == -1) {
            perror("Pb select");
            pthread_exit(NULL);
        }

        Joueur *tmp;
        for (unsigned int i = 0; i < joueurs.size(); i++) {
            tmp = joueurs.at(i);
            if (tmp != NULL && FD_ISSET(tmp->getSocket(), &read_fds)) {

                if (recv(joueurs.at(i)->getSocket(), buffer, BUFF_LEN, 0) == -1) {
                    this->abort();
                    FD_CLR(joueurs.at(i)->getSocket(), &original_fds);
                    joueurs.erase(joueurs.begin() + i - 1);
                } else {
                    Reponse *reponse = joueurs.at(i)->negotiate(bufferToString(buffer));
                    if (reponse != NULL) {
                        printf("Thread partie : %d, message : %s \n", this->id, reponse->build().c_str());
                        if (reponse->getStatus_code() == 102) is_cancel = false;
                    }
                }
                memset(buffer, 0, BUFF_LEN);
            }
        }
    }
    for (unsigned int i = 0; i < this->joueurs.size(); i++) {
        this->joueurs.at(i)->deleteCurrent(is_cancel);
        FD_CLR(this->joueurs.at(i)->getSocket(), &original_fds);
        FD_SET(this->joueurs.at(i)->getSocket(), &original_wait_list);
    }
    pthread_exit(NULL);
}


bool Partie::begin() {
    if (this->nb_joueur != (int) joueurs.size()) return false;
    if (this->state == 1) return false;

    this->state = 1;

    for (unsigned int i = 0; i < this->joueurs.size(); i++)
        this->joueurs.at(i)->sendRes(new Reponse(105, boolToString(true)));

    if (pthread_create(&this->thread, NULL, &Partie::start_helper, this) == -1) {
        perror("Thread create ");
        exit(EXIT_FAILURE);
    }
    return true;
}

void Partie::abort() {
    this->stop = true;
}


/*===== GAME UTILS =====*/

Reponse *Partie::addPlayer(Joueur *joueur) {

    if ((unsigned int) this->nb_joueur == this->joueurs.size()) return new Reponse(204);


    if (this->wait_for.size() > 0) {
        bool is_in = false;
        for (unsigned int i = 0; i < wait_for.size() && !is_in; i++)
            if (wait_for.at(i).compare(joueur->getPseudo()) == 0)
                is_in = true;

        if (!is_in) return new Reponse(206);
    }

    joueurs.push_back(joueur);
    FD_SET(joueur->getSocket(), &this->original_fds);

    if (this->nb_joueur == (int) this->joueurs.size()) this->begin();

    return new Reponse(110);

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


/*===== TESTS =====*/

void Partie::testIA() {
    taille_plateau = 5;
    nb_joueur = 2;
    nb_pion_a_aligner = 3;

    plateau = new Joueur **[taille_plateau];
    for (int i = 0; i < taille_plateau; i++) plateau[i] = new Joueur *[taille_plateau];

    Joueur *un = new Joueur(0);

    plateau[0][1] = un;
    plateau[1][1] = un;
    plateau[2][2] = un;
    plateau[3][3] = un;

    Joueur *win = checkWin();

    if (win != NULL) cout << "Le gagnant est : " << win->getSocket() << endl;
    else cout << "Pas de gagnant" << endl;
}





