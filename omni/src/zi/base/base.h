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

#define FOR_EACH(it_var, container)                                     \
  FOR_EACH_RANGE(it_var, (container).begin(), (container).end())

#define FOR_EACH_R(it_var, container)                                   \
  FOR_EACH_RANGE(it_var, (container).rbegin(), (container).rend())

#define FOR_EACH_RANGE(it_var, begin, end)                              \
  for (__typeof__(begin) it_var = (begin); it_var != (end); ++it_var)

#define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))

#define ITERATOR(container) __typeof__((container).begin())

#ifdef LIVECODE
#define ASSERT(x) {if(x){}}
#else
#define ASSERT(x) assert(x)
#endif

#endif
