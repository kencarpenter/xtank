/*-
 * Copyright (c) 1988 Terry Donahue
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "xtank.h"
#include "mdesign.h"
#include "graphics.h"
#include "gr.h"
#include "terminal.h"
#include "proto.h"

extern Terminal *term;
extern Map real_map;
extern int team_color[];


static Mdesc design_mdesc;

static int
handle_key(Event *event)
{
	PixC ploc;
	BoxC bloc;

	ploc.x = event->x;
	ploc.y = event->y;
	pix_to_box(&ploc, &bloc);

	switch (event->key) {
	  case ' ':
		  make_landmark(&bloc, NORMAL);
		  break;
	  case 'a':
		  make_landmark(&bloc, ARMOR);
		  break;
	  case 'p':
		  make_landmark(&bloc, PEACE);
		  break;
	  case 't':
		  set_teleport_code(&bloc);
		  make_landmark(&bloc, TELEPORT);
		  break;
	  case 'f':
		  make_landmark(&bloc, FUEL);
		  break;
	  case 'w':
		  make_landmark(&bloc, AMMO);
		  break;
	  case 'o':
		  make_landmark(&bloc, OUTPOST);
		  break;
	  case 'g':
		  make_landmark(&bloc, GOAL);
		  break;
	  case '-':
		  make_landmark(&bloc, SLIP);
		  break;
	  case '=':
		  make_landmark(&bloc, SLOW);
		  break;
	  case 'x':
		  make_landmark(&bloc, START_POS);
		  break;
	  case '1':
		  make_landmark(&bloc, SCROLL_SW);
		  break;
	  case '2':
		  make_landmark(&bloc, SCROLL_S);
		  break;
	  case '3':
		  make_landmark(&bloc, SCROLL_SE);
		  break;
	  case '4':
		  make_landmark(&bloc, SCROLL_W);
		  break;
	  case '6':
		  make_landmark(&bloc, SCROLL_E);
		  break;
	  case '7':
		  make_landmark(&bloc, SCROLL_NW);
		  break;
	  case '8':
		  make_landmark(&bloc, SCROLL_N);
		  break;
	  case '9':
		  make_landmark(&bloc, SCROLL_NE);
		  break;

	  case ')':
		  set_team(&bloc, 0);
		  break;
	  case '!':
		  set_team(&bloc, 1);
		  break;
	  case '@':
		  set_team(&bloc, 2);
		  break;
	  case '#':
		  set_team(&bloc, 3);
		  break;
	  case '$':
		  set_team(&bloc, 4);
		  break;
	  case '%':
		  set_team(&bloc, 5);
		  break;
	  case '^':
		  set_team(&bloc, 6);
		  break;

	  case 'm':
		  move_area();
		  break;
	  case 'c':
		  copy_area();
		  break;
	  case 'e':
		  erase_area();
		  break;

	  case 'Q':
		  if (confirm(ANIM_WIN, "quit", 0, INPUT_Y, MD_FONT))
			  return GAME_QUIT;
		  mdesign_clear_input();
		  break;
	  case 'E':
		  if (confirm(ANIM_WIN, "erase this maze", 0, INPUT_Y, MD_FONT)) {
			  unmake_maze();
			  mdesign_show_anim();
		  } else
			  mdesign_clear_input();
		  break;
	  case 'S':
		  mdesign_save();
		  break;
	  case 'L':
		  mdesign_load();
		  mdesign_show_anim();
		  break;
	}

	return GAME_RUNNING;
}

void
design_maze(void)
{
	Event event;
	int num_events;
	Boolean finished = FALSE;

	/* Expose the animation window to get it to display everything */
	expose_win(ANIM_WIN, TRUE);
	set_cursor(PLUS_CURSOR);

	do {
		if (win_exposed(ANIM_WIN)) {
			mdesign_show_anim();
			expose_win(ANIM_WIN, FALSE);
		}
		if (win_exposed(HELP_WIN)) {
			mdesign_show_help();
			expose_win(HELP_WIN, FALSE);
		}
		num_events = 1;
		get_events(&num_events, &event);
		if (num_events == 0)
			continue;
		switch ((int) event.type) {
		  case EVENT_KEY:
			  if (handle_key(&event) == GAME_QUIT)
				  finished = TRUE;
			  break;
		  case EVENT_LBUTTON:
		  case EVENT_MBUTTON:
		  case EVENT_RBUTTON:
			  if (handle_button(&event) == GAME_QUIT)
				  finished = TRUE;
			  break;
		}
	} while (finished == FALSE);

	set_cursor(CROSS_CURSOR);
	clear_window(HELP_WIN);
}

