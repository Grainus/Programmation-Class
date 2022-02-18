/************************************************************************************
Auteur		: Grainus
Description : Version 'idéale' du premier labo, pour comprendre du bon code
			* Il ne s'agit pas nécessairement de la 'meilleure' implémentation,
			* mais d'une fonctionnelle
Note		: Je n'utilise pas namespace std, alors les fonctions standard sont
			* préfixées pas std::
Version		: 1.0
************************************************************************************/

#include <iostream>
#include <string>
#include <conio.h>
#include <cvm 21.h>

// PRAGMA
#pragma warning (disable:6031)		// valeur de retour ignorée '_getch'
#pragma warning (disable:26812)		// type enum 'type-name' est non délimité. Préférez 'enum class' à 'enum' (enum.3)

// DIMENSION DU DAMIER

const size_t LIG = 8, COL = 12;											// Lignes et colonnes du damier

// POSITION DU DAMIER À LA CONSOLE

const size_t START_X = 10;												// x entre le bord de la console et le début du damier
const size_t START_Y = 5;												// y entre le bord de la console et le début du damier

// CARACTÉRISTIQUES DES CASES À LA CONSOLE

const size_t CASE_X = 6;												// Largeur d'une case à l'affichage, 2 minimum
const size_t CASE_Y = 3;												// Hauteur d'une case à l'affichage, 2 minimum

const size_t SPACE_X = 2;												// Nombres de colonnes vides entre les cases, 1 minimum
const size_t SPACE_Y = 1;												// Nombres de lignes vides entre les cases, 1 minimum

const size_t DELTA_X = CASE_X + SPACE_X;								// Saut d'une case à l'autre, sur l'axe des X
const size_t DELTA_Y = CASE_Y + SPACE_Y;								// Saut d'une case à l'autre, sur l'axe des Y

// DIMENSION DE LA FENÊTRE

const size_t WIN_X = 2 * START_X + (COL - 1) * DELTA_X + CASE_X;		// Nombres de colonnes pour l'affichage d'une case
const size_t WIN_Y = 2 * START_Y + (LIG - 1) * DELTA_Y + CASE_Y;		// Nombres de lignes   pour l'affichage d'une case

// COMMANDES

enum class Arrowkeys						// Code ascii décimal des touches fléchées du clavier
{
	up_left = 71,
	up = 72,
	up_right = 73,
	left = 75,
	right = 77,
	down_left = 79,
	down = 80,
	down_right = 81,
};

using Ak = Arrowkeys;						// Alias plus concis

// CASES, DAMIER ET TRANSFORMATIONS

enum Case { CO, CS, CD, CF, CV };			// Les différentes cases possibles: ordinaire, surprise, dollars, fragile, vide

Case damier[LIG][COL] =						// Le damier et ses cases initiales
{
	{ CO, CO, CO, CO, CO, CO, CO, CO, CV, CO, CO, CS },
	{ CO, CO, CV, CV, CO, CO, CO, CO, CO, CV, CO, CV },
	{ CO, CO, CV, CS, CV, CO, CO, CO, CO, CO, CV, CS },
	{ CO, CO, CV, CS, CV, CO, CO, CV, CV, CO, CV, CS },
	{ CS, CO, CV, CV, CV, CS, CV, CO, CV, CO, CV, CO },
	{ CS, CO, CS, CS, CO, CS, CV, CS, CV, CO, CV, CO },
	{ CS, CO, CS, CO, CO, CO, CV, CV, CV, CO, CV, CO },
	{ CS, CS, CO, CO, CO, CO, CO, CO, CO, CO, CO, CO }
};

using Map = Case[LIG][COL];					// Alias pour le tableau de cases

Case futur[5] = { CF, CD, CF, CV, CV };		// La liste des transformations possibles  --  ex: futur[CO] ==> CF

// COOODONNÉE LOGIQUE D'UN DÉPLACEMENT 

struct LC									// Ligne et colonne (l,c) d'une case du damier[l][c]
{
	size_t l, c;							// Ligne: [0..LIG-1] -- colonne: [0..COL-1]
};

struct Move									// Coordonnées des 2 cases impliquées dans un déplacement
{
	LC from, to;
};

// AFFICHAGE DES CASES

struct XY									// Coordonnée graphique (x,y) d'une case dans la console
{
	size_t x, y;
};

struct Style								// Le style pour l'affichage d'une case
{
	Color color; char c;
};

Style map[5] =								// Les style pour toutes les cases
{
	{ Color::blu, '\xB2' },					// ex: map[CO].c == '\xB2'
	{ Color::blu, '\xB2' },
	{ Color::grn, '\x24' },					// ex: map[CD].color == Color::grn 
	{ Color::pur, '\xB0' },
	{ Color::blk, '\x00' }
};

