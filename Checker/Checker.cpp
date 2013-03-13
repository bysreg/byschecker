#include "stdafx.h"
#include "Checker.h"

using namespace std;

Checker::Checker(int size)
{
	this->size_ = size;
	for(int i=0;i<size;i++) {
		vector<GameTile> v;
		for(int j=0;j<size;j++) {
			v.push_back(GameTile());
			if(i%2 == j%2 || i==size/2-1 || i==size/2) 
				continue;
			v[j].is_there_piece = true;
			if(i<size/2) {
				v[j].player_number = 2;
				v[j].is_king = false;				
			}else{
				v[j].player_number = 1;
				v[j].is_king = false;
			}
		}
		this->board_.push_back(v);
	}

	turn_ = 1; // player 1 is on the bottom row of the board
	any_eatable_ = false;
	win_ = 0;
}

Checker::Checker(const string& game_state) {
	size_ = (int) sqrt(static_cast<float>(game_state.size()-1));		
	turn_ = game_state[game_state.size() - 1] - '0';	
	
	int string_index = 0;
	for(int i=0;i<size_;++i) {
		vector<GameTile> v;
		for(int j=0;j<size_;++j) {			
			v.push_back(convertToTile(game_state[string_index]));
			++string_index;
		}
		this->board_.push_back(v);
	}	

	any_eatable_ = isThereEatable();
	win_ = 0;
	if(getAllLegalMoves().size() == 0)
		win_ = (turn_ == 1 ? 2 : 1);	
}

string Checker::getGameStateString() const {
	string ret;	
	for(int i=0;i<size_;i++) {
		for(int j=0;j<size_;j++) {
			if(!board_[i][j].is_there_piece)
				ret.push_back('0');
			else if(board_[i][j].player_number == 1)
				ret.push_back((board_[i][j].is_king ? 2:1) + '0');
			else
				ret.push_back((board_[i][j].is_king ? 4:3) + '0');				
		}
	}
	ret.push_back(turn_ + '0');
	return ret;
}

Checker Checker::parseGameStateString(const string& game_state) {
	return Checker(game_state);	
}

Checker Checker::simulate(GameMove move) const {
	Checker ret = *this;
	
	ret.movePiece(move);

	return ret;
}

int Checker::getSize() const {
	return size_;
}

void Checker::removePiece(Point p) {
	board_[p.row][p.col].is_there_piece = false;
}

void Checker::setPiece(Point p, int player_number, bool is_king) {
	board_[p.row][p.col].is_there_piece = true;
	board_[p.row][p.col].player_number = player_number;
	board_[p.row][p.col].is_king = is_king;
}

bool Checker::isMoveValid(GameMove move) const {	
	Point from = move.from;
	Point to = move.to;	

	if(from.row < 0 || to.row < 0 || from.col < 0 || to.col < 0 || from.row >= size_ || from.col >= size_ || to.row >= size_ || to.col >= size_) {
		return false;// gak boleh keluar board
	}

	if(!board_[from.row][from.col].is_there_piece || 
		board_[to.row][to.col].is_there_piece) {
		return false; // piece ada di tile from, tidak ada piece di tile to
	}

	if(abs(from.row - to.row) > 2 || abs(from.col - to.col) > 2) {
		return false;// gak boleh gerak lebih dari selisih row dua atau selisih col dua
	}

	if(abs(from.row - to.row) != abs(from.col - to.col)) {
		return false;// hanya boleh gerak diagonal	
	}

	if(any_eatable_ && abs(from.row - to.row) == 1){
		return false;// jika ada piece musuh yang bisa dimakan, harus makan 
	}

	if(board_[from.row][from.col].player_number != turn_) {		
		return false;// bukan player yang sedang jalan
	}

	if((board_[from.row][from.col].player_number == 1 && to.row - from.row >= 0 && board_[from.row][from.col].is_king == false) || 
		(board_[from.row][from.col].player_number == 2 && to.row - from.row <= 0 && board_[from.row][from.col].is_king == false)) {
		return false; // pion biasa hanya boleh bergerak maju dari sisinya
	}

	Point mid((to.row + from.row) / 2, (to.col + from.col) / 2);
	if(abs(to.row - from.row) == 2 && (!board_[mid.row][mid.col].is_there_piece || (board_[mid.row][mid.col].is_there_piece && board_[mid.row][mid.col].player_number == turn_))) {
		return false;// kalau bergerak selisih row dua, maka harus melompati pion musuh
	}	
	return true;
}

bool Checker::movePiece(GameMove move) {		
	if(!isMoveValid(move)) 
		return false;

	Point from = move.from;
	Point to = move.to;
	bool eat = false;

	setPiece(to, board_[from.row][from.col].player_number, board_[from.row][from.col].is_king);		
	removePiece(from);

	if(abs(from.row - to.row) == 2) {
		eat = true;
		removePiece(Point((to.row + from.row) / 2, (to.col + from.col) / 2)); // makan
	}

	//jika setelah dipindahkan, piece berada di baris paling jauh board dari si pemain yang punya koin tersebut, jadikan raja
	if((turn_ == 1 && to.row==0) || (turn_==2 && to.row==size_-1))
		setPiece(to, turn_, true);
		
	if(!eat) {
		nextTurn();
	} else {
		any_eatable_ = isThereEatable();
		if(!any_eatable_)
			nextTurn();
	}
	return true;
}

