/*
 *  Copyright 2014 The Luvit Authors. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#include "luv.h"

static const char* luv_signal_to_string(int signal) {
#ifdef SIGINT
  if (signal == SIGINT) return "SIGINT";
#endif
#ifdef SIGBREAK
  if (signal == SIGBREAK) return "SIGBREAK";
#endif
#ifdef SIGHUP
  if (signal == SIGHUP) return "SIGHUP";
#endif
#ifdef SIGWINCH
  if (signal == SIGWINCH) return "SIGWINCH";
#endif
  return "";
}

static int luv_string_to_signal(const char* string) {
#ifdef SIGINT
  if (strcmp(string, "SIGINT") == 0) return SIGINT;
#endif
#ifdef SIGBREAK
  if (strcmp(string, "SIGBREAK") == 0) return SIGBREAK;
#endif
#ifdef SIGHUP
  if (strcmp(string, "SIGHUP") == 0) return SIGHUP;
#endif
#ifdef SIGWINCH
  if (strcmp(string, "SIGWINCH") == 0) return SIGWINCH;
#endif
  return 0;
}

static uv_signal_t* luv_check_signal(lua_State* L, int index) {
  uv_signal_t* handle = luaL_checkudata(L, index, "uv_handle");
  luaL_argcheck(L, handle->type == UV_SIGNAL && handle->data, index, "Expected uv_signal_t");
  return handle;
}

static int luv_new_signal(lua_State* L) {
  uv_signal_t* handle = lua_newuserdata(L, sizeof(*handle));
  int ret = uv_signal_init(luv_loop(L), handle);
  if (ret < 0) {
    lua_pop(L, 1);
    return luv_error(L, ret);
  }
  handle->data = luv_setup_handle(L);
  return 1;
}

static void luv_signal_cb(uv_signal_t* handle, int signum) {
  lua_State* L = luv_state(handle->loop);
  luv_handle_t* data = handle->data;
  luv_find_handle(L, data);
  lua_pushstring(L, luv_signal_to_string(signum));
  luv_call_callback(L, data, LUV_SIGNAL, 2);
}

static int luv_signal_start(lua_State* L) {
  uv_signal_t* handle = luv_check_signal(L, 1);
  int signum, ret;
  signum = luv_string_to_signal(luaL_checkstring(L, 2));
  luv_check_callback(L, handle->data, LUV_SIGNAL, 3);
  luaL_argcheck(L, signum, 3, "Invalid Signal name");
  ret = uv_signal_start(handle, luv_signal_cb, signum);
  if (ret < 0) return luv_error(L, ret);
  lua_pushinteger(L, ret);
  return 1;
}

static int luv_signal_stop(lua_State* L) {
  uv_signal_t* handle = luv_check_signal(L, 1);
  int ret = uv_signal_stop(handle);
  if (ret < 0) return luv_error(L, ret);
  lua_pushinteger(L, ret);
  return 1;
}