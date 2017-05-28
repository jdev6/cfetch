#include <sys/utsname.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#define int64_t __eeee_E_E /* justs so cpuid doesnt redefine int64_t... it does that for whartever reason*/
#include <libcpuid/libcpuid.h>
#undef int64_t
#include <X11/Xlib.h>
#include "ascii.h"

/* program stuff.*/
#define ORDER "kosdcgruR" //default order
#define MAX_INFOS 30
#define PROGRAM "cfetch"
#define VERSION "0.1"
#define HELP PROGRAM" "VERSION" (compiled "__DATE__"-"__TIME__")\n\
usage: "PROGRAM" [options] order\nwherein options are\n\
 -h show help\n\
 -v version\n\
 -c\"color code\" set things color for example red '"PROGRAM" -c\"[1;31m\"'\n\
 -a icon align: '-au' up, '-ad' down, '-al' left \n\
 -d\"distro\": for distro icon (see https://github.com/jdev6/cfetch/blob/master/ascii.h)\n\
order is the order for the things that are the information.. default one is '"ORDER"',and letters:\n\
k-kernel, o-os, s-shell, d-desktop, c-cpu, g-gpu, r-resolution, u-uptime, R-ram\n"

#ifdef DEBUG
	#define flog(...) do { fprintf(stderr, "[%s:%s():%i] ", __FILE__, __func__, __LINE__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#else
	#define flog(...)
#endif

char color[7] = "[1;32m";

int ICON_LINE = 0; //for printing icon left or right
int ICON_FINISHED = 0;
//formates printf
#define printff(var, ...) do {\
	if(ICON_INDEX > -1 && ICON_ALIGN == ALIGN_LEFT && !(ICON_FINISHED = !*icons[ICON_INDEX][ICON_LINE])) printf(" %s\e[0m   ", icons[ICON_INDEX][ICON_LINE++]);\
	printf(" \e%s%s\e[0m: ", color, var); printf(__VA_ARGS__);}\
while(0)

/* kernel */
void i_kernel(void) {
	struct utsname unamed;
	if(uname(&unamed) != -1) {
		printff("Kernel", "%s %s %s\n", unamed.sysname, unamed.release, unamed.machine);
	} else flog("cannot uname (buf invalid)");
}

/* get distro id and release and codename*/
int _distro(char* id, char* release, char* codename) {
	FILE* lsb = fopen("/etc/lsb-release", "r"); //contains DISTRIB_KEY=VALUE pairs
	if (lsb != NULL) {
		while (!feof(lsb) && !ferror(lsb)) { //iterate over every line. and get key=value pair
			char value[64];
			char key[64];

			if (fscanf(lsb, "DISTRIB_%[^=]=%[^\n]\n", key, value)) {
				if (!strcmp(key, "ID") && id != NULL) {
					strcpy(id, value);
				} else if (!strcmp(key, "RELEASE") && release != NULL) {
					strcpy(release, value);
				} else if (!strcmp(key, "CODENAME") && codename != NULL) {
					strcpy(codename, value);
				}
			}
		}
		fclose(lsb);
		return 1;
	} else {
		flog("cant open /etc/lsb-release, doesnt exist. create it or use -d\"distro name\"");
		return 0;
	}
}

/* os */
void i_os(void) {
	char id[16], release[8], codename[16];
	if (_distro(id, release, codename)) {
		printff("OS", "%s %s %s\n", id, release, codename);
	} else flog("couldnt get distro information");
}
/* shell */
void i_shell(void) {
	char* shell = getenv("SHELL");
	if (shell) {
		printff("Shell", "%s\n", shell);
	} else flog("cant get $SHELL");
}

/* desktop session?..*/
void i_desktop(void) {
	char* getenv(const char* name);
	char* desktop = getenv("GDMSESSION");
	if (desktop) {
		printff("Desktop", "%s\n", desktop);
	} else flog("cant get $GDMSESSION");
}

/* cpu */
void i_cpu(void) {
	struct cpu_raw_data_t cpuraw;
	struct cpu_id_t cpudata;

	if (cpuid_present() && cpuid_get_raw_data(&cpuraw) >= 0 && cpu_identify(&cpuraw, &cpudata) >= 0) { // check for CPUID work or not
		printff("CPU", "%s", cpudata.brand_str);
		float temp = 0;
		FILE* thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
		if (thermal != NULL) {
			fscanf(thermal, "%f\n", &temp);
			printf(" (%.1f°C)\n", temp/1000);
			fclose(thermal);
		} else {
			printf("\n");
		}
	} else flog("cant get cpuid information");
}

/* gpu */
void i_gpu(void) {
	FILE* pci = popen("/usr/bin/lspci", "r");
	if (pci != NULL) {
		char gpu[64];
		char* line = NULL;
		size_t n;
		#ifdef DEBUG
		int matched = 0;
		#endif
		while (!feof(pci) && getline(&line, &n, pci)) { //iterate over every line,
			if (strstr(line, "VGA") && sscanf(line, "%*[0-9a-f:.] VGA compatible controller: %[^\n(](%*[^)]) \n", gpu)) { //parse output
				printff("GPU", "%s\n", gpu);
				#ifdef DEBUG
				matched = 1;
				#endif
				break;
			}
		}
		#ifdef DEBUG
		if (!matched) flog("couldnt parse lspci information");
		#endif
		free(line);
		pclose(pci);
	} else flog("cant popen /usr/bin/lspci");
}