static int
handle_button(Event *event)
{
	int num_events;
	Boolean finished;
	PixC loc;

	loc.x = event->x;
	loc.y = event->y;

	switch (event->type) {
	  case EVENT_LBUTTON:
		  change_maze(&loc, MAKE_NORM_WALL);
		  break;
	  case EVENT_MBUTTON:
		  change_maze(&loc, MAKE_DEST_WALL);
		  break;
	  case EVENT_RBUTTON:
		  change_maze(&loc, DESTROY_WALL);
		  break;
	  default:
		  break;
	}

#ifndef X11
	follow_mouse(ANIM_WIN, TRUE);
	button_up(ANIM_WIN, TRUE);
#endif

	/* Keep trying to change the maze until the button is released */
	finished = FALSE;
	do {
		num_events = 1;
		get_events(&num_events, event);
		switch (event->type) {
		  case EVENT_LBUTTONUP:
		  case EVENT_MBUTTONUP:
		  case EVENT_RBUTTONUP:
			  finished = TRUE;
			  break;
		  case EVENT_MOVED:
			  loc.x = event->x;
			  loc.y = event->y;
			  change_maze(&loc, CONTINUE);
			  break;
		  default:
			  break;
		}
	} while (finished == FALSE);

#ifndef X11
	follow_mouse(ANIM_WIN, FALSE);
	button_up(ANIM_WIN, FALSE);
#endif

	return GAME_RUNNING;
}

/*
** Shows the maze, the instructions, and info about the mdesc.
*/
static void
mdesign_show_anim(void)
{
	clear_window(ANIM_WIN);
	display_mdesc_maze();
	display_mdesc_info(&design_mdesc);
	mdesign_show_help();
	flush_output();
}

/*
** Draws all the walls, landmarks, and teams in the maze.
*/
void
display_mdesc_maze(void)
{
	BoxC loc;

	for (loc.x = PAD; loc.x <= GRID_WIDTH - PAD; ++(loc.x))
		for (loc.y = PAD; loc.y <= GRID_HEIGHT - PAD; ++(loc.y)) {
			show_dot(&loc);
			show_box_fast(&loc);
		}
}

#define mprint(str,x,y) \
  (display_mesg2(ANIM_WIN,str,x,y,MD_FONT))

/*
** Shows the type, name, designer and description of the specified mdesc.
*/
void
display_mdesc_info(Mdesc *d)
{
	char temp[110];
	extern char *games_entries[];

	sprintf(temp, "Name: %-15s  Type: %-10s  Designer: %-24s",
			d->name, games_entries[(int) d->type], d->designer);
	mprint(temp, 0, INFO_Y);
	sprintf(temp, "Description: %-50s", d->desc);
	mprint(temp, 0, INFO_Y + 1);
}

#define hprint(str,x,y) \
  (display_mesg2(HELP_WIN,str,x,y,S_FONT))

static void
mdesign_show_help(void)
{
	hprint("WALLS & TEAMS", 0, 0);
	hprint("left    make wall", 0, 1);
	hprint("center  make dest", 0, 2);
	hprint("right   erase wall", 0, 3);
	hprint("!@#$%^  set team", 0, 4);
	hprint(")       unset team", 0, 5);

	hprint("LANDMARKS", 22, 0);
	hprint("space  normal", 22, 1);
	hprint("a      armor", 22, 2);
	hprint("w      ammo", 22, 3);
	hprint("f      fuel", 22, 4);
	hprint("g      goal", 22, 5);
	hprint("p      peace", 22, 6);
	hprint("t      teleport", 22, 7);

	hprint("o    outpost", 39, 1);
	hprint("-    slip", 39, 2);
	hprint("=    slow", 39, 3);
	hprint("1-9  scroll", 39, 4);
	hprint("x    start", 39, 5);

	hprint("UTILITIES", 56, 0);
	hprint("m  move area", 56, 1);
	hprint("c  copy area", 56, 2);
	hprint("e  erase area", 56, 3);
	hprint("L  load maze", 56, 4);
	hprint("E  erase maze", 56, 5);
	hprint("S  save maze", 56, 6);
	hprint("Q  quit", 56, 7);
}

/*
** Clears the input area.
*/
static void
mdesign_clear_input(void)
{
	int top, height;

	top = INPUT_Y * font_height(MD_FONT);
	height = 4 * font_height(MD_FONT);
	draw_filled_rect(ANIM_WIN, LEFT_BORDER, TOP_BORDER + top,
					 ANIM_WIN_WIDTH, height, DRAW_COPY, BLACK);
}

