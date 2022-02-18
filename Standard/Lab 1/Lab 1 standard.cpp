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
bool cursor_move(const Ak, Move&);	// Déplace le curseur, update le damier, et return si le score doit être augmenté
void endgame(const int, const int, const time_t);	// Écran de fin de partie
void center(const std::string, const int);			// Centrer du texte sur l'écran

 // FONCTIONS BASIQUES

XY LC_to_XY(const LC lc)
{
	return XY({ START_X + (lc.c * DELTA_X) , START_Y + (lc.l * DELTA_Y) });
}

LC& LC_plusegal(LC& left, const LC& right)
{
	left.l += right.l;
	left.c += right.c;
	return left;
}

// Explication: left est passé par valeur, alors += est effectué sur une copie
LC LC_plus(LC left, const LC& right)
{
	LC_plusegal(left, right);
	return left;
}

// Overload de gotoxy pour accepter une valeur de type XY
void gotoxy(XY xy)
{
	gotoxy(xy.x, xy.y);
	return;
}

 // DÉFINITION DES FONCTIONS


void center(const std::string text, const int yoffset)
{
	gotoxy(WIN_X / 2 - text.size() / 2, yoffset);
	std::cout << text;
	return;
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

void output_score(const int score)
{
	XY score_ouput = LC_to_XY(LC({ LIG, COL }));
	score_ouput.x -= SPACE_X + 7 + (score >= 10 ? 1 : 0);	//Déplacement à gauche dynamique

	gotoxy(score_ouput);
	setcolor(Color::grn);
	std::cout << "$$$$ : " << score;
}

void output_move(const Move move)
{
	gotoxy(LC_to_XY(LC({ LIG, 0 })));
	setcolor(Color::yel);
	std::cout << "move: (" << move.from.l << ',' << move.from.c << ") --> (" << move.to.l << ',' << move.to.c << ')';
	return;
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

void draw_case(const LC coords)
{
	XY consolepos = LC_to_XY(coords);
	gotoxy(consolepos);

	Case type = damier[coords.l][coords.c];
	setcolor(map[type].color);
	std::string line(CASE_X, map[type].c);

	for (int lin = 0; lin < CASE_Y; ++lin) {
		gotoxy(consolepos.x, consolepos.y++);
		std::cout << line;
	}

	setcolor(Color::wht);
	return;
}

void draw_cursor(const LC coords)
{
	XY consolepos = LC_to_XY(coords);
	gotoxy(consolepos);
	setcolor(Color::yel);
	std::cout << cursor[0][0]						//Top left
		<< std::string(CASE_X - 2, cursor[0][1])	//Top middle
		<< cursor[0][2];							//Top right

	gotoxy(consolepos.x, consolepos.y + 1);
	for (int y = 1; y < CASE_Y; ++y) {
		std::cout << cursor[1][0]					//Middle left
			<< std::string(CASE_X - 2, cursor[1][1])//Middle center
			<< cursor[1][2];
		gotoxy(consolepos.x, consolepos.y + y);
	}

	std::cout << cursor[2][0]						//Bottom left
		<< std::string(CASE_X - 2, cursor[2][1])	//Bottom middle
		<< cursor[2][2];							//Bottom right

	setcolor(Color::wht);
	return;
}

void draw_initial()
{
	for (size_t y = 0; y < COL; ++y)
		for (size_t x = 0; x < LIG; ++x)
			draw_case(LC({ x, y }));
	output_score(0);
	return;
}

bool cursor_move(const Ak key, Move& move)
{
	LC delta = ak_to_delta(key);
	LC_plusegal(move.to, delta);
	Case& state = damier[move.to.l][move.to.c];
	bool addscore = (state == CD);
	state = futur[state];
	draw_case(move.from);
	draw_cursor(move.to);
	output_move(move);
	return addscore;
}

void endgame(const int score, const int moves, const time_t starttime)
{
	clrscr();
	gotoxy(0, 1);

	if (score < 15) {
		setcolor(Color::red);
		std::cout << "\220CHEC !\n\n";
	}
	else {
		setcolor(Color::grn);
		std::cout << "VICTOIRE !\n\n";
	}

	std::cout << "\n\n"
		<< "  Total des points\t    : " << score << " sur un objectif de " << 15
		<< "\n  Total des d\202placements    : " << moves
		<< "\n  Temps \202coul\202\t\t    : " << time(0) - starttime << " sec";

	_getch();
	return;
}

int main()
{
	setwsize(WIN_Y, WIN_X);								// Redimensionner la fenêtre de la console
	show(false);										// Afficher (oui/non) le trait d'affichage de la console
	draw_initial();
	draw_cursor({});

	Move move = {};
	int score = 0, movecount = 0;
	time_t start = time(0);

	center("D\220COUVREZ ET AMMASEZ 15 CASES $$$$", 2);
	
	do {
		uint8_t c = _getch();					// lire le premier code ascii du tampon
		if (c == 0 || c == 224) {				// vérifier s'il s'agit du code réservé. Si oui, il faut lire le code suivant
			if (_kbhit()) {
				c = _getch();
				if (check_move(Ak(c), move.from))
				{
					if (cursor_move(Ak(c), move))// Les fonctions dans un if sont exécutées même si la valeur finit fausse
					{
						++score;
						output_score(score);
					}
					move.from = move.to;
					++movecount;
				}
			}
		}

	} while (count_moves(move.to) != 0 && score < 15);
	endgame(score, movecount, start);
}