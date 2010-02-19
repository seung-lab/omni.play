
#include "omGarbage.h"
#include "omPreferenceDefinitions.h"
#include "omCacheManager.h"

#include "common/omException.h"
#include "common/omDebug.h"
#include "system/omProjectData.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <QFileInfo>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/utsname.h>



namespace bfs = boost::filesystem;

#define DEBUG 0

//init instance pointer
OmGarbage *OmGarbage::mspInstance = 0;

#pragma mark -
#pragma mark OmGarbage
/////////////////////////////////
///////
///////          OmGarbage
///////

OmGarbage::OmGarbage()
{
}

OmGarbage::~OmGarbage()
{

}

OmGarbage *OmGarbage::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmGarbage;
	}

	QString fileName  = QString::fromStdString( OmProjectData::GetFileName() );
	QString pathName  = QString::fromStdString( OmProjectData::GetDirectoryPath() );
	QString rel_fnpn = pathName + fileName;
	QFileInfo fInfo(rel_fnpn);
	QString fnpnProject = fInfo.absoluteFilePath();
	mspInstance->mHdf5Lock = fnpnProject + ".lock";

	pthread_mutex_init(&mspInstance->mTextureMutex, NULL);
	return mspInstance;
}

void OmGarbage::Delete()
{
	if (mspInstance) {
		pthread_mutex_destroy(&OmGarbage::Instance()->mTextureMutex);
		delete mspInstance;
	}
	mspInstance = NULL;
}

void OmGarbage::asOmTextureId(GLuint texture)
{
	OmGarbage::Instance()->Lock();
	OmGarbage::Instance()->mTextures.push_back(texture);
	OmGarbage::Instance()->Unlock();
}

void OmGarbage::Lock()
{
	pthread_mutex_lock(&OmGarbage::Instance()->mTextureMutex);
}

vector < GLuint > &OmGarbage::LockTextures()
{
	OmGarbage::Instance()->Lock();
	return OmGarbage::Instance()->mTextures;
}

void OmGarbage::Unlock()
{
	pthread_mutex_unlock(&OmGarbage::Instance()->mTextureMutex);
}

void OmGarbage::UnlockTextures()
{
	OmGarbage::Instance()->Unlock();
}

void OmGarbage::safeCleanTextureIds()
{
	OmGarbage::Instance()->Lock();

	//debug("FIXME",  << "freeing... " << OmGarbage::Instance()->mTextures.size()<< endl;

	glDeleteTextures(OmGarbage::Instance()->mTextures.size(), &OmGarbage::Instance()->mTextures[0]);
	OmGarbage::Instance()->mTextures.clear();

	OmGarbage::Instance()->Unlock();
}


/* 
 * Almost all this code is ganked from liblockfile for use with locking over nfs.
 */
struct utsname uts;

/*
 *      See if the directory where is certain file is in
 *      is located on an NFS mounted volume.
 */
static int is_nfs(const char *file)
{
        char dir[1024];
        char *s;
        struct stat st;

        strncpy(dir, file, sizeof(dir));
        if ((s = strrchr(dir, '/')) != NULL)
                *s = 0;
        else
                strcpy(dir, ".");

        if (stat(dir, &st) < 0)
                return 0;

        return ((st.st_dev & 0xFF00) == 0);
}


/*
 *      Put our process ID into a string.
 */
static void putpid(char *s)
{
        static char pidstr[6];
        pid_t pid;
        int i;

        if (pidstr[0] == 0) {
                pid = getpid();
                for(i = 0; i < 5; i++) {
                        pidstr[4 - i] = (pid % 10) + '0';
                        pid /= 10;
                }
                pidstr[5] = 0;
        }
        strcpy(s, pidstr);
}


int fakeopen(const char *file, int flags, ...)
{
        char tmp[1024];
        char *s;
        int mode, i, e, error;
        va_list ap;
        struct stat st1, st2;

        if (!(flags & O_CREAT))
                return open(file, flags);

        va_start(ap, flags);
        mode = va_arg(ap, int);
        va_end(ap);

        /*
         *      NFS has no atomic creat-if-not-exist (O_EXCL) but we
         *      can emulate it by creating the file under a temporary
         *      name and then renaming it to the final destination.
         */
        if ((flags & O_EXCL) /* && !istmplock(file) */ && is_nfs(file)) {
                /*
                 *      Try to make a unique temp name, network-wide.
                 */
                if (strlen(file) > sizeof(tmp) - 16) {
                        errno = ENAMETOOLONG;
                        return -1;
                }
                strcpy(tmp, file);
                if ((s = strrchr(tmp, '/')) != NULL)
                        s++;
                else
                        s = tmp;
                *s = 0;
                strcpy(s, ".nfs");
                if (uts.nodename[0] == 0) uname(&uts);
                for(i = 0; i < 5 && uts.nodename[i] &&
                        uts.nodename[i] != '.'; i++)
                                s[4 + i] = uts.nodename[i];
                putpid(s + 4 + i);
                if ((i = open(tmp, flags, mode)) < 0)
                        return i;

                /*
                 *      We don't just check the result code from link()
                 *      but we stat() both files as well to see if they're
                 *      the same just to be sure.
                 */
                error = link(tmp, file);
                e = errno;
                if (error < 0) {
                        (void)unlink(tmp);
                        close(i);
                        errno = e;
                        return error;
                }

                error = stat(tmp, &st1);
                e = errno;
                (void)unlink(tmp);
                if (error < 0) {
                        close(i);
                        errno = e;
                        return -1;
                }
                if (stat(file, &st2) < 0) {
                        close(i);
                        errno = e;
                        return -1;
                }
                if (st1.st_ino != st2.st_ino) {
                        close(i);
                        errno = EEXIST;
                        return -1;
                }

                return i;
        }
        return open(file, flags, mode);
}


int fakecreat(const char *file, mode_t mode)
{
        return fakeopen(file, O_CREAT|O_WRONLY|O_TRUNC, mode);
}


void OmGarbage::Hdf5Lock ()
{
	//OmGarbage::Instance()->Lock ();
	//return;

	int hdf5Fd;
	do {
		hdf5Fd = fakecreat (qPrintable (OmGarbage::Instance()->mHdf5Lock), O_CREAT|O_EXCL);
	} while (-1 == hdf5Fd);
	close (hdf5Fd);
}


void OmGarbage::Hdf5Unlock ()
{
	//OmGarbage::Instance()->Unlock ();
	//return;

	unlink (qPrintable (OmGarbage::Instance()->mHdf5Lock));
}
