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
#include <unistd.h>
#include <dirent.h>
#include <switch.h>

#include "util.h"
#include "touch.h"
#include "menu.h"
#include "unzip.h"
#include "download.h"
#include "reboot_payload.h"

AppTextures appTextures;
AppFonts appFonts;
#ifdef __DEBUG__
int sockFD = 0;
#endif

int appInit()
{
	Result rc;

	if (R_FAILED(rc = socketInitializeDefault()))
		printf("socketInitializeDefault() failed: 0x%x.\n\n", rc);

#ifdef __DEBUG__
	sockFD = nxlinkStdio();
	if(sockFD < 0)
		printf("nxlinkStdio() failed: 0x%x.\n\n", sockFD);
#endif

	if (R_FAILED(rc = setsysInitialize()))
		printf("setsysInitialize() failed: 0x%x.\n\n", rc);

	if (R_FAILED(rc = splInitialize()))
		printf("splInitialize() failed: 0x%x.\n\n", rc);

	if (R_FAILED(rc = plInitialize(PlServiceType_User)))
		printf("plInitialize() failed: 0x%x.\n\n", rc);

	if (R_FAILED(rc = romfsInit()))
		printf("romfsInit() failed: 0x%x.\n\n", rc);

	sdlInit();

	romfsExit();

	return 0;
}

void appExit()
{
	sdlExit();
	socketExit();
	plExit();
	splExit();
	setsysExit();
#ifdef __DEBUG__
	if(sockFD > 0)
		close(sockFD);
#endif
}

int main(int argc, char **argv)
{
	appInit();
	mkdir(APP_PATH, 0777);
	chdir(ROOT);

	writeSysVersion();
	writeAmsVersion();
	refreshScreen(0);
	updateRenderer();
	writeLatestAtmosphereVersion();

	short cursor = 0;

	int touch_lock = OFF;
	u32 tch = 0;
	touchPosition touch;

	while(appletMainLoop())
	{
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		hidTouchRead(&touch, tch);
		u32 touch_count = hidTouchCount();

		printOptionList(cursor);

		if (kDown & KEY_DOWN)
		{
			if (cursor == CURSOR_LIST_MAX) cursor = 0;
			else cursor++;
		}

		if (kDown & KEY_UP)
		{
			if (cursor == 0) cursor = CURSOR_LIST_MAX;
			else cursor--;
		}

		if (kDown & KEY_A || (touch_lock == OFF && touch.px > 530 && touch.px < 1200 && touch.py > FIRST_LINE - HIGHLIGHT_BOX_MIN && touch.py < (NEWLINE * CURSOR_LIST_MAX) + FIRST_LINE + HIGHLIGHT_BOX_MAX))
		{
			if (touch_lock == OFF && touch_count > 0)
				cursor = touch_cursor(touch.px, touch.py);

			switch (cursor)
			{
			case UP_AMS:
				if (yesNoBox(cursor, 390, 250, "Update Atmosphere?") == YES)
					update_ams();
				break;

			case UP_HEKATE:
				if (yesNoBox(cursor, 390, 250, "Update Hekate?") == YES)
					update_hekate();
				break;

			case UP_PATCHES:
				if (yesNoBox(cursor, 390, 250, "Update Sigpatches?") == YES)
					update_sigpatches(cursor);
				break;

			case UP_APP:
				if (yesNoBox(cursor, 390, 250, "Update App?") == YES)
					update_app();
				break;

			case REBOOT_PAYLOAD:
				if (yesNoBox(cursor, 390, 250, "Reboot to Payload?") == YES)
					reboot_payload("/atmosphere/reboot_payload.bin");
				break;
			}
		}

		if (kDown & KEY_PLUS || (touch.px > 1145 && touch.px < SCREEN_W && touch.py > 675 && touch.py < SCREEN_H))
			break;

		if (touch_count > 0) touch_lock = ON;
		else touch_lock = OFF;

		updateRenderer();
	}

	appExit();
	return 0;
}
