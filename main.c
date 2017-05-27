#include <sys/utsname.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <libcpuid/libcpuid.h>

/* program stuff.*/

#define ORDER "kosdcgruR" //default order
#define PROGRAM "cfetch"
#define VERSION "0.1"
#define HELP PROGRAM" "VERSION" (compiled "__DATE__"-"__TIME__")\n\
usage: "PROGRAM" [options] order\nwherein options are\n\
 -h show help\n\
 -v version\n\
 -c\"color code\" set things color for example red '"PROGRAM" -c\"[1;31m\"'\n\
order is the order for the things that are the information.. default one is '"ORDER"',and letters:\n\
k-kernel, o-os, s-shell, d-desktop, c-cpu, g-gpu, r-resolution, u-uptime, R-ram\n"

#ifdef DEBUG
	#define flog(...) do { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#else
	#define flog(...)
#endif

char color[7] = "[1;32m";

//formates printf
#define printff(var, ...) do {printf("\e%s%s\e[0m: ", color, var); printf(__VA_ARGS__);} while(0)

/* kernel */
void i_kernel(void) {
	struct utsname unamed;
	uname(&unamed);
	printff("Kernel", "%s %s %s\n", unamed.sysname, unamed.release, unamed.machine);
}

/* os */
void i_os(void) {
	FILE* lsb = fopen("/etc/lsb-release", "r"); //contains DISTRIB_KEY=VALUE pairs
	if (lsb != NULL) {
		char id[64];
		char release[64];
		char codename[64];
		while (!feof(lsb) && !ferror(lsb)) { //iterate over every line. and get key=value pair
			char value[64];
			char key[64];

			if (fscanf(lsb, "DISTRIB_%[^=]=%[^\n]\n", key, value)) {
				#define eq(s1,s2) (strcmp(s1,s2) == 0)
				if (eq(key, "ID"))
					strcpy(id, value);
				else if (eq(key, "RELEASE"))
					strcpy(release, value);
				else if (eq(key, "CODENAME"))
					strcpy(codename, value);
				#undef eq
			}
		}
		printff("OS", "%s %s %s\n", id, release, codename);
		fclose(lsb);
	}
}
/* shell */
void i_shell(void) {
	char* getenv(const char* name);
	char* shell = getenv("SHELL");
	printff("Shell", "%s\n", shell ? shell : "unknown");
}

/* desktop session?..*/
void i_desktop(void) {
	char* getenv(const char* name);
	char* desktop = getenv("DESKTOP_SESSION");
	if (desktop) {
		printff("Desktop", "%s\n", desktop);
	}
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
	}
}

/* gpu */
void i_gpu(void) {
	void free (void* ptr);
	FILE* pci = popen("/usr/bin/lspci", "r");
	if (pci != NULL) {
		char gpu[64];
		char* line = NULL;
		size_t n;
		while (!feof(pci) && getline(&line, &n, pci)) { //iterate over every line,
			if (strstr(line, "VGA") && sscanf(line, "%*[0-9a-f:.] VGA compatible controller: %[^\n(](%*[^)]) \n", gpu)) { //parse output
				printff("GPU", "%s\n", gpu);
			}
		}
		free(line);
		pclose(pci);
	}
}

/* resoluion*/
void i_resolution(void) {
	FILE* xrandr = popen("/usr/bin/xrandr", "r");
	if (xrandr != NULL) {
		int width, height;
		if (fscanf(xrandr, "%*[^,], current %i x %i", &width, &height)) {
			printff("Resolution", "%ix%i\n", width, height);
		}
		pclose(xrandr);
	}
}


/* uptime*/
void i_uptime(void) {
	struct sysinfo info;
	sysinfo(&info);
	printff("Uptime", "%lih %lim\n", info.uptime/3600, info.uptime/60%60);
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
				#define eq(s1,s2) (strcmp(s1,s2) == 0)
				if (eq(key, "MemTotal"))
					memtotal = value;
				else if (eq(key, "MemFree"))
					memfree = value;
				else if (eq(key, "Buffers"))
					buffers = value;
				else if (eq(key, "Cached"))
					cached = value;					
				#undef eq
			}
		}
		printff("RAM", "%ziMiB / %ziMiB\n", (memtotal - memfree - buffers - cached)/1024, memtotal/1024);
		fclose(meminfo);
	}
	#endif
}

int main(int argc, char** argv) {
	char order[] = ORDER;

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
				default:
					fprintf(stderr, "illegal option '%c'\n", arg[1]);
					fprintf(stderr, HELP);
					return 1;
			}
		} else {
			strncpy(order, arg, strlen(order));
		}
	}

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


	return 0;
}
