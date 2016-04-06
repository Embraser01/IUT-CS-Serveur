#include "Reponse.h"

string Reponse::build() {
    if (data.size() == 0) return to_string(this->status_code);

    return to_string(this->status_code) + " " + data;
}

Reponse::Reponse(int status_code, const string &data) {

    this->status_code = status_code;
    this->data = data;
}

Reponse::Reponse(int status_code) {
    this->status_code = status_code;
    this->data = "";
}




