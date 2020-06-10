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

#ifndef _UTIL_H_
#define _UTIL_H_

#define ROOT                    "/"
#define APP_PATH                "/switch/atmosphere-updater/"
#define AMS_OUTPUT              "/switch/atmosphere-updater/ams.zip"
#define HEKATE_OUTPUT           "/switch/atmosphere-updater/hekate.zip"
#define PATCH_OUTPUT            "/switch/atmosphere-updater/sigpatches.zip"
#define APP_OUTPUT              "/switch/atmosphere-updater/atmosphere-updater.nro"
#define OLD_APP_PATH            "/switch/atmosphere-updater.nro"

#define YES                     10
#define NO                      20
#define ON                      1
#define OFF                     0

void writeSysVersion();                                                 // writes the sys version.
void writeAmsVersion();                                                 // writes the ams version with hash.
void writeLatestAtmosphereVersion();                                    // writes the latest Atmosphere version.
char *getSysVersion();                                                  // returns sys version.
char *getAmsVersion();                                                  // returns ams version.
char *getLatestAtmosphereVersion();                                     // returns the latest Atmosphere version.

void copyFile(char *src, char *dest);                                   // basic copy file. Use malloc if you need dynamic mem
int parseSearch(char *phare_string, char *filter, char* new_string);    // hacky way to parse a file for a string
void update_ams();                                                      // update ams
void update_hekate();                                                   // update hekate
void update_sigpatches(int cursor);                                     // update sigpatches
void update_app();                                                      // update the app

#endif