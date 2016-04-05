//
// Created by user01 on 22/03/16.
//

#include <cstring>
#include "Joueur.h"


Joueur::Joueur(int socket) {
    this->socket = socket;
}


Reponse *Joueur::negotiate(string *req) {

    this->buffer = req;

    string action = req->substr(0, 5);

    if (action.compare("LOGIN") == 0) {
        return this->login();
    }
    else if (pseudo.compare("") != 0) {
        if (action.compare("NEW__") == 0) {
            return this->new_partie();
        } else if (action.compare("ABORT") == 0) {
            return this->abort_partie();
        } else if (action.compare("JOIN_") == 0) {
            return this->join_partie();
        } else if (action.compare("PLAY_") == 0) {
            return this->play_partie();
        } else if (action.compare("GRID_") == 0) {
            return this->get_grid();
        } else if (action.compare("STATE") == 0) {
            return this->get_state();
        } else if (action.compare("BEGIN") == 0) {
            return this->is_started();
        } else if (action.compare("ENDED") == 0) {
            return this->is_ended();
        } else if (action.compare("LEAVE") == 0) {
            return this->leave();
        } else if (action.compare("SCORE") == 0) {
            return this->get_score();
        }
    }
    return new Reponse(210);
}


Reponse *Joueur::new_partie() {
    vector<string> params;
    int paramNumber = split(*this->buffer, params, ' ');

    //Si le nombre de paramètres est suffisant
    if (paramNumber >= 4) {
        //Si le nombre de joueurs réservés ne dépasse pas le nombre de joueurs total de la partie
        if (stoi(params.at(3)) < paramNumber - 4) {
            int taillePlateau = stoi(params.at(1));
            int pionsAAligner = stoi(params.at(2));
            int nbJoueurs = stoi(params.at(3));
            vector<string> waitFor;

            for (int i = 4; i < paramNumber; i++) {
                waitFor.push_back(params.at(i));
            }


            this->partie_en_cours = new Partie(taillePlateau, nbJoueurs, pionsAAligner, waitFor, this);
        }
    }
    return new Reponse(201);
}


Reponse *Joueur::abort_partie() {
    if (partie_en_cours != NULL) {
        partie_en_cours = NULL;
        vector<Joueur *> joueurs = partie_en_cours->getJoueurs();

        for (int i = 0; i < joueurs.size(); i++) {
            if (joueurs.at(i) != this)
                joueurs.at(i)->leave();
        }

        return new Reponse(106);
    }
    return new Reponse(205);
}

Reponse *Joueur::join_partie() {
    return partie_en_cours->addPlayer(this);
}

Reponse *Joueur::play_partie() {
    vector<string> params;
    int paramNumber = split(*this->buffer, params, ' ');

    if (paramNumber == 3) {
        int taille = partie_en_cours->getTaille_plateau();

//        if ((params.at(1) >= 0) && (params.at(1) < taille) && (params.at(2) >= 0) && (params.at(2) < taille)) {
//            if (partie_en_cours->getPlateau()[params.at(i)][params.at(j)] == null) {
//                partie_en_cours->getPlateau()[params.at(i)][params.at(j)] = this;
//                return new Reponse(100, "Pion joué");
//            }
//            else {
//                return new Reponse(203);
//            }
//        }
//        else {
//            return new Reponse(202);
//        }
    }
    else {
        return new Reponse(210, "Paramètres incorrects");
    }
}

Reponse *Joueur::get_grid() {

    return partie_en_cours->getPlateau();
}

Reponse *Joueur::get_state() {
    vector<string> params;
    int paramNumber = split(*this->buffer, params, ' ');

    if (paramNumber == 3) {
        bool state = partie_en_cours->getState(stoi(params.at(1)), stoi(params.at(2)));

        return new Reponse(104, boolToString(state));
    }

    return new Reponse(210, "Paramètres incorrects");
}

Reponse *Joueur::is_started() {
    if (partie_en_cours != NULL)
        return new Reponse(105, boolToString(partie_en_cours->getState() == 1));

    return new Reponse(205);
}

Reponse *Joueur::is_ended() {
    if (partie_en_cours != NULL)
        return new Reponse(105, boolToString(partie_en_cours->getState() == 2));

    return new Reponse(205);

}

Reponse *Joueur::leave() {
    if (partie_en_cours != NULL) {
        partie_en_cours = NULL;
    }
    else {
        return new Reponse(205);
    }
//
//    int toDelete = 0;
//    for (int i = 0; i < partie_en_cours.joueurs.size()) {
//        if (partie_en_cours.joueurs.at(i) == this)
//            toDelete = i;
//    }
//
//    partie_en_cours.joueurs.erase(i);
//    partie_en_cours.next_round = partie_en_cours.joueurs.at(i);

    return new Reponse(100);
}

Reponse *Joueur::get_score() {
    return new Reponse(103, to_string(score));
}

Reponse *Joueur::login() {
    if (pseudo != "") {
        return new Reponse(200, "Déjà connecté");
    }
    vector<string> params;
    int paramNumber = split(*this->buffer, params, ' ');

    if (paramNumber == 2) {
        bool is_in = false;
        for (int i = 0; i < joueurs.size(); i++) {
            if (joueurs.at(i)->getPseudo().compare(params.at(1)) == 0) is_in = true;
        }
        if (is_in) return new Reponse(207);

        this->pseudo = params.at(1);
        return new Reponse(100, "Vous etes logger sous le nom de " + pseudo);

    }
    return new Reponse(208);

}


