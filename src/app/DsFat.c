#include "DsFat.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//Ä£ÄâDsFat½Ó¿Ú
char g_dsFatDiretory[260];

FAT_BOOL fatInit(unsigned long flashSize)
{
    g_dsFatDiretory[0] = '\0';
    return FAT_TRUE;
}

FAT_BOOL fatEnterDirectory(char * subDirectoryName)
{
    if (subDirectoryName == 0)
    {
        return FAT_FALSE;
    }

    if (g_dsFatDiretory[0] != '\0')
    {
        sprintf(g_dsFatDiretory + strlen(g_dsFatDiretory), "//%s", subDirectoryName);
    } 
    else
    {
        strcpy(g_dsFatDiretory, subDirectoryName);
    } 
    
    return FAT_TRUE;    
}

FAT_FILE_HANDLE fatFileOpen(char * fileName, unsigned char openMode)
{
    FILE * file;
    char * path;
    
    if (fileName == 0)
    {
        return FAT_FILE_HANDLE_NULL;
    }
    
    path = (char *)malloc(512);
    
    if (g_dsFatDiretory[0] != '\0')
    {
        sprintf(path, "%s//%s", g_dsFatDiretory, fileName);
    } 
    else
    {
        strcpy(path, fileName);
    }
    
    file =  fopen(path, "rb");
    
    if (file == NULL)
    {
		free(path);
		return FAT_FILE_HANDLE_NULL;
	}
    
    free(path);
    return (FAT_FILE_HANDLE)file;      
}

unsigned long fatFileGetLen(FAT_FILE_HANDLE handle)
{
    FILE * file;
    unsigned long len;
    
    file = (FILE *)handle;
    
	fseek(file, 0, SEEK_END);
    len = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    return len;	    
}

unsigned long fatFileRead(FAT_FILE_HANDLE handle, unsigned char * buffer, unsigned long size)
{
    FILE * file;
    unsigned long len;
    
    file = (FILE *)handle;
    
    len = fread(buffer, 1, size, file);
    
    return len;
}

FAT_BOOL fatFileClose(FAT_FILE_HANDLE handle)
{
    FILE * file;
    
    file = (FILE *)handle; 
    fclose(file);   
}


void fatBackToRootDirectory(void)
{
    g_dsFatDiretory[0] = '\0';        
}
