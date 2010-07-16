#ifndef _ZI_THREAD_WIN32_
# define _ZI_THREAD_WIN32_
# if (defined(WIN32) || defined(__WIN32__) || defined(__WIN32)) &&       \
   !(defined(__CYGWIN32__) || defined(__CYGWIN__))
#  define ZI_WINDOWS
# endif
#endif
