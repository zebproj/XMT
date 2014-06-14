#include <lua.h>                               
#include <lauxlib.h>                           
#include <lualib.h>                            
#include <stdint.h>
#include <sndfile.h>

#include "xmt-base.h"

void make_note_wrapper(
xm_note *n, 
int note,
int ins,
int vol,
int fx,
int param)
{
    xm_note tmp = make_note(
    (uint8_t) note,
    (uint8_t) ins,
    (uint8_t) vol,
    (uint8_t) fx,
    (uint8_t) param);
    *n = tmp;
}

static int L_hello(lua_State *L)
{
	//double cf = lua_tonumber(L, 1);
	//double r = lua_tonumber(L, 2);
	//size_t n = sizeof(sp_fx);
	//sp_fx *fx = (sp_fx *)lua_newuserdata(L, n);
	//moogladder_init(&D.data, fx, cf, r);
    printf("hello world!\n");
	return 1;
}

static int L_new(lua_State *L)
{
    xm_params p;
    xm_file *file = (xm_file *)lua_newuserdata(L, sizeof(xm_file));
    init_xm_params(&p);
    init_xm_file(file, &p); 
    return 1;
}

static int L_write(lua_State *L)
{
    xm_file *f = (xm_file *)lua_touserdata(L, 1);
    const char *filename = lua_tostring(L, 2);
	if(lua_isuserdata(L, 1))
	write_xm_file(f, filename);
	else
	printf("Could not close file.");
	return 1;
}

static int L_makenote(lua_State *L)
{   
    int note = lua_tonumber(L, 1);
    int ins = lua_tonumber(L, 2);
    int vol = lua_tonumber(L, 3);
    int fx = lua_tonumber(L, 4);
    int param = lua_tonumber(L, 5);
    xm_note *n = (xm_note *)lua_newuserdata(L, sizeof(xm_note));
    make_note_wrapper(n, note, ins, vol, fx, param);
    return 1;
}

static int L_addnote(lua_State *L)
{
    xm_file *f = (xm_file *)lua_touserdata(L, 1);
    int patnum = lua_tonumber(L, 2);
    int chan = lua_tonumber(L, 3);
    int row = lua_tonumber(L, 4);
    xm_note *note = (xm_note *)lua_touserdata(L, 5);
    add_note(f, (uint8_t)patnum, (uint8_t)chan, (uint8_t)row, *note);
    return 1;
}

static int L_addinstrument(lua_State *L)
{
    xm_file *f = (xm_file *)lua_touserdata(L, 1);
    int ins = add_instrument(f);
    lua_pushnumber(L, ins);
    return 1;
}

static int L_addsample(lua_State *L)
{
    xm_file *f = (xm_file *)lua_touserdata(L, 1);
    int ins = lua_tonumber(L, 2);
    const char *fname = lua_tostring(L, 3);
    xm_samp_params sparams = new_samp(fname);
    add_samp(f, &sparams, ins);
    return 1;
}

int luaopen_luaxmt(lua_State *L){
	lua_register(L, "hello", L_hello);
	lua_register(L, "xm_write", L_write);
	lua_register(L, "xm_new", L_new);
	lua_register(L, "xm_makenote", L_makenote);
	lua_register(L, "xm_addnote", L_addnote);
	lua_register(L, "xm_addinstrument", L_addinstrument);
	lua_register(L, "xm_addsample", L_addsample);
	return 0;
}
