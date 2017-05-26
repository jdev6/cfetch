#include <sys/utsname.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <libcpuid/libcpuid.h>

#ifndef NODEBUG
	#define flog(...) do { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#else
	#define flog(...)
#endif

char color[7] = "[1;32m";

//formates printf
#define printff(var, ...) do {printf("\e%s%s\e[0m: ", color, var); printf(__VA_ARGS__);} while(0)

int main() {
	struct utsname unamed;
	uname(&unamed);
	
	/* kernel */
	printff("Kernel", "%s %s\n", unamed.sysname, unamed.release);

	/* os */
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

	/* shell */
	char* getenv(const char* name);
	char* shell = getenv("SHELL");
	printff("Shell", "%s\n", shell ? shell : "unknown");

	/* desktop session?..*/
	char* desktop = getenv("DESKTOP_SESSION");
	if (desktop) {
		printff("Desktop", "%s\n", desktop);
	}

	/* cpu */
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
	
	/* gpu */
	FILE* pci = popen("/usr/bin/lspci | /usr/bin/grep VGA", "r"); //todo not use grep..
	if (pci != NULL) {
		char gpu[256];
		if (fscanf(pci, "%*[0-9:.] VGA compatible controller: %[^\n(]", gpu)) { //parse output
			printff("GPU", "%s\n", gpu);
		}
		pclose(pci);
	}


	/* uptime*/
	struct sysinfo info;
	sysinfo(&info);
	printff("Uptime", "%lih %lim\n", info.uptime/3600, info.uptime/60%60);

	/* ram */
	//uses sysinfo from uptime
	printff("RAM", "%luMiB / %luMiB\n", (unsigned long int)(info.totalram-info.freeram-info.bufferram*10.4)/(1024*1024), info.totalram/(1024*1024));	
}