/*
** Changes the walls in the maze.  Remembers the last wall that was changed
** and will continue performing the same action as before if passed
** CONTINUE as an action.  Attempts to figure out what walls should be
** drawn based on the previous location of the pointer.
*/
static void
change_maze(PixC *loc, int action)
{
	static int cur_action;
	static Wall cur_wall;
	static BoxC curbloc;
	BoxC bloc;
	int i, j;					/* location of cursor in pixels, initially */
	Wall wl = 0;

	i = loc->x - X_OFFSET;
	j = loc->y - Y_OFFSET;

	/* See if the location is in the maze */
	if ((i / DES_BOX_WIDTH < PAD) || (i / DES_BOX_WIDTH > GRID_WIDTH - PAD) ||
	 (j / DES_BOX_HEIGHT < PAD) || (j / DES_BOX_HEIGHT > GRID_HEIGHT - PAD))
		return;

	/* See if location is within THRESHOLD pixels of a vertical wall */
	if ((i + THRESHOLD) % DES_BOX_WIDTH <= 2 * THRESHOLD) {
		wl |= WEST_WALL;
		/* When we do an int division by DES_BOX_WIDTH, we'll get the right
	   box */
		i += THRESHOLD;
	}
	/* See if location is within THRESHOLD pixels of a horizontal wall */
	if ((j + THRESHOLD) % DES_BOX_HEIGHT <= 2 * THRESHOLD) {
		wl |= NORTH_WALL;
		j += THRESHOLD;
	}
	/* * If he's close to both walls, we check to see if this is a new click *
       or if he's holding it down.  If he's holding it down, he's probably *
       trying to continue the line he started.  Otherwise, we toggle the *
       wall which he's closest to. */
	if (wl == (WEST_WALL | NORTH_WALL)) {
		if (((action == CONTINUE) && (cur_wall == WEST_WALL)) ||
			((action != CONTINUE) &&
			 (ABS(i % DES_BOX_WIDTH - THRESHOLD) < ABS(j % DES_BOX_HEIGHT - THRESHOLD)))) {
			wl = WEST_WALL;
			j -= THRESHOLD;		/* return j to old value */
		} else {
			wl = NORTH_WALL;
			i -= THRESHOLD;		/* return i to old value */
		}
	}
	/* Change to maze coords */
	bloc.x = i / DES_BOX_WIDTH;
	bloc.y = j / DES_BOX_HEIGHT;

	/* If we are not in the maze area, return */
	if (check_wall(&bloc, wl) == FALSE)
		return;

	/* * If the button is being held down, and we are on the same wall as *
       before, don't change it. */
	if ((action == CONTINUE) && (wl == cur_wall) &&
		(bloc.x == curbloc.x) && (bloc.y == curbloc.y))
		return;

	/* We are on a new wall, so remember it for later. * If the calling
       function sent a CONTINUE, a button is being held down. * Otherwise, a
       button was just pressed. */
	if (action != CONTINUE)
		cur_action = action;
	cur_wall = wl;
	curbloc.x = bloc.x;
	curbloc.y = bloc.y;

	/* Change the internal state */
	switch (cur_action) {
	  case MAKE_NORM_WALL:
		  make_wall(&bloc, wl);
		  break;
	  case MAKE_DEST_WALL:
		  make_destructible(&bloc, wl);
		  break;
	  case DESTROY_WALL:
		  unmake_wall(&bloc, wl);
		  break;
	}

	/* Show the change on the screen */
	if (real_map[bloc.x][bloc.y].flags & wl)
		show_wall(&bloc, wl);
	else
		unshow_wall(&bloc, wl);
}

/*
** Clears the maze, the name, the description, and the type.
*/
static void
unmake_maze(void)
{
	static Box empty_box =
	{0, NORMAL, 0};
	int i, j;

	for (i = 0; i < GRID_WIDTH; ++i)
		for (j = 0; j < GRID_HEIGHT; ++j)
			real_map[i][j] = empty_box;
	clear_mdesc(&design_mdesc);
}