vector<Point> Checker::getWalkableFromCoinInTile(int row, int col) const {
	vector<Point> arrGamePoint;		
	if(!board_[row][col].is_there_piece || board_[row][col].player_number!=turn_) {
		return arrGamePoint;
	}		
	Point from(row, col);		
	int drow[8] = {-1, -2, -1, -2, 1, 2, 1, 2};
	int dcol[8] = {-1, -2, 1, 2, 1, 2, -1, -2};
	for(int i=0;i<8;i++) {
		Point to(drow[i] + row, dcol[i] + col);
		GameMove move(from, to);
		if(isMoveValid(move))
			arrGamePoint.push_back(to);
	}				 		
	return arrGamePoint;
}

vector<GameMove> Checker::getAllLegalMoves() const {
	vector<GameMove> arrMove;
	for(int i=0;i<size_;i++) {
		for(int j=0;j<size_;j++) {
			if(board_[i][j].is_there_piece && board_[i][j].player_number==turn_) {
				vector<Point> arrGamePoint = getWalkableFromCoinInTile(i,j);
				for(int k=0;k < arrGamePoint.size();k++) {
					GameMove m;
					m.from.row = i;
					m.from.col = j;
					m.to.row = arrGamePoint[k].row;
					m.to.col = arrGamePoint[k].col;
					arrMove.push_back(m);
				}
			}
		}
	}
	return arrMove;
}

void Checker::printBoard() const{
	cout<<"    ";
	for(int i=0;i<size_;i++)
		cout<<" "<<i<<" ";
	cout<<endl;
	cout<<"    ";
	for(int i=0;i<size_;i++)
		cout<<"___";
	cout<<endl;
	for(int i=0;i<size_;i++) {
		for(int j=0;j<size_;j++) {
			if(j==0)
				cout<< i << " | ";
			GameTile piece = board_[i][j];
			if(!piece.is_there_piece) 
				cout << " 0 ";
			else if(piece.player_number == 1) 
				cout <<" "<< (1 * (piece.is_king ? 2 : 1)) << " ";
			else
				cout << (-1 * (piece.is_king ? 2 : 1)) << " ";
		}
		cout<<endl;
	}

}

GameTile Checker::convertToTile(char c) {
	int v = c - '0';
	GameTile ret;
	if(v == 0) {
		ret.is_there_piece = false;
	}else if(v <=2) {
		ret.is_there_piece = true;
		ret.player_number = 1;
		ret.is_king = (v==2 ? true:false);
	}else {
		ret.is_there_piece = true;
		ret.player_number = 2;
		ret.is_king = ((v - 2)==2? true:false);
	}
	
	return ret;
}

//mengembalikan nilai true, jika pada giliran sekarang ada yang bisa dimakan 
bool Checker::isThereEatable() const {
	for(int i=0;i<size_;i++) {
		for(int j=0;j<size_;j++) {			
			if(!board_[i][j].is_there_piece || board_[i][j].player_number != turn_) 
				continue;			

			GameTile tile = board_[i][j];
			int drow[4] = {-1, -1, 1, 1};
			int dcol[4] = {-1, 1, -1, 1};
			int far_row[4] = {-2, -2, 2, 2};
			int far_col[4] = {-2, 2, -2, 2};

			for(int k=0;k<4;k++) {

				if(turn_ == 1 && far_row[k] > 0 && !tile.is_king)
					continue;
				else if(turn_==2 && far_row[k] < 0 && !tile.is_king )
					continue;

				Point check(i + drow[k], j + dcol[k]);
				Point far_check(i + far_row[k], j + far_col[k]);				
				if(check.row < 0 || check.row >= size_ || check.col < 0 || check.col >= size_)
					continue;
				if(far_check.row < 0 || far_check.row >= size_ || far_check.col < 0 || far_check.col >= size_)
					continue;
				if(board_[check.row][check.col].is_there_piece && board_[check.row][check.col].player_number != tile.player_number && !board_[far_check.row][far_check.col].is_there_piece)
					return true;
			}			
		}
	}
	return false;
}

bool Checker::nextTurn() {
	turn_++;
	if(turn_ > 2)
		turn_ = 1;
	
	any_eatable_ = isThereEatable();

	for(int i=0;i<size_;i++) {
		for(int j=0;j<size_;j++) {			
			if(board_[i][j].is_there_piece && board_[i][j].player_number==turn_ && getWalkableFromCoinInTile(i,j).size() > 0) {				
				return true;
			}
		}
	}

	win_ = (turn_ == 1) ? 2 : 1;
	return false;	
}

int Checker::getTurn() const {
	return turn_;
}

int Checker::whoWin() const {
	return win_;
}