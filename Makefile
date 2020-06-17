ifndef DEVKITPRO
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
else
include $(DEVKITPRO)/libnx/switch_rules
endif

VERSION_MAJOR	:=	0
VERSION_MINOR	:=	6
VERSION_PATCH	:=	2

APP_TITLE	:=	Atmosphere Updater
APP_AUTHOR	:=	$(shell whoami)
APP_VERSION	:=	$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

TARGET	:=	atmosphere-updater
BUILD	:=	build
SOURCE	:=	source
INCLUDE	:=	include
ROMFS	:=	romfs

LIBDIRS	:=	$(PORTLIBS) $(LIBNX)

ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

DEFINES	:=	-D__SWITCH__

CFLAGS	:=	-Wall\
			-O3\
			-ffunction-sections \
			$(shell freetype-config --cflags)\
			$(shell sdl2-config --cflags)\
			$(ARCH)\
			$(DEFINES)

CXXFLAGS	:=	-fno-rtti\
				-fno-exceptions

ASFLAGS	:=	$(ARCH)

LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs

LIBS	:=	-lSDL2_ttf\
			-lSDL2_image\
			-lwebp\
			-lpng\
			-lturbojpeg\
			-lSDL2_gfx\
			$(shell sdl2-config --libs)\
			$(shell freetype-config --libs)\
			-lcurl\
			-lmbedtls\
			-lmbedx509\
			-lmbedcrypto\
			-lminizip\
			-lz\
			-lnx

ifeq ($(strip $(CPPFILES)),)
	LD	:=	$(CXX)
else
	LD	:=	$(CXX)
endif

DEPSDIR	:=	$(BUILD)/deps
OBJSDIR	:=	$(BUILD)/objs

CFILES		:=	$(wildcard $(SOURCE)/*.c) $(wildcard $(SOURCE)/*/*.c)
CPPFILES	:=	$(wildcard $(SOURCE)/*.cpp) $(wildcard $(SOURCE)/*/*.cpp)
OFILES		:=	$(foreach CPPFILE,$(CPPFILES),$(patsubst $(SOURCE)/%.cpp,$(OBJSDIR)/%.o,$(CPPFILE)))\
				$(foreach CFILE,$(CFILES),$(patsubst $(SOURCE)/%.c,$(OBJSDIR)/%.o,$(CFILE)))
DEPENDS		:=	$(foreach OFILE,$(OFILES),$(patsubst $(OBJSDIR)/%.o,$(DEPSDIR)/%.d,$(OFILE)))
INCFLAGS	:=	$(foreach dir,$(LIBDIRS),-I$(dir)/include) -I$(INCLUDE)
LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

APP_ICON	:=	icon.jpg

OUTPUT	:=	$(BUILD)/$(TARGET)

NACP_OUTPUT	:=	$(OUTPUT).nacp

NRO_OUTPUT	:=	$(OUTPUT).nro

NSO_OUTPUT	:=	$(OUTPUT).nso

ELF_OUTPUT	:=	$(OUTPUT).elf

NROFLAGS	+=	--nacp=$(NACP_OUTPUT) --romfsdir=$(ROMFS) --icon=$(APP_ICON)

.PHONY: $(BUILD) $(OBJSDIR) $(DEPSDIR) release nso clean

release:	$(NRO_OUTPUT)
nso:	$(NSO_OUTPUT)
$(NRO_OUTPUT):	$(BUILD) $(ELF_OUTPUT) $(NACP_OUTPUT)
$(ELF_OUTPUT):	$(OBJSDIR) $(DEPSDIR) $(OFILES)

$(BUILD) $(OBJSDIR) $(DEPSDIR):
	[ -d $@ ] || mkdir -p $@

$(OBJSDIR)/%.o:	$(SOURCE)/%.cpp
	$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) $(INCFLAGS) $(CXXFLAGS) -c $< -o $@ $(ERROR_FILTER)

$(OBJSDIR)/%.o:	$(SOURCE)/%.c
	$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) $(INCFLAGS) -c $< -o $@ $(ERROR_FILTER)

$(NSO_OUTPUT): $(ELF_OUTPUT)
	elf2nso $< $@

$(ELF_OUTPUT):
	@$(LD) $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@

clean:
	rm -f $(NRO_OUTPUT) $(NACP_OUTPUT) $(ELF_OUTPUT) $(NSO_OUTPUT)

-include $(DEPENDS)