/*
** Asks user to choose a box inside the maze.
** If successful, prompts user for information about maze, and then
** saves the maze description, puts it on the mdesc list, and makes it the
** maze in the settings.
*/
static void
mdesign_save(void)
{
	Game type;
	char name[80], desc[256];

	if (figure_insideness() == FALSE)
		mprint("Your maze is not completely surrounded.", 0, INPUT_Y);
	else {
		/* Prompt the user for the type, name, and description of the maze */
		input_string(ANIM_WIN, "Enter maze name:", name,
					 0, INPUT_Y, MD_FONT, 256);
		if (name[0] != '\0') {
			mprint("1) Combat  2) War  3) Ultimate  4) Capture  5) Race",
				   0, INPUT_Y + 1);
			type = (Game) (input_int(ANIM_WIN, "Enter maze type", 0,
									 INPUT_Y + 2, 1, 1, 5, MD_FONT) - 1);
			input_string(ANIM_WIN, "Enter maze description:", desc,
						 0, INPUT_Y + 3, MD_FONT, 256);

			build_mdesc(&design_mdesc, type, name, term->player_name,
						desc);
			display_mdesc_info(&design_mdesc);
			mdesign_clear_input();

			if (save_mdesc(&design_mdesc) == DESC_SAVED) {
				mprint("Maze saved.", 0, INPUT_Y);
				interface_set_desc(MDESC, name);
			} else
				mprint("Maze name already used.  Try another.", 0, INPUT_Y);
		}
	}
	mprint("Any key or button to return", 0, INPUT_Y + 1 /* 2 */ );
	wait_input();
	mdesign_clear_input();
}

/*
** Prompts user for name of maze and tries to load it.
*/
static void
mdesign_load(void)
{
	char name[80];

	input_string(ANIM_WIN, "Enter maze name:", name, 0, INPUT_Y, MD_FONT, 256);
	if (name[0] != '\0') {
		if (load_mdesc(&design_mdesc, name) == DESC_LOADED) {
			mprint("Maze loaded.", 0, INPUT_Y + 1);
			make_maze(&design_mdesc);
		} else
			mprint("Error.  Maze not loaded.", 0, INPUT_Y + 1);
	}
	mprint("Any key or button to return", 0, INPUT_Y + 3);
	wait_input();
	mdesign_clear_input();
}

static void
make_landmark(BoxC *loc, LandmarkType type)
{
	if (!check_box(loc))
		return;

	real_map[loc->x][loc->y].type = type;
	if (type == NORMAL)
		show_box(loc);
	else
		show_landmark(loc);
}

static void
show_landmark(BoxC *loc)
{
	extern Object *landmark_obj[];
	Picture *pic;
	PixC ploc;
	LandmarkType type;

	box_to_pix(loc, &ploc);

	type = real_map[loc->x][loc->y].type;
	pic = &landmark_obj[2]->pic[(int) type];
	draw_picture(ANIM_WIN, ploc.x, ploc.y, pic, DRAW_COPY, WHITE);
}

static void
set_team(BoxC *loc, int teamnum)
{
	if (!check_box(loc))
		return;

	real_map[loc->x][loc->y].team = teamnum;
	show_box(loc);
}

static void
set_teleport_code(BoxC *loc)
{
	int code;

	/* Check that this location is inside the maze area. */
	if (!check_box(loc))
		return;

	/* If it was already a teleport, the default code is the current code,
   * otherwise default to 1.
   */

	code = (real_map[loc->x][loc->y].type == TELEPORT) ?
	  real_map[loc->x][loc->y].teleport_code :
	  1;

	code = input_int(ANIM_WIN, "Code number", 0, INPUT_Y, code, 1, 255, MD_FONT);
	real_map[loc->x][loc->y].teleport_code = code;
	mdesign_clear_input();
}

/*
** Shows team only if non-zero. Shows only north and west walls.
*/
static void
show_box_fast(BoxC *loc)
{
	extern Object *random_obj[];
	Box *b;
	Picture *pic;
	PixC ploc;

	b = &real_map[loc->x][loc->y];

	if (b->team != 0) {
		box_to_pix(loc, &ploc);
		if (color_display()) {
			/* HACK.  Draw overlapping rectangles so that I don't draw over
	       the gridpoints. */
			draw_filled_rect(ANIM_WIN, ploc.x + 2, ploc.y, DES_BOX_WIDTH - 3,
							 DES_BOX_HEIGHT, DRAW_COPY, team_color[b->team]);
			draw_filled_rect(ANIM_WIN, ploc.x, ploc.y + 2, DES_BOX_WIDTH,
							 DES_BOX_HEIGHT, DRAW_COPY, team_color[b->team]);
		} else {
			pic = &random_obj[TEAM_OBJ]->pic[b->team];
			draw_picture(ANIM_WIN, ploc.x, ploc.y, pic, DRAW_COPY, WHITE);
		}
	}
	if (b->type != NORMAL)
		show_landmark(loc);
	if (b->flags & NORTH_WALL)
		show_wall(loc, NORTH_WALL);
	if (b->flags & WEST_WALL)
		show_wall(loc, WEST_WALL);
}

