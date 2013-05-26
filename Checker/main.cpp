#include "stdafx.h"
#include "Checker.h"
#include "Ai.h"

using namespace std;

int main() {		
	Ai ai_1("my_ai_1");	
	Ai ai_2("my_ai_2");
			
	cout<<"------------------NEW GAME----------------------"<<endl;
	Checker ca(10);
	int count = 0;
	do {		
		int turn = ca.getTurn();			
		GameMove m;
		if(turn == 1)
			m = ai_1.selectMove(ca);
		else
			m = ai_2.selectMove(ca);
		ca.movePiece(m);
		printf("P%d langkah dari [%d, %d] ke [%d, %d] sukses\n", turn, m.from.row, m.from.col, m.to.row, m.to.col);
		count++;			
	}while(ca.whoWin() == 0);
	printf("%d turns : %d , result : %d\n", i, count, ca.whoWin());					

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