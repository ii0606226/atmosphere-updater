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

#include <switch.h>

#include "menu.h"
#include "util.h"

int touch_cursor(int x, int y)
{
	int cursor = 0;

	for (int nl=0; cursor < (CURSOR_LIST_MAX+1); cursor++, nl+=NEWLINE)
		if (y > (FIRST_LINE + nl - HIGHLIGHT_BOX_MIN) && y < (FIRST_LINE + nl + NEWLINE - HIGHLIGHT_BOX_MIN))
			break;

	return cursor;
}

int touch_yes_no_option(int x, int y)
{
	if (x > 380 && x < 555 && y > 410 && y < 475)
		return NO;

	if (x > 700 && x < 890 && y > 410 && y < 475)
		return YES;

	return 1;
}