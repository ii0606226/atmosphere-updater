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

#include <stdio.h>
#include <minizip/unzip.h>
#include <string.h>
#include <dirent.h>
#include <switch.h>

#include "util.h"
#include "unzip.h"
#include "menu.h"

#define WRITEBUFFERSIZE	500000 // 500KB
#define MAXFILENAME		256

int unzip(const char *output, int cursor)
{
	unzFile zfile = unzOpen(output);
	unz_global_info gi;
	unzGetGlobalInfo(zfile, &gi);

	for (int i = 0; i < gi.number_entry; i++)
	{
		printOptionList(cursor);
		popUpBox(appFonts.fntSmall, 350, 250, SDL_GetColour(white), "Unzipping...");

		char filename_inzip[MAXFILENAME];
		unz_file_info file_info;

		unzOpenCurrentFile(zfile);
		unzGetCurrentFileInfo(zfile, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);

		if (strstr(filename_inzip, "/config/BCT.ini"))
		{
			FILE *f = fopen(filename_inzip, "r");
			if (f)
			{
				if(yesNoBox(cursor, 390, 250, "Overwrite BCT.ini?") == NO)
				{
					fclose(f);
					goto jump_to_end;
				}
			}
			fclose(f);
		}

		if ((filename_inzip[strlen(filename_inzip) - 1]) == '/')
		{
			DIR *dir = opendir(filename_inzip);
			if (dir) closedir(dir);
			else
			{
				drawText(appFonts.fntSmall, 350, 350, SDL_GetColour(white), filename_inzip);
				mkdir(filename_inzip, 0777);
			}
		}

		else
		{
			const char *write_filename = filename_inzip;
			void *buf = malloc(WRITEBUFFERSIZE);

			FILE *outfile = fopen(write_filename, "wb");

			drawText(appFonts.fntSmall, 350, 350, SDL_GetColour(white), write_filename);

			for (int j = unzReadCurrentFile(zfile, buf, WRITEBUFFERSIZE); j > 0; j = unzReadCurrentFile(zfile, buf, WRITEBUFFERSIZE))
				fwrite(buf, 1, j, outfile);

			fclose(outfile);
			free(buf);
		}

		updateRenderer();

		jump_to_end: // goto
		unzCloseCurrentFile(zfile);
		unzGoToNextFile(zfile);
	}

	unzClose(zfile);
	remove(output);
	return 0;
}
