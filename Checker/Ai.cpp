#include "StdAfx.h"
#include "Ai.h"

void bail(lua_State *L, char *msg){
	fprintf(stderr, "\nFATAL ERROR:\n  %s: %s\n\n",
		msg, lua_tostring(L, -1));	
	getchar(); //pause before exit
	exit(1);
}

//mengembalikan banyaknya langkah yang bisa diambil
static int l_getNumberOfMoves(lua_State *L) {
	string game_state = luaL_checkstring(L, 1);
	Checker checker(game_state);
	vector<GameMove> legalMoves = checker.getAllLegalMoves();	
	lua_pushnumber(L, legalMoves.size());
	return 1;
}

static int l_simulate(lua_State *L) {
	string game_state = luaL_checkstring(L, 1);
	int move_num = luaL_checkinteger(L, 2);
	Checker checker(game_state);
	vector<GameMove> legalMoves = checker.getAllLegalMoves();
	checker.movePiece(legalMoves[move_num]);
	lua_pushstring(L, checker.getGameStateString().c_str());
	return 1;
}

static int l_whoWin(lua_State *L) {
	string game_state = luaL_checkstring(L, 1);
	Checker checker(game_state);
	int winner = checker.whoWin(); // 0 berarti seri, 1 berarti P1, -1 berarti P2, 2 berarti blom ada yang menang
	if(winner == 2) 
		winner = -1;
	else if(winner == 1)
		winner = 1;
	else if(winner == 0)
		winner = 2;
	lua_pushinteger(L, winner);
	return 1;
}

// mengeluarkan 1 untuk P1(yang jalan duluan), 2 untuk P2
static int l_getTurn(lua_State *L) {
	string game_state = luaL_checkstring(L, 1);
	Checker checker(game_state);	
	lua_pushinteger(L, checker.getTurn());
	return 1;
}

static const struct luaL_Reg aiclib_funcs [] = {
    {"getNumberOfMoves", l_getNumberOfMoves},
	{"simulate", l_simulate},
	{"whoWin", l_whoWin},
	{"getTurn", l_getTurn},
    {NULL, NULL}  /* sentinel */
};

int luaopen_aiclib (lua_State *L) {
	luaL_newlib(L, aiclib_funcs);	
    return 1;
}

Ai::Ai(int algo_type) : L(NULL), algo_type(algo_type) {	
	L = luaL_newstate();                        /* Create Lua state variable */
    luaL_openlibs(L);                           /* open semua standard Lua libraries */

	luaL_requiref(L, "aif", luaopen_aiclib, 1); // butuh aif (AI functions) dan taro di global name lua

    if (luaL_loadfile(L, "ai.lua"))    /* Load but don't run the Lua script */
		bail(L, "luaL_loadfile() failed");      /* Error out if file can't be read */	

	if (lua_pcall(L, 0, 0, 0))                  /* Run the loaded Lua script */
		bail(L, "lua_pcall() failed");          /* Error out if Lua file has an error */	
}

Ai::~Ai() {
	lua_close(L);                               /* Clean up, free the Lua state var */
}

GameMove Ai::selectMove(const Checker& checker) {
	vector<GameMove> legalMoves = checker.getAllLegalMoves();	
		
	lua_getglobal(L, "monteCarlo");	
	lua_pushstring(L, checker.getGameStateString().c_str());
	lua_pushnumber(L, legalMoves.size());	
	
	if(lua_pcall(L, 2, 1, 0))
		bail(L, "lua_pcall() failed");          /* Error out if Lua file has an error */	
    
	int ret = lua_tonumber(L, -1);
    lua_pop(L, 1);  /* pop returned value */		
	return legalMoves[ret];
}