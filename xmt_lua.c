#include <lua.h>                               
#include <lauxlib.h>                           
#include <lualib.h>                            

static int sp_moogladder_L(lua_State *L)
{
	double cf = lua_tonumber(L, 1);
	double r = lua_tonumber(L, 2);
	size_t n = sizeof(sp_fx);
	sp_fx *fx = (sp_fx *)lua_newuserdata(L, n);
	moogladder_init(&D.data, fx, cf, r);
	return 1;
}

int luaopen_soundpipe(lua_State *L){
	lua_register(L, "sp_moogfilt", sp_moogladder_L);
	return 0;
}
