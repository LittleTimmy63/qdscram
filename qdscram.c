// Quick And Dirty Scrambler
// COPYLEFT 2022 ALL WRONGS RESERVED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#if defined (__unix__) || defined (unix) || \
(defined (__APPLE__) && defined (__MACH__))

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

int fltrper(const struct dirent* dp)
{
	if ((strcmp(dp->d_name, ".") == 0) ||
	(strcmp(dp->d_name, "..")) == 0) {
		return 0;
	} else return 1;
}

void scramble_posix(char* name, char psycho)
{
	/* chdir() ensures I don't have to worry about whether the user
	 * wrote the path with a / at the end or not.
	 */
	chdir(name);
	char nmbuf [PATH_MAX];
	getcwd(nmbuf, PATH_MAX);
	DIR* dir = opendir(".");
	time_t tm = time(NULL);
	struct dirent* entry;
	char bak [PATH_MAX];
	char tmp1 [PATH_MAX];
	char tmp2 [PATH_MAX];
	if (psycho != 1) {
		snprintf(bak, PATH_MAX, "%s-%lu", nmbuf, (unsigned long)tm);
		mkdir(bak, 0777);
	}
	while ((entry = readdir(dir)) != NULL) {
		if ((strcmp(entry->d_name, ".") == 0) ||
		(strcmp(entry->d_name, "..")) == 0) {
			continue;
		}

		if (psycho != 1) {
			snprintf(tmp1, (PATH_MAX / 2),
			"%s/%s", nmbuf, entry->d_name);
			FILE* ftmp1 = fopen(tmp1, "r");
			snprintf(tmp2, (PATH_MAX / 2),
			"%s/%s", bak, entry->d_name);
			FILE* ftmp2 = fopen(tmp2, "w");
			int c;
			while ((c = fgetc(ftmp1)) != EOF) {
				fputc(c, ftmp2);
			}

			fclose(ftmp1);
			fclose(ftmp2);
		}
	}
	rewinddir(dir);

	srand(tm);
	struct dirent** nmls;
	int dirsz = scandir(nmbuf, &nmls, fltrper, alphasort);
	int ran;
	// Highly unlikely to be in use.
	char* temp = "DONKEYSTRING!#";
	for (int i = 0; i < dirsz; i++) {
		ran = (rand() % ((dirsz - 1) + 1));
		strcpy(tmp1, nmls[i]->d_name);
		strcpy(tmp2, nmls[ran]->d_name);
		rename(nmls[i]->d_name, temp);
		rename(tmp2, tmp1);
		rename(temp, tmp2);
	}

	for (int n = 0; n < dirsz; n++)
		free(nmls[n]);
	free(nmls);
	closedir(dir);
}

#define SCRAMBLE scramble_posix
#endif

#if defined (_WIN32) || defined (_WIN64)

#include <windows.h>

void scramble_windows(char* name, char psycho) {
	puts("Hello from Windows!");
}

#define SCRAMBLE scramble_windows
#endif

int main (int argc, char** argv)
{
	switch(argc) {
		case 2:
			SCRAMBLE(argv[1], 0);
			break;
		case 3:
			if ((strcmp(argv[2], "PSYCHO")) == 0)
				SCRAMBLE(argv[1], 1);
			else
				SCRAMBLE(argv[1], 0);
			break;
		default:
			fputs("USAGE: qdscram [DIRECTORY] [PSYCHOPATH]\n"
			"[PSYCHOPATH] - If the third arg is \"PSYCHO\", "
			"in all caps,\n"
			"then no backup is made.\n",stderr);
			exit(EXIT_FAILURE);
	}
	return 0;
}