// AFFICHAGE DU CURSEUR

char cursor[3][3] =							// Informations pour l'affichage du curseur
{
	{ '\xC9', '\xCD', '\xBB' },
	{ '\xBA', '\x00', '\xBA' },
	{ '\xC8', '\xCD', '\xBC' }
};

 // PROTOTYPES DES FONCTIONS

size_t s(int i){ return size_t(i); }// Raccourci de conversion
LC ak_to_delta(const Ak);			// Converti un input en LC représentant un delta
LC& LC_plusegal(LC&, const LC&);	// Opérateur += sans overload pour les structures LC
LC LC_plus(LC, const LC&);			// Opérateur +  sans overload pour les structures LC
bool check_move(const Ak, const LC);// Vérifie si un déplacement est valide
int count_moves(const LC);			// Compte le nombre de déplacements disponible, pour déterminer si le jour est coincé
void output_score(const int);		// Affiche le score du joueur lorsqu'il est nécessaire
void output_move(const Move);		// Affiche le déplacement du joueur
void draw_case(LC);					// Affiche une case du jeu
void draw_cursor(LC);				// Affiche le curseur
void draw_initial();				// Affiche le damier en entier pour le début du programme
int move(const Arrowkeys);			// Déplace le curseur et retourne 1 si le score augmente
void endgame(const int, const int, const time_t);	// Écran de fin de partie
void center(const std::string, const int);			// Centrer du texte sur l'écran

 // DÉFINITION DES FONCTIONS

XY LC_to_XY(const LC lc)
{
	return XY({ START_X + (lc.c * DELTA_X) , START_Y + (lc.l * DELTA_Y) });
}

void center(const std::string text, const int yoffset)
{
	gotoxy(WIN_X / 2 - text.size() / 2, yoffset);
	std::cout << text;
}


LC ak_to_delta(const Ak key)
{
	LC delta = {};
	switch (key)
	{
	case Ak::up_left	: delta = {	s(-1) , s(-1) };break;
	case Ak::up			: delta = { s(-1) , s(0) };	break;
	case Ak::up_right	: delta = { s(-1) , s(1) };	break;
	case Ak::left		: delta = { s(0) , s(-1) };	break;
	case Ak::right		: delta = {	s(0) , s(1) };	break;
	case Ak::down_left	: delta = { s(1) , s(-1) };	break;
	case Ak::down		: delta = { s(1) , s(0) };	break;
	case Ak::down_right	: delta = {	s(1) , s(1) };	break;
	}
	return delta;
}

LC& LC_plusegal(LC& left, const LC& right)
{
	left.l += right.l;
	left.c += right.c;
	return left;
}

bool check_move(const Ak key, const LC current)
{
	LC delta = ak_to_delta(key);
	LC presume_pos = LC_plus(current, delta);	//Un nombre négatif fonctionne même si size_t est unsigned
	if (presume_pos.l >= LIG || presume_pos.l < 0 || presume_pos.c >= COL || presume_pos.c < 0)
		return false;
	else if (damier[presume_pos.l][presume_pos.c] == Case::CV)
		return false;
	else
		return true;
}

int count_moves(const LC current)
{
	Ak keys[8] = { Ak::up_left, Ak::up, Ak::up_right, Ak::left, Ak::right, Ak::down_left, Ak::down, Ak::down_right };
	int moves = 0;
	for (int i = 0; i < 8; ++i)
		if (check_move(keys[i], current))
			++moves;
	return moves;
}

// Explication: left est passé par valeur, alors += est effectué sur une copie
LC LC_plus(LC left, const LC& right)
{
	LC_plusegal(left, right);
	return left;
}

int main()
{
	setwsize(WIN_Y, WIN_X);								// Redimensionner la fenêtre de la console
	show(true);											// Afficher (oui/non) le trait d'affichage de la console

	Move m;
	m.from = { 0,0 };									// Coordonnée logique {l,c} du curseur au départ du jeu

	center("D\220COUVREZ ET AMMASEZ 15 CASES $$$$", 2);
	/*
	do {
		uint8_t c = _getch();					// lire le premier code ascii du tampon
		if (c == 0 || c == 224) {				// vérifier s'il s'agit du code réservé. Si oui, il faut lire le code suivant
			if (_kbhit()) {
				c = _getch();
				if (game.check_move(Ak(c)))
					game.move(Ak(c));
			}
		}
		else {
			// ici pour le traitement d'un caractère régulier
		}
	} while (game.count_moves() != 0 && game.score < MAXSCORE);
	game.endgame();*/

}