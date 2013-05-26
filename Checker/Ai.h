#pragma once
#include "Checker.h"
using namespace std;

class Ai
{
public:	
	Ai(string);
	GameMove selectMove(const Checker& checker);
	~Ai();	
	static void staticInit();

private:
	static lua_State *L;
    int profile_ref;
};

typedef int (*lua_CFunction) (lua_State *L);	

