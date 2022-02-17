#include <conio.h>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <Windows.h>

#include <cvm 21.h>

// PRAGMA
#pragma warning (disable:6031)		// valeur de retour ignorée '_getch'
#pragma warning (disable:26812)		// type enum 'type-name' est non délimité. Préférez 'enum class' à 'enum' (enum.3)

//using namespace std;
using duration = std::chrono::high_resolution_clock::duration;
using time_point = std::chrono::high_resolution_clock::time_point;

// DIMENSION DU DAMIER

const size_t LIG = 8, COL = 12;											// lignes et colonnes du damier
const size_t MAXSCORE = 15;												// Score requis pour gagner

// POSITION DU DAMIER À LA CONSOLE

const size_t START_L = 0;												//Position de départ du joueur en X
const size_t START_C = 0;												//Position de départ du joueur en Y

// CARACTÉRISTIQUES DES CASES À LA CONSOLE

const size_t PADDING_X = 10;											// X vide autour de la zone de jeu
const size_t PADDING_Y = 5;												// Y vide autour de la zone de jeu

const size_t CASE_X = 6;												// largeur d'une case à l'affichage, 2 minimum
const size_t CASE_Y = 3;												// hauteur d'une case à l'affichage, 2 minimum

const size_t SPACE_X = 2;												// nombres de colonnes vides entre les cases, 1 minimum
const size_t SPACE_Y = 1;												// nombres de lignes vides entre les cases, 1 minimum

const size_t DELTA_X = CASE_X + SPACE_X;								// saut d'une case à l'autre, sur l'axe des X
const size_t DELTA_Y = CASE_Y + SPACE_Y;								// saut d'une case à l'autre, sur l'axe des Y

// DIMENSION DE LA FENÊTRE

const size_t WIN_X = 2 * PADDING_X + (COL - 1) * DELTA_X + CASE_X;		// nombres de colonnes pour l'affichage d'une case
const size_t WIN_Y = 2 * PADDING_Y + (LIG - 1) * DELTA_Y + CASE_Y;		// nombres de lignes   pour l'affichage d'une case

// COMMANDES

enum class Arrowkeys {						// Code ascii décimal des touches fléchées du clavier
	up_left		= 71,
	up			= 72,
	up_right	= 73,
	left		= 75,
	right		= 77,
	down_left	= 79,
	down		= 80,
	down_right	= 81,
};

using Ak = Arrowkeys;						// un alias plus concis

// STRUCTURES DE COOODONNÉES

struct XY {									// coordonnée graphique (x,y) d'une case dans la console
	size_t x, y;
};

struct LC {									// ligne et colonne (l,c) d'une case du damier[l][c]
	size_t l, c;							// ligne: [0..LIG-1] -- colonne: [0..COL-1]

	//Comparaison de membres entre deux LC
	inline bool operator==(LC pos) { return (pos.l == l && pos.c == c);	}

	//Addition et assignement entre deux LC
	LC& operator+=(const LC& pos) {
		l += pos.l; c += pos.c;
		return *this;
	}

	//Conversion explicite de LC à XY
	explicit operator XY () { return XY({ PADDING_X + (c * DELTA_X), PADDING_Y + (l * DELTA_Y) }); };
};
//Addition de deux LC
LC operator+(LC lpos, const LC& rpos) {
	lpos += rpos;
	return lpos;
}
//Source importante
//https://stackoverflow.com/questions/4421706/what-are-the-basic-rules-and-idioms-for-operator-overloading/4421719#4421719

struct Move {								// coordonnées des 2 cases impliquées dans un déplacement
	LC from, to;
	friend std::ostream& operator<<(std::ostream&, const Move&);
};
//Opérateur de bitshift permettant d'insérer la représentation de la structure dans un output stream
std::ostream& operator<<(std::ostream& os, const Move& move) {
	os << "move: (" << move.from.l << ',' << move.from.c << ") --> (" << move.to.l << ',' << move.to.c << ')';
	return os;
};

// CASES, DAMIER ET TRANSFORMATIONS

enum Case { CO, CS, CD, CF, CV };			// les différentes cases possibles: ordinaire, surprise, dollars, fragile, vide

