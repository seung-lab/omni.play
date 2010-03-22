#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "common/omDebug.h"
#include <QImage>                                                     
#include <QPainter>                                                  
#include <QDialog>                                              
#include <QLabel>                                                     
#include <QPixmap>                                                    
#include <QWidget>                                                     
#include <QGridLayout>                                                    
#include <QMessageBox>     

char debugCategoryArray[OM_DEBUG_STRING_MAX_NUMBER][OM_DEBUG_STRING_SIZE];
int debugCategoryNumber;
bool ViewerIsEnabled;
bool mDebugAll = false;

bool isDebugCategoryEnabled( const char *category )
{
	int i;
	for (i=0;i<debugCategoryNumber;i++){
		if (!strncmp(category,debugCategoryArray[i], OM_DEBUG_STRING_SIZE)) {
			return true;
		}
	}
	return false;
}

void actual_debug(const char *category, const char *format, ...)
{
	int i;
	va_list args;
	va_start(args, format);

	if( mDebugAll ) {
		vfprintf(stdout, format, args);
		fflush(stdout);
	} else {
		for (i=0;i<debugCategoryNumber;i++){
			if (!strncmp(category,debugCategoryArray[i], OM_DEBUG_STRING_SIZE)) {
				vfprintf(stdout, format, args);
				fflush(stdout);
			}
		}
	}

	va_end(args);
}

void debugInit()
{
	debugCategoryNumber=0;
}

int debugParseArg(char *stringInput,int action)
{
	int i, j, length, position;
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
	}
}

void debugRemoveCategory(char *category, int length)
{
        int i,j;
	bool exists=false;
	for (i=0;!exists&&(i<debugCategoryNumber);i++){
		if (!strncmp(category,debugCategoryArray[i],length)){
			exists=true;
		}
	}
	if (exists){
		debugCategoryNumber--;
		for (j=i-1;j<debugCategoryNumber;j++){
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
				if (-1==debugParseArg(&inputString[8],OM_DEBUG_ADD)) return -1; 
			} else {
				if (!strncmp(inputString,"-d ",3)){
					if (-1==debugParseArg(&inputString[3],OM_DEBUG_ADD)) return -1;
			        } else {
					if (!strncmp(inputString,"-d",2)){ 
						if (-1==debugParseArg(&inputString[2],OM_DEBUG_ADD)) return -1;
					} else {
						printf("Format error in the environment variable OMNI_DEBUG\n");
						printf("The format should is the same as it is for the command line\n");
						printf("The Omni program will resume as if the OMNI_DEBUG\n");
						printf("environment variable was never set . . . \n");
						debugCategoryNumber=0;
						return 0;
					}
				}
			 }
		} else if (-1==debugParseArg(inputString,OM_DEBUG_ADD)) return -1;
		return 1;
	} else return 0;
}

