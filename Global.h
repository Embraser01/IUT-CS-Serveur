//
// GLOBAL VARIABLES
//

#define BUFF_LEN 100

#define MY_PORT 3490
#define BACKLOG 10

#include <string.h>
#include <vector>
#include <sys/socket.h>
#include "Reponse.h"
#include "Joueur.h"
#include "Partie.h"


/*===== Lists of parties AND players =====*/

class Joueur;

class Partie;

class Reponse;

extern vector<Joueur *> joueurs;
extern vector<Partie *> parties;


/*===== Wait list access =====*/

extern fd_set original_wait_list;
extern fd_set read_wait_list;


/*===== MISC =====*/

extern struct timeval time_now;
extern int max_fd;

/*===== UTILS =====*/

extern std::string boolToString(bool value);

extern std::string *bufferToString(char *buffer);

extern unsigned int split(string txt, vector<string> &strs, char ch);