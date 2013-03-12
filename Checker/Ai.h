#pragma once
#include "Checker.h"
using namespace std;

class Ai
{
private:
	lua_State *L;

public:	
	Ai();
	GameMove selectMove(const Checker& checker);
	~Ai();
};

typedef int (*lua_CFunction) (lua_State *L);	