CmdLineArgs parseArgs(int argc, char *argv[])
{
	int i;
	int fileArgIndex=0;
	bool runHeadless = false;
	QString headlessCMD="";

	for(i=1; i<argc; i++) {
		if(argv[i][0] == '-') {
			if( 'd' == argv[i][1]){
				if(strlen(argv[i]) > 2){
					if(!strncmp(argv[i],"-debug",6)){
						usage();
						fileArgIndex = -2;
						break;
					} else {
						if(-1==debugParseArg(&argv[i][2],OM_DEBUG_ADD)){
							fileArgIndex = -1;
							break;
						}
					}
				} else 	if(-1==debugParseArg(argv[++i],OM_DEBUG_ADD)){
					fileArgIndex = -1;
					break;
				}
			} else if ( 'n' == argv[i][1] ){
				if(strlen(argv[i]) > 2){
					if(!strncmp(argv[i],"-nodebug",8)){
						usage();
						fileArgIndex = -2;
						break;
					} else {
						if( -1==debugParseArg(&argv[i][2],OM_DEBUG_REMOVE)){
							fileArgIndex = -1;
							break;
						}
					}
				} else if (-1==debugParseArg(argv[++i],OM_DEBUG_REMOVE)) {
					fileArgIndex = -1;
					break;
				}
			} else if ( 'p' ==  argv[i][1] ){
				//ignore process id passed from osx
				if(!strncmp(argv[i],"-psn",4)) {
					// do nothing
				} else {
					usage();
					fileArgIndex = -2;
					break;
				}
			} else if ( '-' ==  argv[i][1] ){
				if(!strncmp(argv[i],"--debug=",8)){
					if (-1==debugParseArg(&argv[i][8],OM_DEBUG_ADD)){
						fileArgIndex = -1;
						break;
					}
				} else if(!strncmp(argv[i],"--nodebug=",10)){
					if (-1==debugParseArg(&argv[i][10],OM_DEBUG_REMOVE)){
						fileArgIndex = -1;
						break;
					}
				} else if(!strncmp(argv[i],"--debugAll",10)){
					mDebugAll = true;
				} else if(!strncmp(argv[i],"--headless",10)){
					runHeadless = true;
					headlessCMD = QString::fromStdString( argv[i] );
				} else {
					printf("Unrecognized option %s.\n\n",argv[i]);
					usage();
					fileArgIndex = -2;
					break;
				}
			} else {
				printf("Unrecognized option %s.\n\n",argv[i]);
				usage();
				fileArgIndex = -2;
				break;
			}
			
		} else {
			if (!fileArgIndex){
				fileArgIndex=i;
			}
		}
	}

	CmdLineArgs args;
	args.fileArgIndex = fileArgIndex;
	args.runHeadless  = runHeadless;
	args.headlessCMD  = headlessCMD;
	return args;
}

CmdLineArgs parseAnythingYouCan(int argc, char *argv[])
{	
	int result;
	result   = parseEnvironment();

	if (result==-1){
		printf("\nA category string in your $OMNI_DEBUG environment \n");
		printf("variable is longer than the maximum length of\n");
		printf("%i characters.\n",OM_DEBUG_STRING_SIZE);
		printf("The Omni program will resume as if the OMNI_DEBUG\n");
		printf("environment variable was never set . . . \n\n");
		debugCategoryNumber=0;
	}

	CmdLineArgs args = parseArgs(argc, argv);

	if ( -1 == args.fileArgIndex ){
		printf("\nA category string in your command line \n");
		printf("is longer than the maximum length of\n");
		printf("%i characters.\n",OM_DEBUG_STRING_SIZE);
		printf(" . . . exiting . . . \n\n");
	}
	return args;
}

void usage() 
{
	printf("Usage: omni [OPTION] [FILE]\n\nOptions:\n"); 
	printf("   --debug=<category>    Add <category> to the list of strings which\n"); 
	printf("                         are used to determine what debug output is\n");
	printf("                         acceptable.\n");
	printf("   -d <category>         Same as --debug=<category>\n");
	printf("   -d<category>          Same as --debug=<category>\n");
	printf("   --nodebug=<category>  Remove<category> from the list of strings which\n"); 
	printf("                         are used to determine what debug output is\n");
	printf("                         acceptable.\n");
	printf("   -n <category>         Same as --nodebug=<category>\n");
	printf("   -n<category>          Same as --nodebug=<category>\n\n");
}

bool ToggleShowMeAnImageEnabler()
{
        ViewerIsEnabled=!ViewerIsEnabled;
        return ViewerIsEnabled;
}


void ShowMeAnImage(char *        data_buffer,
                int     dx,
                int     dy)
{
        if (ViewerIsEnabled){
                QImage image((uchar*)data_buffer,dx,dy,QImage::Format_Indexed8);
                for (int ii=0;ii<255;ii++) {image.setColor(ii,qRgb(ii,ii,ii));}

                QDialog* dialog = new QDialog(NULL,Qt::Popup);
                QGridLayout* gridLayout = new QGridLayout(dialog);
                QPixmap pixmap;
                QLabel *label = new QLabel();
                gridLayout->addWidget(label);

                label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                label->setScaledContents(true);

                dialog->resize(dx+30,dy+30);
                label->setPixmap(QPixmap::fromImage(image));

                dialog->exec();
                return;
        }
}
