#include <iostream>
#include <iomanip>
#include <conio.h>
#include <string>
#include <windows.h>

#include "../cvm 21.h"

using namespace std;


const size_t CLIENTS_MAX = 70;						// nombre maximal de clients
const size_t COMPTES_MAX = 3;						// nombre de comptes par client

const double MARGE_CREDIT_MAX = 10000;				// marge de crédit maximun d'un compte du client
const double SOLDE_COMPTE_MAX = 1000000;			// maximun à ne pas dépasser dans un compte d'un client


enum class Commandes { ajouter, quitter, inconnue };// À COMPLÉTER AVEC LES AUTRES COMMANDES ...
using Cmd = Commandes;

struct Nom											// nom d'un client
{
	string prenom;
	string nom;
};

struct Adresse										// adresse d'un client
{
	string noCivique;
	string rue;
	string ville;
	string codePostal;
};

struct Personne										// informations personnelles d'un client
{
	Nom		nom;
	Adresse	adresse;
	string	telephone;
	string	nas;
};

struct Compte										// un compte d'un client
{
	double solde;
	double margeCredit;
};

struct Client										// informations relatives à un client
{
	Personne	info;
	Compte		comptes[COMPTES_MAX];
	time_t		date;								// date d'ajout du client
};

struct Banque										// La banque de clients
{
	size_t cpt = 0;									// cpt: compteur de client [0..CLIENTS_MAX]
	Client clients[CLIENTS_MAX];
};


/* ----------------------------------- */
/* LES VARIABLES GLOBALES DU PROGRAMME */
/* ----------------------------------- */

// ATTENTION: les variables globales ne sont pas permises dans ce TP


/* ------------------------------------------ */
/* LES DÉCLARATIONS (PROTOTYPES) DE FONCTIONS */
/* ------------------------------------------ */

void io_bip();
void io_clean();
double io_round(double v, size_t p = 2);			// par defaut la précision est à deux décimales (p = 2)

/*
	Suggestion de fonctions:

	size_t lireNoClientValide(size_t maxno);
	size_t lireNoCompteValide();
	double lireMontantValide(double maxmontant);
	void afficherComptesClient(Client& client);
*/

// écrire ici toutes vos déclarations de fonctions ...
// ...



/* ------------------------------------- */
/* FONCTIONS TRÈS GÉNÉRALES DU PROGRAMME */
/* ------------------------------------- */

void io_bip() { Beep(200, 300); }

void io_clean()										// pour vider les 2 tampons
{
	cin.clear();									// s'assure que le cin est remis en marche
	cin.ignore(cin.rdbuf()->in_avail());			// vide le tampon du cin
	while (_kbhit()) (void)_getch();				// vide le tampon de la console
}

double io_round(double v, size_t p)					// valeur et précision
{
	double e = pow(10, p);
	return round(v * e) / e;
}


/* ----------------------------------------------------------------------- */
/* FONCTIONS GÉNÉRALES POUR L'INTERFACE USAGER (UI) EN LECTURE OU ÉCRITURE */
/* ----------------------------------------------------------------------- */

void afficherMenu()
{
	// afficher le menu ...
}

Commandes lireChoixMenu()
{ 
	Commandes cmd = Cmd::quitter;					// commande par défaut

	// lire la commande et la retourner à main ...

	return cmd;
}

/* ------------------------------------------------------------ */
/* LES FONCTIONS OBLIGATOIRES POUR CHAQUE COMMANDE DU PROGRAMME */
/* ------------------------------------------------------------ */

void cmd_ajouter( /* Paramètre ? indice: doit avoir accès à la variable bk de main */ )
{
	// Écrire le code ici ...
}

void cmd_afficher( /* Paramètres ? */ )
{
	// Écrire le code ici ...
}

void cmd_deposer( /* Paramètres ? */ )
{
	// Écrire le code ici ...
}

void cmd_retirer( /* Paramètres ? */ )
{
	// Écrire le code ici ...
}

void cmd_virer( /* Paramètres ? */ )
{
	// Écrire le code ici ...
}

void cmd_lister( /* Paramètres ? */ )
{
	// Écrire le code ici ...
}

void cmd_supprimer( /* Paramètres ? */ )
{
	// Écrire le code ici ...
}

void cmd_quitter( /* Paramètres ? */)
{
	// Écrire le code ici ...
}

/* ---------------------- */
/* LA FONCTION PRINCIPALE */
/* ---------------------- */

int main()
{
	setcp(1252);
	cout << fixed << setprecision(2);

	Cmd cmd = Cmd::inconnue;

	Banque& b = *new Banque;	// création dynamique d'une Banque

	do
	{
		afficherMenu();
		cmd = lireChoixMenu();
		
		switch (cmd)
		{
			case Cmd::ajouter: cmd_ajouter( /* cmd_ajouter doit avoir accès à la variable bk de main */ ); break;
			
			//...

			case Cmd::quitter: cmd_quitter(); break;
		}

	} while(cmd != Cmd::quitter);

	delete &b;
}