/*
** Shows the team, landmark, and all four walls of a box.
*/
static void
show_box(BoxC *loc)
{
	extern Object *random_obj[];
	Picture *pic;
	Team tm;
	LandmarkType type;
	PixC ploc;
	int color;

	box_to_pix(loc, &ploc);
	tm = real_map[loc->x][loc->y].team;
	type = real_map[loc->x][loc->y].type;

	if (color_display()) {
		/* HACK.  Draw overlapping rectangles so that I don't draw over the
	   gridpoints. */
		if (tm == 0)
			color = BLACK;
		else
			color = team_color[tm];
		draw_filled_rect(ANIM_WIN, ploc.x + 2, ploc.y, DES_BOX_WIDTH - 3,
						 DES_BOX_HEIGHT, DRAW_COPY, color);
		draw_filled_rect(ANIM_WIN, ploc.x, ploc.y + 2, DES_BOX_WIDTH,
						 DES_BOX_HEIGHT - 3, DRAW_COPY, color);
	} else {
		pic = &random_obj[TEAM_OBJ]->pic[tm];
		draw_picture(ANIM_WIN, ploc.x, ploc.y, pic, DRAW_COPY, WHITE);
	}
	if (type != NORMAL)
		show_landmark(loc);
	show_surrounding_walls(loc);
}

/*
** Redraws walls around a square that have been covered up by the team pattern.
*/
static void
show_surrounding_walls(BoxC *loc)
{
	BoxC iloc;

	iloc = *loc;

	if (real_map[iloc.x][iloc.y].flags & NORTH_WALL)
		show_wall(&iloc, NORTH_WALL);
	if (real_map[iloc.x][iloc.y].flags & WEST_WALL)
		show_wall(&iloc, WEST_WALL);

	iloc.x++;
	if (real_map[iloc.x][iloc.y].flags & WEST_WALL)
		show_wall(&iloc, WEST_WALL);

	iloc.y++;
	iloc.x--;
	if (real_map[iloc.x][iloc.y].flags & NORTH_WALL)
		show_wall(&iloc, NORTH_WALL);
}

static void
move_area(void)
{
	BoxC vertices[2];
	Box temp_maze[GRID_WIDTH][GRID_HEIGHT];
	BoxC new_start;

	select_area(vertices);
	read_area(vertices, temp_maze);
	place_area_request(&new_start);
	kill_area(vertices);
	put_area(vertices, temp_maze, &new_start);
}

static void
copy_area(void)
{
	BoxC vertices[2];
	Box temp_maze[GRID_WIDTH][GRID_HEIGHT];
	BoxC new_start;

	select_area(vertices);
	read_area(vertices, temp_maze);
	place_area_request(&new_start);
	put_area(vertices, temp_maze, &new_start);
}

static void
erase_area(void)
{
	BoxC vertices[2];

	select_area(vertices);
	kill_area(vertices);
}

static void
place_area_request(BoxC *loc)
{
	PixC ploc;
	int num_events;
	Event event;

	mprint("Click where you want to move it", 0, INPUT_Y);
	set_cursor(UL_CURSOR);

	do {
		num_events = 1;
		get_events(&num_events, &event);
	} while (num_events == 0 ||
			 (event.type != EVENT_LBUTTON &&
			  event.type != EVENT_MBUTTON &&
			  event.type != EVENT_RBUTTON));

	ploc.x = event.x;
	ploc.y = event.y;
	pix_to_box(&ploc, loc);

	set_cursor(PLUS_CURSOR);
	mdesign_clear_input();
}

static void
read_area(BoxC *vertices, Box temp_maze[GRID_WIDTH][GRID_HEIGHT])
{
	int i, j;

	for (i = vertices[0].x; i < vertices[1].x + 1; ++i)
		for (j = vertices[0].y; j < vertices[1].y + 1; ++j)
			temp_maze[i][j] = real_map[i][j];
}

static void
kill_area(BoxC *vertices)
{
	int i, j;
	BoxC loc;

	for (i = vertices[0].x; i <= vertices[1].x; ++i)
		for (j = vertices[0].y; j <= vertices[1].y; ++j) {
			loc.x = i;
			loc.y = j;
			if (j != vertices[1].y) {
				unmake_wall(&loc, WEST_WALL);
				unshow_wall(&loc, WEST_WALL);
			}
			if (i != vertices[1].x) {
				unmake_wall(&loc, NORTH_WALL);
				unshow_wall(&loc, NORTH_WALL);
			}
			if (i != vertices[1].x && j != vertices[1].y) {
				real_map[i][j].flags = 0;
				real_map[i][j].type = NORMAL;
				real_map[i][j].team = 0;
				show_box(&loc);
			}
		}
}

