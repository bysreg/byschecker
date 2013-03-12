#pragma once

using namespace std;

struct Point {
	int row;
	int col;
	Point() {row = 0; col = 0;}
	Point(int row, int col) {
		this->row = row;
		this->col = col;
	}
};

struct GameMove {
	Point from;
	Point to;	
	GameMove() {}
	GameMove(Point from, Point to) {
		this->from = from;
		this->to = to;
	}
};

struct GameTile {
public:
	bool is_there_piece;
	int player_number; // 1 or 2 
	bool is_king;

	GameTile() {
		is_there_piece = false;
		is_king = false;
		player_number = 1;
	}
	GameTile(bool is_there_piece, int player_number, bool is_king) {
		this->is_there_piece = is_there_piece;
		this->player_number = player_number;
		this->is_king = is_king;
	}
};

class Checker
{
private:
	int size_;
	vector<vector<GameTile> > board_;
	int turn_;
	bool any_eatable_; // apakah ada piece musuh yang bisa dimakan
	int win_;

	void removePiece(Point p);
	void setPiece(Point p, int player_number, bool is_king);

public:	
	Checker(int size);
	Checker(const string& game_state);	

	string getGameStateString() const;
	static Checker parseGameStateString(const string& game_state);
	Checker simulate(GameMove move) const;	
		
	bool movePiece(GameMove move);
	vector<Point> getWalkableFromCoinInTile(int row,int col) const; 
	vector<GameMove> getAllLegalMoves() const;
	static GameTile convertToTile(char c);
	void printBoard() const;	
	bool isThereEatable() const;
	bool nextTurn();
	int getTurn() const;
	int whoWin() const;
	bool isMoveValid(GameMove move) const;
};

