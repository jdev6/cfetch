#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define ALIGN_LEFT  0
#define ALIGN_UP    1
#define ALIGN_DOWN  2

int ICON_ALIGN = ALIGN_LEFT;
int ICON_INDEX = -1;

char* indexs[] = {"ManjaroLinux", "Ubuntu"};

int get_index(char* distro) {
	for (int i = 0; i < ARRAY_SIZE(indexs); i++) {
		if (strcmp(distro, indexs[i]) == 0) {
			return i;
		}
	}
	return -1;
}

char* icons[][24] = {
	/* manjaro */
	{
	"\e[1;32m██████████████████  ████████",
	"\e[1;32m██████████████████  ████████",
	"\e[1;32m██████████████████  ████████",
	"\e[1;32m██████████████████  ████████",
	"\e[1;32m████████            ████████",
	"\e[1;32m████████  ████████  ████████",
	"\e[1;32m████████  ████████  ████████",
	"\e[1;32m████████  ████████  ████████",
	"\e[1;32m████████  ████████  ████████",
	"\e[1;32m████████  ████████  ████████",
	"\e[1;32m████████  ████████  ████████",
	"\e[1;32m████████  ████████  ████████",
	"\e[1;32m████████  ████████  ████████",
	"\e[1;32m████████  ████████  ████████",
	""},
	/*ubuntu*/
	{
	"\e[1;31m            .-/+oossssoo+/-.",
	"\e[1;31m        `:+ssssssssssssssssss+:`",
	"\e[1;31m      -+ssssssssssssssssssyyssss+-",
	"\e[1;31m    .ossssssssssssssssss\e[1;33mdMMMNy\e[1;31msssso.",
	"\e[1;31m   /sssssssssss\e[1;33mhdmmNNmmyNMMMMh\e[1;31mssssss/",
	"\e[1;31m  +sssssssss\e[1;33mhm\e[1;31myd\e[1;33mMMMMMMMNddddy\e[1;31mssssssss+",
	"\e[1;31m /ssssssss\e[1;33mhNMMM\e[1;31myh\e[1;33mhyyyyhmNMMMNh\e[1;31mssssssss/",
	"\e[1;31m.ssssssss\e[1;33mdMMMNh\e[1;31mssssssssss\e[1;33mhNMMMd\e[1;31mssssssss.",
	"\e[1;31m+ssss\e[1;33mhhhyNMMNy\e[1;31mssssssssssss\e[1;33myNMMMy\e[1;31msssssss+",
	"\e[1;31moss\e[1;33myNMMMNyMMh\e[1;31mssssssssssssss\e[1;33mhmmmh\e[1;31mssssssso",
	"\e[1;31moss\e[1;33myNMMMNyMMh\e[1;31msssssssssssssshmmmh\e[1;31mssssssso",
	"\e[1;31m+ssss\e[1;33mhhhyNMMNy\e[1;31mssssssssssss\e[1;33myNMMMy\e[1;31msssssss+",
	"\e[1;31m.ssssssss\e[1;33mdMMMNh\e[1;31mssssssssss\e[1;33mhNMMMd\e[1;31mssssssss.",
	"\e[1;31m /ssssssss\e[1;33mhNMMM\e[1;31myh\e[1;33mhyyyyhdNMMMNh\e[1;31mssssssss/",
	"\e[1;31m  +sssssssss\e[1;33mdm\e[1;31myd\e[1;33mMMMMMMMMddddy\e[1;31mssssssss+",
	"\e[1;31m   /sssssssssss\e[1;33mhdmNNNNmyNMMMMh\e[1;31mssssss/",
	"\e[1;31m    .ossssssssssssssssss\e[1;33mdMMMNy\e[1;31msssso.",
	"\e[1;31m      -+sssssssssssssssss\e[1;33myyy\e[1;31mssss+-",
	"\e[1;31m        `:+ssssssssssssssssss+:`",
	"\e[1;31m            .-/+oossssoo+/-.",
	""}
};