struct Map {								// Structure pour un tableau de cases
	Case state[LIG][COL];
	//Opérateur d'indice de tableau, afin d'utiliser une structure LC comme indice à deux dimensions
	Case& operator[](const LC& pos) { return state[pos.l][pos.c]; }
};

Map damier = {{						// le damier et ses cases initiales

	{ CO, CO, CO, CO, CO, CO, CO, CO, CV, CO, CO, CS },
	{ CO, CO, CV, CV, CO, CO, CO, CO, CO, CV, CO, CV },
	{ CO, CO, CV, CS, CV, CO, CO, CO, CO, CO, CV, CS },
	{ CO, CO, CV, CS, CV, CO, CO, CV, CV, CO, CV, CS },
	{ CS, CO, CV, CV, CV, CS, CV, CO, CV, CO, CV, CO },
	{ CS, CO, CS, CS, CO, CS, CV, CS, CV, CO, CV, CO },
	{ CS, CO, CS, CO, CO, CO, CV, CV, CV, CO, CV, CO },
	{ CS, CS, CO, CO, CO, CO, CO, CO, CO, CO, CO, CO }
}};

Case futur[5] = { CF, CD, CF, CV, CV };		// la liste des transformations possibles  --  ex: futur[CO] ==> CF

struct Style {								// le style pour l'affichage d'une case
	Color color; char c;
};
Style map[5] = {							// les style pour toutes les cases
	{ Color::blu, '\xB2' },					// ex: map[CO].c == '\xB2'
	{ Color::blu, '\xB2' },
	{ Color::grn, '\x24' },					// ex: map[CD].color == Color::grn 
	{ Color::pur, '\xB0' },
	{ Color::blk, '\x00' }
};

struct PlayerMove {
	Arrowkeys move;
	time_point time;
};

// AFFICHAGE DU CURSEUR

char cursor[3][3] =	{						// informations pour l'affichage du curseur
	{ '\xC9', '\xCD', '\xBB' },
	{ '\xBA', '\x00', '\xBA' },
	{ '\xC8', '\xCD', '\xBC' }
};

//	FONCTIONS SIMPLES

void gotoxy(XY xy) {
	gotoxy(xy.x, xy.y);
}


class Board {
	private:
		Map board = damier;
		LC pos = {START_L, START_C};
		Move lastmove;
		const XY move_output = XY(LC({ LIG, 0 }));
		void output_score();
		void draw_case(LC);
		LC ak_to_move(const Arrowkeys);
		std::vector<PlayerMove> replay = {};
		void record_move(const Arrowkeys);
		void replay_game();
		bool replaying = false;
	public:
		int score = 0;
		void draw_initial();
		bool check_move(const Arrowkeys);
		int count_moves();
		void move(const Arrowkeys);
		void endgame();
};

void Board::draw_case(LC coords) {
	XY consolepos = XY(coords);
	gotoxy(consolepos);

	if (coords == pos) {
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
	}
	else {
		Case type = board[coords];
		setcolor(map[type].color);
		std::string line(CASE_X, map[type].c);

		for (int lin = 0; lin < CASE_Y; ++lin) {
			gotoxy(consolepos.x, consolepos.y++);
			std::cout << line;
		}
	}
	setcolor(Color::wht);
	return;
}

//Conversion d'Arrowkey à un delta LC
//{1, 0} signifie un déplacement de 1 pour x et de 0 pour y
LC Board::ak_to_move(Arrowkeys key) {

	LC move = {};
	if (key <= Ak::up_right)
		move.l = -1;
	else if (key >= Ak::down_left)
		move.l = 1;

	if (key == Ak::down_left || key == Ak::left || key == Ak::up_left)
		move.c = -1;
	else if (key == Ak::down_right || key == Ak::right || key == Ak::up_right)
		move.c = 1;
	return move;
}

//Retourne true si un mouvement est valide, sinon false
bool Board::check_move(Arrowkeys key) {
	LC move = ak_to_move(key);
	LC presume_pos = pos + move;
	if (presume_pos.l >= LIG || presume_pos.l < 0 || presume_pos.c >= COL || presume_pos.c < 0)
		return false;
	else if (board[presume_pos] == Case::CV)
		return false;
	else
		return true;
}

