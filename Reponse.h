#ifndef TP_REPONSE_H
#define TP_REPONSE_H

#include <string>

using namespace std;

class Reponse {

private:
    int status_code;
    string data;


public:
    Reponse(int status_code, const string &data);

    Reponse(int status_code);


    int getStatus_code() const {
        return status_code;
    }

    Reponse *setStatus_code(int status_code) {
        Reponse::status_code = status_code;
        return this;
    }

    const string &getData() const {
        return data;
    }

    Reponse *setData(const string &data) {
        Reponse::data = data;
        return this;
    }

    string build();
};


#endif //TP_REPONSE_H
