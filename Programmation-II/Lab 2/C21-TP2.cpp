#include <iostream>
#include <iomanip>
#include <conio.h>
#include <string>
#include <windows.h>

#include <cvm 21.h>


const size_t CLIENTS_MAX = 70;						// nombre maximal de clients
const size_t COMPTES_MAX = 3;						// nombre de comptes par client

const double MARGE_CREDIT_MAX = 10000;				// marge de cr�dit maximun d'un compte du client
const double SOLDE_COMPTE_MAX = 1000000;			// maximun � ne pas d�passer dans un compte d'un client


enum class Commandes { ajouter, afficher, depot, retrait, virement, liste, supprimer, quitter, inconnue };
using Cmd = Commandes;

struct Nom											// nom d'un client
{
	std::string prenom;
	std::string nom;
};

std::ostream& operator<<(std::ostream& os, const Nom& nom)
{
	os << "Nom: " << nom.prenom << ' ' << nom.nom;
	return os;
};

struct Adresse										// adresse d'un client
{
	std::string noCivique;
	std::string rue;
	std::string ville;
	std::string codePostal;
};

std::string stringindent(std::string, std::string, int, int);
std::ostream& operator<<(std::ostream& os, const Adresse& adresse)
{
	os << std::left
		<< stringindent("#", adresse.noCivique, 5, 22) << '\n'
		<< stringindent("Rue", adresse.rue, 5, 22) << '\n'
		<< stringindent("Ville", adresse.ville, 5, 22) << '\n'
		<< stringindent("CP", adresse.codePostal, 5, 22);
	return os;
}

struct Personne										// informations personnelles d'un client
{
	Nom		nom;
	Adresse	adresse;
	std::string	telephone;
	std::string	nas;
};

std::ostream& operator<<(std::ostream& os, const Personne& personne)
{
	os << std::left
		<< personne.adresse << '\n'
		<< stringindent("Tel", personne.telephone, 5, 22) << '\n'
		<< stringindent("NAS", personne.nas, 5, 22);
	return os;
}

struct Compte										// un compte d'un client
{
	double solde;
	double margeCredit;
};

struct Client										// informations relatives � un client
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
/* LES D�CLARATIONS (PROTOTYPES) DE FONCTIONS */
/* ------------------------------------------ */

void io_bip();
void io_clean();
double io_round(double v, size_t p = 2);			// par defaut la pr�cision est � deux d�cimales (p = 2)




/*
	Suggestion de fonctions:

	size_t lireNoClientValide(size_t maxno);
	size_t lireNoCompteValide();
	double lireMontantValide(double maxmontant);
	void afficherComptesClient(Client& client);
*/

// �crire ici toutes vos d�clarations de fonctions ...
// ...



/* ------------------------------------- */
/* FONCTIONS TR�S G�N�RALES DU PROGRAMME */
/* ------------------------------------- */

void io_bip() { Beep(200, 300); }

void io_clean()										// pour vider les 2 tampons
{
	std::cin.clear();									// s'assure que le cin est remis en marche
	std::cin.ignore(std::cin.rdbuf()->in_avail());			// vide le tampon du cin
	while (_kbhit()) (void)_getch();				// vide le tampon de la console
}

double io_round(double v, size_t p)					// valeur et pr�cision
{
	double e = pow(10, p);
	return round(v * e) / e;
}


/* ----------------------------------------------------------------------- */
/* FONCTIONS G�N�RALES POUR L'INTERFACE USAGER (UI) EN LECTURE OU �CRITURE */
/* ----------------------------------------------------------------------- */

std::string stringindent(const std::string name, const std::string value, const int before, const int total)
{
	return std::string(before, ' ') + name + std::string(total - name.size() - before, ' ') + ": " + value;
}

void afficherMenu()
{
	// afficher le menu ...
}

Commandes lireChoixMenu()
{ 
	Commandes cmd = Cmd::quitter;					// commande par d�faut

	// lire la commande et la retourner � main ...

	return cmd;
}

/* ------------------------------------------------------------ */
/* LES FONCTIONS OBLIGATOIRES POUR CHAQUE COMMANDE DU PROGRAMME */
/* ------------------------------------------------------------ */

void cmd_ajouter( /* Param�tre ? indice: doit avoir acc�s � la variable bk de main */ )
{
	// �crire le code ici ...
}

void cmd_afficher( /* Param�tres ? */ )
{
	// �crire le code ici ...
}

void cmd_deposer( /* Param�tres ? */ )
{
	// �crire le code ici ...
}

void cmd_retirer( /* Param�tres ? */ )
{
	// �crire le code ici ...
}

void cmd_virer( /* Param�tres ? */ )
{
	// �crire le code ici ...
}

void cmd_lister( /* Param�tres ? */ )
{
	// �crire le code ici ...
}

void cmd_supprimer( /* Param�tres ? */ )
{
	// �crire le code ici ...
}

void cmd_quitter( /* Param�tres ? */)
{
	// �crire le code ici ...
}

/* ---------------------- */
/* LA FONCTION PRINCIPALE */
/* ---------------------- */

int main()
{
	setcp(1252);
	std::cout << std::fixed << std::setprecision(2);

	Cmd cmd = Cmd::inconnue;

	Banque& b = *new Banque;	// cr�ation dynamique d'une Banque

	Personne pers = { {"***REMOVED***", "***REMOVED***"}, {"Civ", "Rue", "Ville", "Post"}, "Tel", "Ass" };
	std::cout << pers;

	do
	{
		afficherMenu();
		cmd = lireChoixMenu();
		
		switch (cmd)
		{
			case Cmd::ajouter: cmd_ajouter( /* cmd_ajouter doit avoir acc�s � la variable bk de main */ ); break;
			
			//...

			case Cmd::quitter: cmd_quitter(); break;
		}

	} while(cmd != Cmd::quitter);

	delete &b;
}