//Retourne le nombre de mouvements valides du curseur
int Board::count_moves() {
	Ak keys[8] = { Arrowkeys::up_left, Arrowkeys::up, Arrowkeys::up_right, Arrowkeys::left, Arrowkeys::right,
				Arrowkeys::down_left, Arrowkeys::down, Arrowkeys::down_right };
	int moves = 0;
	for (int i = 0; i < 8; ++i)
		if (check_move(keys[i]))
			++moves;
	return moves;
}

//Effectue un mouvement et les calculs nécessaires
void Board::move(const Arrowkeys key) {
	LC move = ak_to_move(key);
	LC oldpos = pos;
	pos += move;
	bool addmoney = (board[pos] == CD);
	Case* newstate = &board.state[pos.l][pos.c];
	*newstate = futur[*newstate];
	lastmove = { oldpos, pos };
	draw_case(oldpos);
	draw_case(pos);
	if (!replaying)
		record_move(key);

	setcolor(Color::yel);
	gotoxy(move_output);
	std::cout << lastmove << ' ';

	if (addmoney) {
		++score;
		output_score();
	}
	return;
}

//Enregistre un mouvement du joueur
void Board::record_move(const Arrowkeys key) {
	replay.push_back({ key, std::chrono::high_resolution_clock::now() });
	return;
}

//Replay de la partie
void Board::replay_game() {
	board = damier;
	score = 0;
	pos = { START_L, START_C };
	replaying = true;
	clrscr();
	draw_initial();

	for (auto iter = replay.begin(); iter != replay.end(); ++iter) {
		move(iter->move);
		if (iter->time <= replay.end()[-2].time) {	//Vérifier si c'est l'avant dernier élément
			auto wait(std::next(iter)->time - iter->time);
			std::this_thread::sleep_for(wait);
		}
	}
	endgame();
}

//Affiche le score
void Board::output_score() {
	XY score_ouput;
	score_ouput = XY(LC({LIG, COL}));	//Bottom right
	score_ouput.x -= SPACE_X + std::string("$$$$ : ").length() + std::to_string(score).length();	//Déplacement à gauche dynamique

	gotoxy(score_ouput);
	setcolor(Color::grn);
	std::cout << "$$$$ : " << score;
}

//Initialisation graphique du jeu
void Board::draw_initial() {
	for (size_t y = 0; y < COL; ++y)
		for (size_t x = 0; x < LIG; ++x)
			draw_case(LC({ x, y }));
	output_score();
	return;
}

//Centrer du texte dans la console
void center(const std::string text, const int yoffset) {
	int sizex;
	CONSOLE_SCREEN_BUFFER_INFO buffInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buffInfo);
	sizex = buffInfo.dwSize.X;
	
	setcolor(Color::wht);
	gotoxy(sizex / 2 - text.size() / 2, yoffset);
	std::cout << text;
}

//Fin de partie
void Board::endgame() {
	clrscr();
	gotoxy(0, 1);

	auto time(std::chrono::duration_cast<std::chrono::milliseconds>(replay.back().time - replay.front().time));
	if (score < MAXSCORE) {
		setcolor(Color::red);
		std::cout << "\220CHEC !\n\n";
	}
	else {
		setcolor(Color::grn);
		std::cout << "VICTOIRE !\n\n";
	}

	std::cout << "\n\n"
			<< "  Total des points\t    : " << score << " sur un objectif de " << MAXSCORE
			<< "\n  Total des d\202placements    : " << replay.size()
			<< "\n  Temps \202coul\202\t\t    : " << time.count() / 1000.0F << " secondes";

	_getch();
	replay_game();
}



int main() {
	setwsize(WIN_Y, WIN_X);								// redimensionner la fenêtre de la console
	show(false);										// afficher (oui/non) le trait d'affichage de la console

	Board game;											//Objet de la partie
	game.draw_initial();
	center("D\220COUVREZ ET AMMASEZ " + std::to_string(MAXSCORE) + " CASES $$$$", 2);
	
	do {
		uint8_t c = _getch();					// lire le premier code ascii du tampon
		if (c == 0 || c == 224)	{				// vérifier s'il s'agit du code réservé. Si oui, il faut lire le code suivant
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
	game.endgame();

	return 0;
}