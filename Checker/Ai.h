#pragma once
#include "Checker.h"
using namespace std;

class Ai
{
private:
	lua_State *L;
	const int algo_type;

public:	
	Ai(int);
	GameMove selectMove(const Checker& checker);
	~Ai();	
};

typedef int (*lua_CFunction) (lua_State *L);	

