#include "StdAfx.h"
#include "Checker.h"
#include <iostream>
#include <string>
#include <cstdio>

using namespace std;


int main() {
	cout<<"Checker start"<<endl;
	Checker c = Checker(10);
	int row1, row2, col1, col2;
	do {
		c.printBoard();
		cout<<"giliran pemain : "<<c.getTurn()<<endl;
		vector<GameMove> legalMoves = c.getAllLegalMoves();
		cout<<"legal move : ";
		for(int i=0;i<legalMoves.size();i++) printf("(%d,%d -> %d %d) ", legalMoves[i].from.row, legalMoves[i].from.col, legalMoves[i].to.row, legalMoves[i].to.col);
		cout<<endl<<"ada yang bisa dimakan ? "<<c.isThereEatable()<<endl;
		cout<<"masukkan koin di petak yang mana dan ke petak yang mana : ";
		cin>>row1>>col1>>row2>>col2;
		if(c.movePiece(GameMove(Point(row1, col1), Point(row2, col2))))
			printf("P%d langkah dari [%d, %d] ke [%d, %d] sukses\n", c.getTurn(), row1, col1, row2, col2);
		else
			printf("P%d langkah dari [%d, %d] ke [%d, %d] GAGAL\n", c.getTurn(), row1, col1, row2, col2);
	}while(c.whoWin() == 0);
	cout<<"winner : "<<c.whoWin()<<endl;
	return 0;
}