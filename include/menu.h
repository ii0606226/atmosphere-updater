/*
 * atmosphere-updater - App that allows you to update Atmosphere (& sigpatches) directly on your switch
 * Copyright (C) 2020 eXhumer
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _MENU_H_
#define _MENU_H_

#include "sdl.h"

#define CURSOR_LIST_MAX		4
#define HIGHLIGHT_BOX_MIN	20
#define HIGHLIGHT_BOX_MAX	70
#define NEWLINE				110
#define FIRST_LINE			130
#define MAX_STRLEN			512

void refreshScreen(char loaded);
void printOptionList(int cursor);
void popUpBox(TTF_Font *font, int x, int y, SDL_Colour colour, char *text);
int yesNoBox(int cursor, int x, int y, char *question);
void errorBox(int x, int y, char *errorText);

#endif