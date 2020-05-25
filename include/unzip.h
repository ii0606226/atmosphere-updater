#ifndef _UNZIP_H_
#define _UNZIP_H_

#define UP_AMS          0
#define UP_HEKATE       1
#define UP_PATCHES      2
#define UP_APP          3
#define REBOOT_PAYLOAD  4

int unzip(const char *output, int cursor);

#endif