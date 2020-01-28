#include "FileSystemUtils.h"

#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>

#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
#define mkdir(a, b) CreateDirectory(a, NULL)
#define VNEEDS_MIGRATION (mkdirResult != 0)
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__mc68000)
#include <sys/stat.h>
#include <limits.h>
#define VNEEDS_MIGRATION (mkdirResult == 0)
/* These are needed for PLATFORM_* crap */
#include <unistd.h>
#include <dirent.h>
#define MAX_PATH PATH_MAX
#include <osbind.h>
#endif

char saveDir[MAX_PATH];
char levelDir[MAX_PATH];

void PLATFORM_getOSDirectory(char* output);
void PLATFORM_copyFile(const char *oldLocation, const char *newLocation);

int FILESYSTEM_init(char *argvZero)
{
	char output[MAX_PATH];
	int mkdirResult;

	/* Determine the OS user directory */
	PLATFORM_getOSDirectory(output);

	/* Create base user directory, mount */
	mkdirResult = mkdir(output, 0777);

	/* Mount our base user directory */
	printf("Base directory: %s\n", output);

	/* Create save directory */
	strcpy(saveDir, output);
	strcat(saveDir, "saves");
	strcat(saveDir, "\\");
	mkdir(saveDir, 0777);
	printf("Save directory: %s\n", saveDir);

	/* Create level directory */
	strcpy(levelDir, output);
	strcat(levelDir, "levels");
	strcat(saveDir, "\\");
	mkdirResult |= mkdir(levelDir, 0777);
	printf("Level directory: %s\n", levelDir);

	/* We didn't exist until now, migrate files! */

	/* Mount the stock content last */
#ifdef _WIN32
	strcpy(output, PHYSFS_getBaseDir());
	strcat(output, "data.zip");
#else
	strcpy(output, "data.zip");
#endif
	//if (!PHYSFS_mount(output, NULL, 1))
	//{
	//	puts("Error: data.zip missing!");
	//	puts("You do not have data.zip!");
	//	puts("Grab it from your purchased copy of the game,");
	//	puts("or get it from the free Make and Play Edition.");

	//	//SDL_ShowSimpleMessageBox(
	//	//	SDL_MESSAGEBOX_ERROR,
	//	//	"data.zip missing!",
	//	//	"You do not have data.zip!"
	//	//	"\n\nGrab it from your purchased copy of the game,"
	//	//	"\nor get it from the free Make and Play Edition.",
	//	//	NULL
	//	//);
	//	return 0;
	//}
	return 1;
}

void FILESYSTEM_deinit()
{
	//PHYSFS_deinit();
}

char *FILESYSTEM_getUserSaveDirectory()
{
	return saveDir;
}

char *FILESYSTEM_getUserLevelDirectory()
{
	return levelDir;
}

void FILESYSTEM_loadFileToMemory(const char *name, unsigned char **mem, size_t *len)
{
    FILE *handle=fopen(name,"r");
	if (handle == NULL)
	{
		return;
	}
    uint32_t length=fseek(handle,0,2);
    fseek(handle,0,0);
	if (len != NULL)
	{
		*len = length;
	}
	*mem = (unsigned char*) malloc(length);
    fread(*mem,length,1,handle);
    fclose(handle);
}

void FILESYSTEM_freeMemory(unsigned char **mem)
{
	free(*mem);
	*mem = NULL;
}

std::vector<std::string> FILESYSTEM_getLevelDirFileNames()
{
	std::vector<std::string> list;
    _DTA *my_dta = Fgetdta();
    short fnd = Fsfirst("levels\\*.*", 0xff);
	char **i;
	std::string builtLocation;

	while (!fnd)
	{
		//if (strcmp(*i, "data") == 0)
		//{
		//	continue; /* FIXME: lolwut -flibit */
		//}
		builtLocation = "levels/";
		builtLocation += my_dta->dta_name;
		list.push_back(builtLocation);
	}

	return list;
}

void PLATFORM_getOSDirectory(char* output)
{
#if defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__) || defined(__OpenBSD__)
	strcpy(output, PHYSFS_getPrefDir("distractionware", "VVVVVV"));
#elif defined(_WIN32)
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, output);
	strcat(output, "\\VVVVVV\\");
#elif defined(__m68k__)
    strcat(output, "\\VVVVVV\\");
#else
#error See PLATFORM_getOSDirectory
#endif
}

void PLATFORM_copyFile(const char *oldLocation, const char *newLocation)
{
	char *data;
	long int length;

	/* Read data */
	FILE *file = fopen(oldLocation, "rb");
	if (!file)
	{
		printf("Cannot open/copy %s\n", oldLocation);
		return;
	}
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);
	data = (char*) malloc(length);
	fread(data, 1, length, file);
	fclose(file);

	/* Write data */
	file = fopen(newLocation, "wb");
	if (!file)
	{
		printf("Could not write to %s\n", newLocation);
		free(data);
		return;
	}
	fwrite(data, 1, length, file);
	fclose(file);
	free(data);

	/* WTF did we just do */
	printf("Copied:\n\tOld: %s\n\tNew: %s\n", oldLocation, newLocation);
}
