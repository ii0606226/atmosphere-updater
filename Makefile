#=====================================================================================================
# atmosphere-updater - App that allows you to update Atmosphere (& sigpatches) directly on your switch
# Copyright (C) 2020 eXhumer
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#=====================================================================================================

#=======================================================#
# Check on whether user is competent enough to continue #
#=======================================================#
ifndef DEVKITPRO
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
else
include $(DEVKITPRO)/libnx/switch_rules
endif

#======================================#
# Target Application Build Information #
#======================================#
APP_TITLE		:=	Atmosphere Updater
APP_AUTHOR		:=	$(shell whoami)
VERSION_MAJOR	:=	0
VERSION_MINOR	:=	7
VERSION_PATCH	:=	0
APP_VERSION		:=	$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

#===================================================#
# Variables holding various paths used during build #
#===================================================#
TARGET					:=	atmosphere-updater
BUILD					:=	build
PROGRAM_EXEFS_SOURCE	:=	source
FORWARDER				:=	forwarder
FORWARDER_EXEFS_SOURCE	:=	$(FORWARDER)/source
INCLUDE					:=	include
OBJS					:=	objs
DEPS					:=	deps
ROMFS					:=	romfs

#=======================================#
# Path to all libraries used in project #
#=======================================#
LIBDIRS	:=	$(PORTLIBS)\
			$(LIBNX)

#=====================================================================#
# Code Generation Options                                             #
# **NOTE**: DO NOT CHANGE ANYTHING UNLESS YOU KNOW WHAT YOU ARE DOING #
#=====================================================================#
ARCHFLAGS	:=	-march=armv8-a+crc+crypto\
				-mtune=cortex-a57\
				-mtp=soft\
				-fPIE

DEFFLAGS	:=	-D__SWITCH__\
				-DVERSION_MAJOR=VERSION_MAJOR\
				-DVERSION_MINOR=VERSION_MINOR\
				-DVERSION_PATCH=VERSION_PATCH\
				-DAPP_VERSION=\"$(APP_VERSION)\"

INCFLAG	:=	-I$(INCLUDE)\
			$(foreach dir,$(LIBDIRS),-I$(dir)/include)

CFLAGS	:=	$(shell freetype-config --cflags)\
			$(shell sdl2-config --cflags)\
			$(ARCHFLAGS)\
			$(DEFFLAGS)\
			$(INCFLAG)\
			-Wall

CXXFLAGS	:=	-fno-rtti\
				-fno-exceptions\
				-std=gnu++2a

ASFLAGS	:=	$(ARCHFLAGS)

LDFLAGS	:=	$(ARCHFLAGS)\
			-specs=$(DEVKITPRO)/libnx/switch.specs

LIBS	:=	$(shell sdl2-config --libs)\
			$(shell freetype-config --libs)\
			-lSDL2_gfx\
			-lSDL2_image\
			-lSDL2_ttf\
			-lpng\
			-lturbojpeg\
			-lwebp\
			-lcurl\
			-lmbedtls
			-lmbedx509\
			-lmbedcrypto\
			-lminizip\
			-lz\
			-lnx

#============================================================#
# Target specific defines to optimize target for its usecase #
#============================================================#
release	:	RELTYPE		:=	release
release	:	CFLAGS		+=	-ffunction-sections -fdata-sections
release	:	CFLAGS		+=	-O3
debug	:	RELTYPE		:=	debug
debug	:	DEFFLAGS	+=	-DDEBUG
debug	:	CFLAGS		+=	-O0
debug	:	ASFLAGS		+=	-g
debug	:	LDFLAGS		+=	-Wl,-Map,$(BUILD)/$(OUTPUT)-$(RELTYPE).map
debug	:	CFLAGS		+=	-g