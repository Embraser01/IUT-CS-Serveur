#include <cstring>
#include "Joueur.h"


/*===== CONSTRUCTOR =====*/

Joueur::Joueur(int socket) {
    this->socket = socket;
}


/*===== PARSER =====*/

Reponse *Joueur::negotiate(string *req) {

    this->buffer = req;

    string action = req->substr(0, 5);


    if (action.compare("LOGIN") == 0) return sendRes(this->login());
    if (this->pseudo.compare("") == 0) return sendRes(new Reponse(200, "Vous devez vous connecter pour acceder ici"));

    if (action.compare("SCORE") == 0) return sendRes(this->get_score());

    if (action.compare("NEW__") == 0) return sendRes(this->new_partie());
    if (action.compare("JOIN_") == 0) return sendRes(this->join_partie());

    if (partie_en_cours == NULL) return sendRes(new Reponse(200, "Vous n avez pas de partie en cours"));

    if (action.compare("ABORT") == 0) return sendRes(this->abort_partie());
    if (action.compare("PLAY_") == 0) return sendRes(this->play_partie());
    if (action.compare("GRID_") == 0) return sendRes(this->get_grid());
    if (action.compare("STATE") == 0) return sendRes(this->get_state());
    if (action.compare("BEGIN") == 0) return sendRes(this->is_started());
    if (action.compare("ENDED") == 0) return sendRes(this->is_ended());
    if (action.compare("LEAVE") == 0) return sendRes(this->leave());


    return sendRes(new Reponse(210));
}

Reponse *Joueur::new_partie() {
    vector<string> params;
    int paramNumber = split(*this->buffer, params, ' ');

    //Si le nombre de paramètres est suffisant
    if (paramNumber >= 4) {

        int taillePlateau = stoi(params.at(1));
        int pionsAAligner = stoi(params.at(2));
        int nbJoueurs = stoi(params.at(3));
        vector<string> waitFor;

        //Si le nombre de joueurs réservés ne dépasse pas le nombre de joueurs total de la partie
        if (paramNumber > 4 && stoi(params.at(3)) < paramNumber - 4) {
            for (int i = 4; i < paramNumber; i++) waitFor.push_back(params.at(i));
        }

        this->partie_en_cours = new Partie(taillePlateau, nbJoueurs, pionsAAligner, waitFor, this);
        return new Reponse(101, to_string(this->partie_en_cours->getId()));
    }
    return new Reponse(201);
}

Reponse *Joueur::abort_partie() {
    partie_en_cours->abort();
    return NULL;
}

Reponse *Joueur::join_partie() {
    vector<string> params;
    int paramNumber = split(*this->buffer, params, ' ');

    if (paramNumber != 2) return new Reponse(210);


    unsigned int id_partie = (unsigned int) stoi(params.at(1));
    if (id_partie > parties.size() && id_partie > 0) return new Reponse(205);

    partie_en_cours = parties.at(id_partie - 1);

    return partie_en_cours->addPlayer(this);
}

Reponse *Joueur::play_partie() {
    vector<string> params;
    int paramNumber = split(*this->buffer, params, ' ');

    if (paramNumber == 3) {
        int taille = partie_en_cours->getTaille_plateau();
        int x = stoi(params.at(1));
        int y = stoi(params.at(2));

        if (x < 0 || x >= taille || y < 0 || y >= taille) return new Reponse(202);

        return partie_en_cours->play(x, y, this);
    }
    return new Reponse(210, "Paramètres incorrects");
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
    return this->abort_partie(); // TODO Add more flexibility
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
        for (unsigned int i = 0; i < joueurs.size(); i++) {
            if (joueurs.at(i)->getPseudo().compare(params.at(1)) == 0) is_in = true;
        }
        if (is_in) return new Reponse(207);

        this->pseudo = params.at(1);
        return new Reponse(109, "Vous etes logger sous le nom de " + pseudo);

    }
    return new Reponse(208);

}


/*===== DELETE GAME =====*/

void Joueur::deleteCurrent(bool is_cancel) {
    this->partie_en_cours = NULL;
    if(is_cancel) sendRes(new Reponse(106));
}


/*==== SEND RESPONSE =====*/

Reponse *Joueur::sendRes(Reponse *reponse) {
    if (reponse == NULL) return NULL;

    string msg = reponse->build();

    send(socket, msg.c_str(), msg.size(), 0);

    return reponse;
}




