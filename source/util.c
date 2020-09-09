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
#include <string.h>
#include <switch.h>

#include "util.h"
#include "menu.h"
#include "unzip.h"
#include "download.h"
#include "reboot_payload.h"

#define TEMP_FILE				"/switch/atmosphere-updater/temp"
#define FILTER_STRING			"browser_download_url\":\""
#define VERSION_FILTER_STRING	"tag_name\":\""

char g_sysVersion[50];
char g_amsVersion[50];
char g_amsVersionWithoutHash[15];
char g_latestAtmosphereVersion[50];
char g_supportedAtmosphereVersion[50];

char *getSysVersion()
{
	return g_sysVersion;
}

char *getAmsVersion()
{
	return g_amsVersion;
}

char *getLatestAtmosphereVersion()
{
	return g_latestAtmosphereVersion;
}

char *getSupportedAtmosphereVersion()
{
	return g_supportedAtmosphereVersion;
}

void writeSysVersion()
{
	Result ret = 0;
	SetSysFirmwareVersion ver;

	if (R_FAILED(ret = setsysGetFirmwareVersion(&ver)))
	{
		printf("GetFirmwareVersion() failed: 0x%x.\n\n", ret);
		return;
	}

	char sysVersionBuffer[20];
	snprintf(sysVersionBuffer, 20, "%u.%u.%u", ver.major, ver.minor, ver.micro);
	snprintf(g_sysVersion, sizeof(g_sysVersion), "Firmware Ver: %s", sysVersionBuffer);

#ifdef __DEBUG__
	printf("sysVersionBuffer: %s\n", sysVersionBuffer);
	printf("g_sysVersion: %s\n", g_sysVersion);
#endif
}

void writeAmsVersion()
{
	Result ret = 0;
	u64 ver;
	u64 fullHash;
	SplConfigItem SplConfigItem_ExosphereVersion = (SplConfigItem)65000;
	SplConfigItem SplConfigItem_ExosphereVerHash = (SplConfigItem)65003;

	if (R_FAILED(ret = splGetConfig(SplConfigItem_ExosphereVersion, &ver)))
	{
		printf("SplConfigItem_ExosphereVersion() failed: 0x%x.\n\n", ret);
		return;
	}

	if (R_FAILED(ret = splGetConfig(SplConfigItem_ExosphereVerHash, &fullHash)))
	{
		printf("SplConfigItem_ExosphereVerHash() failed: 0x%x.\n\n", ret);
		return;
	}

	char shortHash[8];
	snprintf(shortHash, sizeof(shortHash), "%lx", fullHash);

	char amsVersionNum[25];
	snprintf(g_amsVersionWithoutHash, sizeof(g_amsVersionWithoutHash), "%lu.%lu.%lu", (ver >> 56) & 0xFF,  (ver >> 48) & 0xFF, (ver >> 40) & 0xFF);
	snprintf(amsVersionNum, sizeof(amsVersionNum), "%s (%s)", g_amsVersionWithoutHash, shortHash);
	snprintf(g_amsVersion, sizeof(g_amsVersion), "Atmosphere Ver: %s", amsVersionNum);

#ifdef __DEBUG__
	printf("ver: %ld\n", ver);
	printf("fullHash: %ld\n", fullHash);
	printf("shortHash: %s\n", shortHash);
	printf("g_amsVersionWithoutHash: %s\n", g_amsVersionWithoutHash);
	printf("amsVersionNum: %s\n", amsVersionNum);
	printf("g_amsVersion: %s\n", g_amsVersion);
#endif
}

