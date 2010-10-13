/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef COMMON_BASE_BASE_H_
#define COMMON_BASE_BASE_H_

#include <errno.h>
#include <assert.h>

#ifndef ULLONG_MAX
#define ULLONG_MAX 0xffffffffffffffffULL
#endif

class CantBeCopied {
protected:
  CantBeCopied()  {}
  ~CantBeCopied() {}
private:
  CantBeCopied                   (const CantBeCopied&);
  const CantBeCopied& operator = (const CantBeCopied&);
};

#define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))

#define ITERATOR(container) __typeof__((container).begin())

#ifdef LIVECODE
#define ASSERT(x) {if(x){}}
#else
#define ASSERT(x) assert(x)
#endif

#endif