static void
put_area(BoxC *old_vertices, Box temp_maze[GRID_WIDTH][GRID_HEIGHT], BoxC *new_start)
{
	int i, j;
	int maxi, maxj;
	BoxC loc;

	maxi = MIN(old_vertices[1].x,
			   GRID_WIDTH - PAD - new_start->x + old_vertices[0].x);
	maxj = MIN(old_vertices[1].y,
			   GRID_HEIGHT - PAD - new_start->y + old_vertices[0].y);

	for (i = old_vertices[0].x; i <= maxi; ++i)
		for (j = old_vertices[0].y; j <= maxj; ++j) {
			loc.x = i - old_vertices[0].x + new_start->x;
			loc.y = j - old_vertices[0].y + new_start->y;
			if (i != maxi) {
				if (temp_maze[i][j].flags & NORTH_WALL) {
					if (temp_maze[i][j].flags & NORTH_DEST)
						make_destructible(&loc, NORTH_WALL);
					else
						make_wall(&loc, NORTH_WALL);
					show_wall(&loc, NORTH_WALL);
				} else {
					unmake_wall(&loc, NORTH_WALL);
					unshow_wall(&loc, NORTH_WALL);
				}
			}
			if (j != maxj) {
				if (temp_maze[i][j].flags & WEST_WALL) {
					if (temp_maze[i][j].flags & WEST_DEST)
						make_destructible(&loc, WEST_WALL);
					else
						make_wall(&loc, WEST_WALL);
					show_wall(&loc, WEST_WALL);
				} else {
					unmake_wall(&loc, WEST_WALL);
					unshow_wall(&loc, WEST_WALL);
				}
			}
			if (i != maxi && j != maxj) {
				real_map[loc.x][loc.y] = temp_maze[i][j];
				show_box(&loc);
			}
		}
}

/*
** Returns an array of BoxC of length 2.
**   [0] is the upper left corner
**   [1] is the lower right corner
*/
static void
select_area(BoxC *bvertices)
{
	Event event;
	int num_events;
	PixC pvertices[2];
	PixC oldvertex;

	mprint("Drag the mouse over the area", 0, INPUT_Y);
	set_cursor(UL_CURSOR);

	/* wait for him to press at the upper left corner */
	do {
		num_events = 1;
		get_events(&num_events, &event);
	} while (num_events == 0 ||
			 (event.type != EVENT_LBUTTON &&
			  event.type != EVENT_MBUTTON &&
			  event.type != EVENT_RBUTTON));
	pvertices[0].x = event.x;
	pvertices[0].y = event.y;

	/* move it NW to the nearest grid intersection */
	pix_to_box(&pvertices[0], &bvertices[0]);
	box_to_pix(&bvertices[0], &pvertices[0]);

	/* Make a rectangle to indicate what area he is selecting.  Its corners
       are also at grid intersections. */
	set_cursor(LR_CURSOR);
	oldvertex = pvertices[1] = pvertices[0];
	pix_to_box(&pvertices[1], &bvertices[1]);
	xor_rectangle(&pvertices[0], &pvertices[0]);

#ifndef X11
	follow_mouse(ANIM_WIN, TRUE);
	button_up(ANIM_WIN, TRUE);
#endif

	while (TRUE) {
		num_events = 1;
		get_events(&num_events, &event);
		if (num_events == 0)
			continue;
		if (event.type == EVENT_MOVED) {
			pvertices[1].x = event.x;
			pvertices[1].y = event.y;

			/* If the location is legal, move the rectangle to it */
			if (check_pixel_extra(&pvertices[1])) {
				/* Move the location to the nearest grid intersection */
				pix_to_box(&pvertices[1], &bvertices[1]);
				box_to_pix(&bvertices[1], &pvertices[1]);

				/* Move the rectangle on the screen */
				xor_rectangle(&pvertices[0], &oldvertex);
				xor_rectangle(&pvertices[0], &pvertices[1]);

				oldvertex = pvertices[1];
			}
		}
		if (event.type == EVENT_LBUTTONUP ||
			event.type == EVENT_MBUTTONUP ||
			event.type == EVENT_RBUTTONUP)
			break;
	}

#ifndef X11
	follow_mouse(ANIM_WIN, FALSE);
	button_up(ANIM_WIN, FALSE);
#endif

	/* Erase last remaining rectangle */
	xor_rectangle(&pvertices[0], &oldvertex);

	set_cursor(PLUS_CURSOR);
	mdesign_clear_input();
}

static void
xor_rectangle(PixC *start, PixC *end)
{
	int width, height;

	width = end->x - start->x;
	height = end->y - start->y;
	draw_rect(ANIM_WIN, start->x, start->y, width, height, DRAW_XOR, WHITE);
}

