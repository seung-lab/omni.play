#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "system/omDebug.h"

char debugCategoryArray[OM_DEBUG_STRING_MAX_NUMBER][OM_DEBUG_STRING_SIZE];
int debugCategoryNumber;


//***********************************************************************
// TODO Convert to command line switches ---> get rid of #defs
//      - Make this a singleton class
//      - Include a ParseArg function
//      - Include a Default Settings Section
//      - Command Line input will override default
//***********************************************************************

void debug(const char *category, const char *format, ...)
{
	int i;
	va_list args;
	va_start(args, format);
	for (i=0;i<debugCategoryNumber;i++){
		if (!strncmp(category,debugCategoryArray[i], OM_DEBUG_STRING_SIZE))
			vfprintf(stdout, format, args);
	}
	va_end(args);
}

void debugInit()
{
	debugCategoryNumber=0;
}

int debugParseArg(char *stringInput,int action)
{
	int i,j,length,position,newPosition,categoryLength;
	char category[OM_DEBUG_STRING_SIZE];
	length=strlen(stringInput);
	position=0;
	for (i=0;i<=length;i++){
		if ((stringInput[i]==',')||(stringInput[i]=='\0')) {
			//categoryLength=i-position;
			if (i-position <= OM_DEBUG_STRING_SIZE) { 
				for (j=position;j<i;j++){
					category[j-position]=stringInput[j];
				}
				for (j=j-position;j<OM_DEBUG_STRING_SIZE;j++) {
					category[j]='\0';
				}
				if (action==OM_DEBUG_ADD) debugAddCategory(category,i-position+1);
				else debugRemoveCategory(category,i-position+1); 
			} else 	return -1;
			position=i+1;
		}
		
	}
	return 0;
}

void debugAddCategory(char *category,int length)
{
        int i;
	bool exists=false;
	for (i=0;i<debugCategoryNumber;i++){
		if (!strncmp(category,debugCategoryArray[i],length)){
			exists=true;
			i=debugCategoryNumber;
		}
	}
	if (!exists){
		strncpy(debugCategoryArray[debugCategoryNumber++],category,OM_DEBUG_STRING_SIZE);
		debugCategoryNumber++;
	}
}

void debugRemoveCategory(char *category, int length)
{
        int i,j;
	bool exists=false;
	for (i=0;i<debugCategoryNumber;i++){
		if (!strncmp(category,debugCategoryArray[i],length)){
			exists=true;
		}
	}
	if (exists){
		debugCategoryNumber--;
		for (j=i;j<debugCategoryNumber;j++){
			strncpy(debugCategoryArray[j],debugCategoryArray[j+1],OM_DEBUG_STRING_SIZE);
		}
		debugRemoveCategory(category, length);
	}
}
int parseEnvironment()
{
	char* inputString;
	inputString=getenv("OMNI_DEBUG");
	if (inputString != NULL){
		if(inputString[0] == '-') {
			if (!strncmp(inputString,"--debug=",8)){
					debugParseArg(&inputString[8],OM_DEBUG_ADD); 
			} else {
				if (!strncmp(inputString,"-d ",3)){
						debugParseArg(&inputString[3],OM_DEBUG_ADD);
			        } else {
					if (!strncmp(inputString,"-d",2)){ 
							debugParseArg(&inputString[2],OM_DEBUG_ADD);
					} else {
						printf("Format error in the environment variable OMNI_DEBUG\n");
						printf("The format should is the same as it is for the command line\n");
						printf("The Omni program will resume as if the OMNI_DEBUG\n");
						printf("environment variable was never set . . . \n");
						return 0;
					}
				}
			 }
		} else debugParseArg(inputString,OM_DEBUG_ADD);
		return 1;
	} else return 0;
}

int parseArgs(int argc, char *argv[])
{
	int i;
	int fileArgIndex=0;

	for(i=1; i<argc; i++) {
		if(argv[i][0] == '-') {
			switch(argv[i][1]) {
				case 'd':
					if(strlen(argv[i]) > 2){
						if(!strncmp(argv[i],"-debug",6)){
							usage();
							return -1;
						} else {
							debugParseArg(&argv[i][2],OM_DEBUG_ADD);
						}
					} else debugParseArg(argv[i+1],OM_DEBUG_ADD);
					break;
				case 'n':
					if(strlen(argv[i]) > 2){
						if(!strncmp(argv[i],"-nodebug",8)){
							usage();
							return -1;
						} else {
							debugParseArg(&argv[i][2],OM_DEBUG_REMOVE);
						}
					} else debugParseArg(argv[i+1],OM_DEBUG_REMOVE);
					break;
				case '-':
					if(!strncmp(argv[i],"--debug=",8)){
						debugParseArg(&argv[i][8],OM_DEBUG_ADD);
					} else {
						if(!strncmp(argv[i],"--nodebug=",10)){
							debugParseArg(&argv[i][10],OM_DEBUG_REMOVE);
						} else {
							printf("Unrecognized option %s.\n\n",argv[i]);
						        usage();
							return -1;	
						}
					}
					break;
				default:
					printf("Unrecognized option %s.\n\n",argv[i]);
					usage();
					return -1;
			}
			i++;
		} else {
			if (!fileArgIndex) fileArgIndex=i;
		}
	}
	return fileArgIndex;
}

int parseAnythingYouCan(int argc, char *argv[])
{	
	int result,fileArgIndex;
	result   = parseEnvironment();
	fileArgIndex = parseArgs(argc, argv);
	return fileArgIndex;
}
void
usage(void) 
{
	printf("Usage: omni [OPTION] [FILE]\n\nOptions:\n"); 
	printf("   --debug=<category>    Add <category> to the list of strings which\n"); 
	printf("                         are used to determine what debug output is\n");
	printf("                         acceptable.\n");
	printf("   -d <category>         Same as --debug=<category>");
	printf("   -d<category>          Same as --debug=<category>");
	printf("   --nodebug=<category>  Remove<category> from the list of strings which\n"); 
	printf("                         are used to determine what debug output is\n");
	printf("                         acceptable.\n");
	printf("   -n <category>         Same as --nodebug=<category>\n");
	printf("   -n<category>          Same as --nodebug=<category>\n\n");
}

