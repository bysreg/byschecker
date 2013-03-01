#include "StdAfx.h"
#include "Ai.h"

void bail(lua_State *L, char *msg){
	fprintf(stderr, "\nFATAL ERROR:\n  %s: %s\n\n",
		msg, lua_tostring(L, -1));
	int a;
	cin>>a;
	exit(1);
}

GameMove Ai::selectMove(const Checker& checker) {
	vector<GameMove> legalMoves = checker.getAllLegalMoves();
	lua_State *L;

    L = luaL_newstate();                        /* Create Lua state variable */
    luaL_openlibs(L);                           /* Load Lua libraries */

    if (luaL_loadfile(L, "ai.lua"))    /* Load but don't run the Lua script */
		bail(L, "luaL_loadfile() failed");      /* Error out if file can't be read */	

	if (lua_pcall(L, 0, 0, 0))                  /* Run the loaded Lua script */
		bail(L, "lua_pcall() failed");          /* Error out if Lua file has an error */	
	
	lua_getglobal(L, "monteCarlo");	
	lua_pushstring(L, checker.getGameStateString().c_str());
	lua_pushnumber(L, legalMoves.size());
	
	if(lua_pcall(L, 2, 1, 0))
		bail(L, "lua_pcall() failed");          /* Error out if Lua file has an error */	
	
    if (!lua_isnumber(L, -1))  /* retrieve result */
		bail(L, "function `f' must return a number");
    int ret = lua_tonumber(L, -1);
    lua_pop(L, 1);  /* pop returned value */	

	lua_close(L);                               /* Clean up, free the Lua state var */
	cout<<"AI select "<<(ret - 1)<<endl;
	return legalMoves[ret-1];
}