/*
** Figures out what boxes are inside the maze.  Treats destructible
** walls as if they are not there.
** Returns FALSE if the maze extends beyond the allowable area.
*/
static int
figure_insideness(void)
{
	int i, j;
	BoxC boxes[GRID_WIDTH * GRID_HEIGHT];
	BoxC temp;
	int size = 0;
	unsigned int flags = 0;

	for (i = 0; i < GRID_WIDTH; ++i)
		for (j = 0; j < GRID_HEIGHT; ++j)
			real_map[i][j].flags &= ~INSIDE_MAZE;

	for (i = 0; i < GRID_WIDTH; ++i)
		for (j = 0; j < GRID_HEIGHT; ++j)
			if (real_map[i][j].type == TELEPORT) {
				add_to_maze(i, j, boxes, &size);
				flags = 1;
			}
	if (flags == 0) {
		get_inside_spot(&temp);
		i = temp.x;
		j = temp.y;
		add_to_maze(i, j, boxes, &size);
	} else
		flags = 0;

	while (size) {
		temp = boxes[--size];
		i = temp.x;
		j = temp.y;

		flags = real_map[i][j].flags;
		if (!(flags & WEST_WALL) || flags & WEST_DEST) {
			if (i <= PAD)
				return FALSE;
			else
				add_to_maze(i - 1, j, boxes, &size);
		}
		if (!(flags & NORTH_WALL) || flags & NORTH_DEST) {
			if (j <= PAD)
				return FALSE;
			else
				add_to_maze(i, j - 1, boxes, &size);
		}
		flags = real_map[i + 1][j].flags;
		if (!(flags & WEST_WALL) || flags & WEST_DEST) {
			if (i >= GRID_WIDTH - PAD)
				return FALSE;
			else
				add_to_maze(i + 1, j, boxes, &size);
		}
		flags = real_map[i][j + 1].flags;
		if (!(flags & NORTH_WALL) || flags & NORTH_DEST) {
			if (j >= GRID_HEIGHT - PAD)
				return FALSE;
			else
				add_to_maze(i, j + 1, boxes, &size);
		}
	}

	return TRUE;
}

/*
** Prompts user to click inside the maze, and sets the bloc to it.
*/
static void
get_inside_spot(BoxC *bloc)
{
	Event event;
	int num_events;
	PixC ploc;
	Boolean finished = FALSE;

	mprint("Click in a box inside the maze", 0, INPUT_Y);
	do {
		num_events = 1;
		get_events(&num_events, &event);
		if (num_events != 0 && (event.type == EVENT_LBUTTON ||
								event.type == EVENT_MBUTTON ||
								event.type == EVENT_RBUTTON)) {
			ploc.x = event.x;
			ploc.y = event.y;
			if (check_pixel(&ploc))
				finished = TRUE;
		}
	} while (finished == FALSE);

	pix_to_box(&ploc, bloc);
	mdesign_clear_input();
}

/*
** Puts the box at (x,y) into the maze and adds it to the boxes list
** if it wasn't already in the maze.
*/
static void
add_to_maze(int x, int y, BoxC *boxes, int *size)
{
	BoxC temp;

	if (!(real_map[x][y].flags & INSIDE_MAZE)) {
		real_map[x][y].flags |= INSIDE_MAZE;
		temp.x = x;
		temp.y = y;
		boxes[(*size)++] = temp;
	}
}

/*
** Returns TRUE if pixel location is inside maze area, otherwise FALSE.
*/
static int
check_pixel(PixC *loc)
{
	if ((loc->x < X_OFFSET + PAD * DES_BOX_WIDTH) ||
		(loc->x > X_OFFSET + (GRID_WIDTH - PAD) * DES_BOX_WIDTH) ||
		(loc->y < Y_OFFSET + PAD * DES_BOX_HEIGHT) ||
		(loc->y > Y_OFFSET + (GRID_HEIGHT - PAD) * DES_BOX_HEIGHT))
		return FALSE;
	else
		return TRUE;
}

/*
** Returns TRUE if pixel location is inside maze area or on right or bottom
** border, otherwise FALSE.
*/
static int
check_pixel_extra(PixC *loc)
{
	if ((loc->x < X_OFFSET + PAD * DES_BOX_WIDTH) ||
		(loc->x > X_OFFSET + (GRID_WIDTH - PAD) * DES_BOX_WIDTH + DES_BOX_WIDTH / 2) ||
		(loc->y < Y_OFFSET + PAD * DES_BOX_HEIGHT) ||
		(loc->y > Y_OFFSET + (GRID_HEIGHT - PAD) * DES_BOX_HEIGHT + DES_BOX_HEIGHT / 2))
		return FALSE;
	else
		return TRUE;
}

