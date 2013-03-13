#include "stdafx.h"
#include "Checker.h"
#include "Ai.h"
#include <windows.h>

using namespace std;

int main() {	
	DWORD dw1 = GetTickCount();
	//pure random pure c
	for(int i=0;i<100;i++) {		
		Checker ca(10);
		int count = 0;
		do {		
			int turn = ca.getTurn();	
			vector<GameMove> legalMoves = ca.getAllLegalMoves();
			ca.movePiece(legalMoves[rand() % legalMoves.size()]);
			count++;
		}while(ca.whoWin() == 0);
		printf("%d turns : %d , result : %d\n", i, count, ca.whoWin());
	}
	DWORD dw2 = GetTickCount();
	cout << "time : "<<(dw2-dw1)<<endl;

	Ai ai;
	dw1 = GetTickCount();	
	//monte carlo hybrid c & lua
	for(int i=0;i<100;i++) {		
		cout<<"NEW GAME--------------------------"<<endl;
		Checker ca(10);
		int count = 0;
		do {		
			int turn = ca.getTurn();					
			ca.movePiece(ai.selectMove(ca));
			count++;
			ca.printBoard();
		}while(ca.whoWin() == 0);
		printf("%d turns : %d , result : %d\n", i, count, ca.whoWin());		
	}
	dw2 = GetTickCount();
	cout << "time : "<<(dw2-dw1)<<endl;

	/*
	cout<<"Checker start"<<endl;
	Checker c(10);
	int row1, row2, col1, col2;
	do {
		c.printBoard();
		int turn = c.getTurn();
		cout<<"giliran pemain : "<<turn<<endl;
		vector<GameMove> legalMoves = c.getAllLegalMoves();
		cout<<"legal move : ";
		for(int i=0;i<legalMoves.size();i++) printf("(%d,%d -> %d %d) ", legalMoves[i].from.row, legalMoves[i].from.col, legalMoves[i].to.row, legalMoves[i].to.col);
		cout<<endl<<"ada yang bisa dimakan ? "<<c.isThereEatable()<<endl;
		cout<<"masukkan koin di petak yang mana dan ke petak yang mana : ";
		cin>>row1>>col1>>row2>>col2;
		GameMove m;
		if(row1 != -1) {
			m = GameMove(Point(row1, col1), Point(row2, col2));			
		}else{
			//AI
			m = ai.selectMove(c);			
		}
		if(c.movePiece(m))
			printf("P%d langkah dari [%d, %d] ke [%d, %d] sukses\n", turn, m.from.row, m.from.col, m.to.row, m.to.col);
		else
			printf("P%d langkah dari [%d, %d] ke [%d, %d] GAGAL\n", turn, m.from.row, m.from.col, m.to.row, m.to.col);
	}while(c.whoWin() == 0);
	cout<<"winner : "<<c.whoWin()<<endl;
	cin>>row1;
	*/
	int a;
	cin>>a;
	return 0;
}