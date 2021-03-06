/*
** mrb_curses.c - Curses class
**
** Copyright (c) Kotaro Hibi 2014
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/string.h"
#include "mrb_curses.h"
#include <ncurses.h>

#define DONE mrb_gc_arena_restore(mrb, 0);

// static mrb_value rb_stdscr;
struct windata {
  WINDOW *window;
};

WINDOW *echo_win = NULL;

typedef struct {
  char *str;
  int len;
} mrb_curses_data;

static const struct mrb_data_type mrb_curses_data_type = {
  "mrb_curses_data", mrb_free,
};

static mrb_value mrb_curses_init(mrb_state *mrb, mrb_value self)
{
  mrb_curses_data *data;

  data = (mrb_curses_data *)DATA_PTR(self);
  if (data) {
    mrb_free(mrb, data);
  }
  DATA_TYPE(self) = &mrb_curses_data_type;
  DATA_PTR(self) = NULL;

  data = (mrb_curses_data *)mrb_malloc(mrb, sizeof(mrb_curses_data));
  data->str = "dummy";
  data->len = 5;
  DATA_PTR(self) = data;

  return self;
}

static mrb_value
mrb_curses_initscr(mrb_state *mrb, mrb_value self)
{
  // if (rb_stdscr) return rb_stdscr;
  initscr();
  /*
  if (stdscr == 0) {
    rb_raise(rb_eRuntimeError, "can't initialize curses");
  }
  */
  clear();
  // rb_stdscr = prep_window(cWindow, stdscr);
  // return rb_stdscr;
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_endwin(mrb_state *mrb, mrb_value self)
{
  endwin();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_cbreak(mrb_state *mrb, mrb_value self)
{
  cbreak();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_nocbreak(mrb_state *mrb, mrb_value self)
{
  nocbreak();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_echo(mrb_state *mrb, mrb_value self)
{
  echo();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_noecho(mrb_state *mrb, mrb_value self)
{
  noecho();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_clear(mrb_state *mrb, mrb_value self)
{
  clear();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_addstr(mrb_state *mrb, mrb_value self)
{
  mrb_value obj;

  mrb_get_args(mrb, "S", &obj);
  const char *body = mrb_string_value_ptr(mrb, obj);
  addstr(body);
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_keypad(mrb_state *mrb, mrb_value self)
{
  mrb_bool obj = TRUE;
  mrb_get_args(mrb, "|b", &obj);
  keypad(stdscr, obj);
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_move(mrb_state *mrb, mrb_value self)
{
  mrb_value v1, v2;
  mrb_get_args(mrb, "ii", &v1, &v2);
  move(mrb_fixnum(v1), mrb_fixnum(v2));
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_start_color(mrb_state *mrb, mrb_value self)
{
  start_color();
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_init_pair(mrb_state *mrb, mrb_value self)
{
  mrb_value v1, v2, v3;
  mrb_get_args(mrb, "iii", &v1, &v2, &v3);
  int no = mrb_fixnum(v1);
  int stcolor = mrb_fixnum(v2);
  int bgcolor = mrb_fixnum(v3);
  init_pair(no, stcolor, bgcolor);
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_coloron(mrb_state *mrb, mrb_value self)
{
  mrb_value v1;

  mrb_get_args(mrb, "i", &v1);
  int no = mrb_fixnum(v1);
  attron(COLOR_PAIR(no));
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_coloroff(mrb_state *mrb, mrb_value self)
{
  mrb_value v1;

  mrb_get_args(mrb, "i", &v1);
  int no = mrb_fixnum(v1);
  attroff(COLOR_PAIR(no));
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_wbkgd(mrb_state *mrb, mrb_value self)
{
  mrb_value v1;

  mrb_get_args(mrb, "i", &v1);
  int no = mrb_fixnum(v1);
  wbkgd(stdscr, COLOR_PAIR(no));
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_echoline(mrb_state *mrb, mrb_value self)
{
  int h, w;
  mrb_value height, width;

  mrb_get_args(mrb, "ii", &height, &width);
  h = mrb_fixnum(height);
  w = mrb_fixnum(width);

  if (echo_win == NULL) {
    echo_win = subwin(stdscr, 1, 100, h, w);
  }

  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_ewaddstr(mrb_state *mrb, mrb_value self)
{
  if (echo_win == NULL) {
    return mrb_bool_value(false);
  }

  mrb_value obj;
  mrb_get_args(mrb, "S", &obj);
  const char *body = mrb_string_value_ptr(mrb, obj);
  waddstr(echo_win, body);

  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_ewmove(mrb_state *mrb, mrb_value self)
{
  if (echo_win == NULL) {
    return mrb_bool_value(false);
  }

  mrb_value v1, v2;
  mrb_get_args(mrb, "ii", &v1, &v2);
  wmove(echo_win, mrb_fixnum(v1), mrb_fixnum(v2));
  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_refresh(mrb_state *mrb, mrb_value self)
{
  refresh();
  if (echo_win != NULL) {
    wrefresh(echo_win);
  }

  return mrb_bool_value(true);
}

static mrb_value
mrb_curses_ewgetstr(mrb_state *mrb, mrb_value self)
{
  if (echo_win == NULL) {
    return mrb_bool_value(false);
  }

  char str[100];
  mrb_value s;
  echo();
  nocbreak();
  wgetstr(echo_win, str);
  noecho();
  cbreak();
  s =  mrb_str_new_cstr(mrb, str);
  return s;
}

static mrb_value
mrb_curses_screen_rows(mrb_state *mrb, mrb_value self)
{
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  return mrb_fixnum_value(rows);
}

static mrb_value
mrb_curses_screen_cols(mrb_state *mrb, mrb_value self)
{
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  return mrb_fixnum_value(cols);
}

void mrb_mruby_curses_gem_init(mrb_state *mrb)
{
    struct RClass *curses;
    curses = mrb_define_class(mrb, "Curses", mrb->object_class);
    MRB_SET_INSTANCE_TT(curses, MRB_TT_DATA);

    mrb_define_method(mrb, curses, "initialize", mrb_curses_init, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "initscr", mrb_curses_initscr, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "cbreak", mrb_curses_cbreak, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "nocbreak", mrb_curses_cbreak, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "echo", mrb_curses_noecho, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "noecho", mrb_curses_noecho, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "keypad", mrb_curses_keypad, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, curses, "clear", mrb_curses_clear, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "addstr", mrb_curses_addstr, MRB_ARGS_REQ(1));
    mrb_define_class_method(mrb, curses, "move", mrb_curses_move, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, curses, "endwin", mrb_curses_endwin, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "start_color", mrb_curses_start_color, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "init_pair", mrb_curses_init_pair, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, curses, "coloron", mrb_curses_coloron, MRB_ARGS_REQ(1));
    mrb_define_class_method(mrb, curses, "coloroff", mrb_curses_coloroff, MRB_ARGS_REQ(1));
    mrb_define_class_method(mrb, curses, "wbkgd", mrb_curses_wbkgd, MRB_ARGS_REQ(1));

    mrb_define_class_method(mrb, curses, "echoline", mrb_curses_echoline, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, curses, "ewaddstr", mrb_curses_ewaddstr, MRB_ARGS_REQ(1));
    mrb_define_class_method(mrb, curses, "ewmove", mrb_curses_ewmove, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, curses, "refresh", mrb_curses_refresh, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "ewgetstr", mrb_curses_ewgetstr, MRB_ARGS_NONE());

    mrb_define_class_method(mrb, curses, "screen_rows", mrb_curses_screen_rows, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, curses, "screen_cols", mrb_curses_screen_cols, MRB_ARGS_NONE());

    mrb_define_const(mrb, curses, "COLOR_BLACK",  mrb_fixnum_value(COLOR_BLACK));
    mrb_define_const(mrb, curses, "COLOR_RED",  mrb_fixnum_value(COLOR_RED));
    mrb_define_const(mrb, curses, "COLOR_GREEN",  mrb_fixnum_value(COLOR_GREEN));
    mrb_define_const(mrb, curses, "COLOR_YELLOW",  mrb_fixnum_value(COLOR_YELLOW));
    mrb_define_const(mrb, curses, "COLOR_BLUE",  mrb_fixnum_value(COLOR_BLUE));
    mrb_define_const(mrb, curses, "COLOR_MAGENTA",  mrb_fixnum_value(COLOR_MAGENTA));
    mrb_define_const(mrb, curses, "COLOR_CYAN",  mrb_fixnum_value(COLOR_CYAN));
    mrb_define_const(mrb, curses, "COLOR_WHITE",  mrb_fixnum_value(COLOR_WHITE));

    DONE;
}

void mrb_mruby_curses_gem_final(mrb_state *mrb)
{
}
