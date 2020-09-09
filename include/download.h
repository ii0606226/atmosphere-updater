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

#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#define AMS_URL		"https://api.github.com/repos/Atmosphere-NX/Atmosphere/releases"
#define HEKATE_URL	"https://api.github.com/repos/CTCaer/hekate/releases"
#define PATCH_URL	"https://github.com/eXhumer/patches/releases/latest/download/sigpatches.zip"
#define APP_URL		"https://github.com/eXhumer/atmosphere-updater/releases/latest/download/atmosphere-updater.nro"
#define AMS_SUPPORTED_URL "https://raw.githubusercontent.com/ii0606226/atmoshere-supported/master/version"

int downloadFile(const char *url, const char *output, int api_mode);

#endif