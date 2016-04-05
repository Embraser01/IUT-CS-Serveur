//
// Created by user01 on 22/03/16.
//

#include "Reponse.h"

string Reponse::build() {
    if(data.size() == 0) return this->status_code + "";

    return this->status_code + " " + data;
}

Reponse::Reponse(int status_code, const string &data) {

    this->status_code = status_code;
    this->data = data;
}

Reponse::Reponse(int status_code) {
    this->status_code = status_code;
    this->data = "";
}