/*
** Returns TRUE if box location is inside maze area, otherwise FALSE.
*/
static int
check_box(BoxC *loc)
{
	if (loc->x < PAD || loc->x >= GRID_WIDTH - PAD ||
		loc->y < PAD || loc->y >= GRID_HEIGHT - PAD)
		return FALSE;
	else
		return TRUE;
}

/*
** Converts grid coordinates to pixel coordinates.
*/
static void
box_to_pix(BoxC *bloc, PixC *ploc)
{
	ploc->x = bloc->x * DES_BOX_WIDTH + X_OFFSET;
	ploc->y = bloc->y * DES_BOX_HEIGHT + Y_OFFSET;
}

/*
** Converts pixel coordinates to grid coordinates.
*/
static void
pix_to_box(PixC *ploc, BoxC *bloc)
{
	bloc->x = (ploc->x - X_OFFSET) / DES_BOX_WIDTH;
	bloc->y = (ploc->y - Y_OFFSET) / DES_BOX_HEIGHT;
}

/*
** Checks to see if the wall is modifiable.
*/
static int
check_wall(BoxC *loc, Wall wl)
{
	/* First handle special case walls along right and bottom edge */
	if (((loc->x == GRID_WIDTH - PAD) && (loc->y >= PAD) &&
		 (loc->y < GRID_HEIGHT - PAD) && (wl == WEST_WALL)) ||
		((loc->y == GRID_HEIGHT - PAD) && (loc->x >= PAD) &&
		 (loc->x < GRID_WIDTH - PAD) && (wl == NORTH_WALL)))
		return TRUE;
	else
		return check_box(loc);
}

static void
make_wall(BoxC *loc, Wall wl)
{
	real_map[loc->x][loc->y].flags |= wl;
	real_map[loc->x][loc->y].flags &= ~(wl << 2);
}

static void
make_destructible(BoxC *loc, Wall wl)
{
	real_map[loc->x][loc->y].flags |= wl;
	real_map[loc->x][loc->y].flags |= wl << 2;
}

static void
unmake_wall(BoxC *loc, Wall wl)
{
	real_map[loc->x][loc->y].flags &= ~wl;
}

static void
show_wall(BoxC *loc, Wall wl)
{
	draw_wall(loc, wl, WHITE);
}

static void
unshow_wall(BoxC *loc, Wall wl)
{
	BoxC iloc;

	/* Make a copy of the location, so we can frob with it */
	iloc = *loc;

	draw_wall(loc, wl, BLACK);

	/* We might have covered the edge of a team pattern with black, so we
       redraw any necessary patterns */
	if (real_map[iloc.x][iloc.y].team)
		show_box(&iloc);

	if (wl == NORTH_WALL) {
		iloc.y--;
		if (real_map[iloc.x][iloc.y].team)
			show_box(&iloc);
	}
	if (wl == WEST_WALL) {
		iloc.x--;
		if (real_map[iloc.x][iloc.y].team)
			show_box(&iloc);
	}
}

static void
draw_wall(BoxC *loc, Wall wl, int color)
{
	PixC ploc;

	box_to_pix(loc, &ploc);

	switch (wl) {
	  case NORTH_WALL:
		  draw_filled_rect(ANIM_WIN, ploc.x + 2, ploc.y - 1,
						   DES_BOX_WIDTH - 3, 3, DRAW_COPY, color);
		  if (real_map[loc->x][loc->y].flags & NORTH_DEST)	/* destructible */
			  draw_filled_rect(ANIM_WIN, ploc.x + 2, ploc.y,
							   DES_BOX_WIDTH - 3, 1, DRAW_COPY, BLACK);
		  break;
	  case WEST_WALL:
		  draw_filled_rect(ANIM_WIN, ploc.x - 1, ploc.y + 2, 3,
						   DES_BOX_HEIGHT - 3, DRAW_COPY, color);
		  if (real_map[loc->x][loc->y].flags & WEST_DEST)	/* destructible */
			  draw_filled_rect(ANIM_WIN, ploc.x, ploc.y + 2, 1,
							   DES_BOX_HEIGHT - 3, DRAW_COPY, BLACK);
		  break;
	}
}

/*
** Draw the intersections of walls.
*/
static void
show_dot(BoxC *loc)
{
	PixC ploc;

	box_to_pix(loc, &ploc);
	draw_filled_rect(ANIM_WIN, ploc.x - 1, ploc.y - 1, 3, 3, DRAW_COPY, WHITE);
}