void writeLatestAtmosphereVersion()
{
	char *updateString = "- Up to date";
	if (!downloadFile(AMS_SUPPORTED_URL, TEMP_FILE, ON))
	{
		char supportedVersionNumber[20];

		FILE *fp = fopen(TEMP_FILE, "r");

		if (fp)
		{
			char c;
			int i = 0;
			while ((c = fgetc(fp)) != EOF)
			{
				if (c == '\n' || c == '\r')
					break;
				supportedVersionNumber[i] = c;
				i++;
			}
			supportedVersionNumber[i + 1] = '\0';
			fclose(fp);
		}
		snprintf(g_supportedAtmosphereVersion, sizeof(g_supportedAtmosphereVersion), supportedVersionNumber);
		if (strcmp(g_amsVersionWithoutHash, supportedVersionNumber) != 0)
		{
			char buffer[50];
			snprintf(buffer, sizeof(buffer), "- Latest supported: %s", supportedVersionNumber);
			updateString = buffer;
		}
	}
	snprintf(g_latestAtmosphereVersion, sizeof(g_latestAtmosphereVersion), updateString);
}

void copyFile(char *src, char *dest)
{
	FILE *srcfile = fopen(src, "rb");
	FILE *newfile = fopen(dest, "wb");

	if (srcfile && newfile)
	{
		char buffer[10000];	// 10kb per write, which is fast
		size_t bytes;		// size of the file to write (10kb or filesize max)

		while (0 < (bytes = fread(buffer, 1, sizeof(buffer), srcfile)))
		{
			fwrite(buffer, 1, bytes, newfile);
		}
	}
	fclose(srcfile);
	fclose(newfile);
}

int parseSearch(char *parse_string, char *filter, char* new_string)
{
	FILE *fp = fopen(parse_string, "r");

	if (fp)
	{
		char c;
		while ((c = fgetc(fp)) != EOF)
		{
			if (c == *filter)
			{
				for (int i = 0, len = strlen(filter) - 1; c == filter[i]; i++)
				{
					c = fgetc(fp);
					if (i == len)
					{
						for (int j = 0; c != '\"'; j++)
						{
							new_string[j] = c;
							new_string[j+1] = '\0';
							c = fgetc(fp);
						}
						fclose(fp);
						remove(parse_string);
						return 0;
					}
				}
			}
		}
	}

	errorBox(350, 250, "Failed to find parse url!");
	fclose(fp);
	return 1;
}

void update_ams()
{
	char new_url[MAX_STRLEN];
	snprintf(new_url, sizeof(new_url), AMS_URL_WITH_VERSION, g_supportedAtmosphereVersion);
	if (!downloadFile(new_url, TEMP_FILE, ON))
	{
		if (!parseSearch(TEMP_FILE, FILTER_STRING, new_url))
		{
			if (!downloadFile(new_url, AMS_OUTPUT, OFF))
			{
				unzip(AMS_OUTPUT, UP_AMS);
				remove(AMS_OUTPUT);
			}
		}
	}

	// ask if user wants to update sigpatches as well.
	if (yesNoBox(0, 390, 250, "Update sigpatches as well?") == YES)
	{
		update_sigpatches(UP_AMS);
	}
}

void update_hekate()
{
	if (!downloadFile(HEKATE_URL, TEMP_FILE, ON))
	{
		char new_url[MAX_STRLEN];
		if (!parseSearch(TEMP_FILE, FILTER_STRING, new_url))
		{
			if (!downloadFile(new_url, HEKATE_OUTPUT, OFF))
			{
				unzip(HEKATE_OUTPUT, UP_HEKATE);
				remove(HEKATE_OUTPUT);
			}
		}
	}
}

void update_sigpatches(int cursor)
{
	if (!downloadFile(PATCH_URL, PATCH_OUTPUT, OFF))
	{
		unzip(PATCH_OUTPUT, cursor);
		remove(PATCH_OUTPUT);
	}
}

void update_app()
{
	if (!downloadFile(APP_URL, TEMP_FILE, OFF))
	{
		remove(APP_OUTPUT);
		remove(OLD_APP_PATH);
		rename(TEMP_FILE, APP_OUTPUT);
		errorBox(400, 250, "      Update complete!\nRestart app to take effect");
	}
}