/* resoluion*/
void i_resolution(void) {
	Display* pdsp = NULL;
	Screen* pscr = NULL;

	pdsp = XOpenDisplay(NULL);
	if (!pdsp) {
		flog("X11 failed to open display");
		return;
	}

	pscr = DefaultScreenOfDisplay(pdsp);
	if (!pscr) {
		flog("X11 fail to get screen of display");
		return;
	}

	printff("Resolution", "%ix%i\n", pscr->width, pscr->height);

	XCloseDisplay(pdsp);
	/*
	FILE* xrandr = popen("/usr/bin/xrandr", "r");
	if (xrandr != NULL) {
		int width, height;
		if (fscanf(xrandr, "%*[^,], current %i x %i", &width, &height)) {
			printff("Resolution", "%ix%i\n", width, height);
		}
		pclose(xrandr);
	} else flog("cant popen /usr/bin/xrandr");
	*/
}


/* uptime*/
void i_uptime(void) {
	struct sysinfo info;
	if (sysinfo(&info) != -1) {
		printff("Uptime", "%lih %lim\n", info.uptime/3600, info.uptime/60%60);
	} else flog("cannt use sysinfo");
}

/* ram */
void i_ram(void) {
	#if 0
	//uses sysinfo from uptime (but is less acurate..??)
	printff("RAM", "%luMiB / %luMiB\n", (unsigned long int)(info.totalram-info.freeram-info.bufferram*10.4-info.sharedram)/(1024*1024), info.totalram/(1024*1024));
	#else
	FILE* meminfo = fopen("/proc/meminfo", "r"); //contains Key: value (kB) pairs
	if (meminfo != NULL) {
		size_t memtotal = 0;
		size_t memfree = 0;
		size_t buffers = 0;
		size_t cached = 0;
		while (!feof(meminfo) && !ferror(meminfo)) { //iterate over every line. and get he pair
			size_t value;
			char key[64];

			if (fscanf(meminfo, "%[^:]: %zi %*s\n", key, &value)) {
				if (!strcmp(key, "MemTotal"))
					memtotal = value;
				else if (!strcmp(key, "MemFree"))
					memfree = value;
				else if (!strcmp(key, "Buffers"))
					buffers = value;
				else if (!strcmp(key, "Cached"))
					cached = value;					
			}
		}
		printff("RAM", "%ziMiB / %ziMiB\n", (memtotal - memfree - buffers - cached)/1024, memtotal/1024);
		fclose(meminfo);
	}
	#endif
}

int main(int argc, char** argv) {
	char order[MAX_INFOS] = ORDER;
	char distro[MAX_INFOS] = "\0";

	for (int i = 1; i < argc; i++) {
		char* arg = argv[i];
		if (*arg == '-') {
			switch (arg[1]) {
				case 'h':
					fprintf(stderr, HELP);
					return 0;
				case 'v':
					fprintf(stderr, PROGRAM" "VERSION"\n");
					return 0;
				case 'c':
					strcpy(color, arg+2);
					break;
				case 'a':
					switch(arg[2]) {
						case 'u':
							ICON_ALIGN = ALIGN_UP;
							break;
						case 'd':
							ICON_ALIGN = ALIGN_DOWN;
							break;
						case 'l':
							ICON_ALIGN = ALIGN_LEFT;
							break;
						default:
							fprintf(stderr, "illegal align '%c'; valid are 'l', 'd' or 'u'\n", arg[2]);
							break;
					} break;
				case 'd':
					strncpy(distro, arg+2, MAX_INFOS);
					break;

				default:
					fprintf(stderr, "illegal option '%c'\n", arg[1]);
					fprintf(stderr, HELP);
					return 1;
			}
		} else {
			strncpy(order, arg, MAX_INFOS);
		}
	}
	
	if ((*distro) ? 1 : _distro(distro, NULL, NULL)) {
		//get index
		ICON_INDEX = get_index(distro);
	} 	
	
	#ifdef DEBUG
	if (ICON_INDEX == -1) flog("couldnt get distro icon");
	#endif

	//if icon in the top, print now
	if (ICON_INDEX > -1 && ICON_ALIGN == ALIGN_UP) {
		while(*icons[ICON_INDEX][ICON_LINE]) {
			printf(" %s\n", icons[ICON_INDEX][ICON_LINE++]);
		}
		printf("\e[0m");
	}

	//print each information
	for (int i = 0; i < strlen(order); i++) {
		#define map(c,f) case c: i_##f(); break
		switch (order[i]) {
			map('k', kernel);	
			map('o', os);
			map('s', shell);
			map('d', desktop);
			map('c', cpu);
			map('g', gpu);
			map('r', resolution);
			map('u', uptime);
			map('R', ram);
			default: fprintf(stderr, "ilegal thing. doesnot exist '%c'\n", order[i]);

		}
		#undef map
	}

	//IF icon at the bottom OR icon is at left and not finished.. print
	if (ICON_INDEX > -1 && ((ICON_ALIGN == ALIGN_LEFT && !ICON_FINISHED) || ICON_ALIGN == ALIGN_DOWN)) {
		while(*icons[ICON_INDEX][ICON_LINE]) {
			printf(" %s\n", icons[ICON_INDEX][ICON_LINE++]);
		}
		printf("\e[0m");
	}
}
