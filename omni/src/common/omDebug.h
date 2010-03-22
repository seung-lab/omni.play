#ifndef _OM_DEBUG_H_
#define _OM_DEBUG_H_

#define OM_DEBUG_STRING_SIZE 30
#define OM_DEBUG_STRING_MAX_NUMBER 30

#include <QString>

#ifdef NO_DEBUG
#define debug(x, ...) ()
#else
#define debug(x, ...) actual_debug(x, __VA_ARGS__)
#endif

#define DEBUGV3(vec) vec.x, vec.y, vec.z

class CmdLineArgs 
{
public:
	int fileArgIndex;
	bool runHeadless;
	QString headlessCMD;
};

enum {OM_DEBUG_REMOVE,OM_DEBUG_ADD};

/**
 * A function designed for selecting and customizing debug statements to stdout
 */

bool isDebugCategoryEnabled( const char *category );
void actual_debug(const char *category, const char *format, ...);
void debugInit();
int  debugParseArg(char *stringInput,int action);
void debugAddCategory(char *category,int length);
void debugRemoveCategory(char *category, int length);
int  parseEnvironment();
CmdLineArgs parseArgs(int argc, char *argv[]);
CmdLineArgs parseAnythingYouCan(int argc, char *argv[]);
void usage();

#define pthread_mutex_lock(x) debug("mutex","locking enter: %p (line:fun) %i:%s \n",x,__LINE__, __FUNCTION__);\
        pthread_mutex_lock(x);                                             \
        debug("mutex","locking exit: %p (line:fun) %i:%s \n",x,__LINE__, __FUNCTION__);

#define pthread_mutex_unlock(x) debug("mutex","unlocking enter: %p (line:fun) %i:%s \n",x,__LINE__, __FUNCTION__);\
        pthread_mutex_unlock(x);\
        debug("mutex","unlocking exit: %p (line:fun) %i:%s \n",x,__LINE__, __FUNCTION__);


bool ToggleShowMeAnImageEnabler();
void ShowMeAnImage(char *data_buffer, int dx, int dy);


#endif


