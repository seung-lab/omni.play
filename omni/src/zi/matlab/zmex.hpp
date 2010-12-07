/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef _ZI_MATLAB_ZMEX_
#define _ZI_MATLAB_ZMEX_

#include <string>
#include <cmath>
#include <boost/lexical_cast.hpp>

#include <zi/utility/for_each.hpp>
#include <zi/bits/cstdint.hpp>
#include <zi/time.hpp>

#include "mex.h"

template <typename T> struct MexType {
  static mxClassID CLASS;
  static bool CHECK(const mxArray *pa);
};

template<typename T> mxClassID MexType<T>::CLASS = mxUNKNOWN_CLASS;
template<typename T>
bool MexType<T>::CHECK(const mxArray *pa) { return false; }

#define MEX_TYPE_SPEC_CLASS(_DTYPE_, _MXTYPE_, _MXCFN_)         \
  template <> struct MexType<_DTYPE_> {                         \
    static mxClassID CLASS;                                     \
    static bool CHECK(const mxArray *pa);                       \
  };                                                            \
  mxClassID MexType<_DTYPE_>::CLASS = _MXTYPE_;                 \
  bool MexType<_DTYPE_>::CHECK(const mxArray *pa) {             \
    return _MXCFN_(pa);                                         \
  }                                                             \


MEX_TYPE_SPEC_CLASS(bool,     mxLOGICAL_CLASS, mxIsLogical)
MEX_TYPE_SPEC_CLASS(int,      mxINT32_CLASS,   mxIsInt32)
MEX_TYPE_SPEC_CLASS(uint32_t, mxUINT32_CLASS,  mxIsUint32)
MEX_TYPE_SPEC_CLASS(int64_t,  mxINT64_CLASS,   mxIsInt64)
MEX_TYPE_SPEC_CLASS(uint64_t, mxUINT64_CLASS,  mxIsUint64)
MEX_TYPE_SPEC_CLASS(float,    mxSINGLE_CLASS,  mxIsSingle)
MEX_TYPE_SPEC_CLASS(double,   mxDOUBLE_CLASS,  mxIsDouble)

template <typename T>
T getMexScalar(const mxArray *prhs) {
  double d = mxGetScalar(prhs);
  return boost::lexical_cast<T>(d);
}

#define DECLARE_MEX(_name_)                     \
    static zi::timer::wall zTimer;		        \
    static int    mexExecCount = 0;                     \
    static std::string S_MEX_NAME = #_name_

#define MEXINIT()                                               \
  do {                                                          \
    mexExecCount++;                                             \
    zTimer.reset();                                             \
    MEXPRINTF("!gooD asdfasdf moorninG # %d time", mexExecCount);        \
    MEXFLUSH();                                                 \
  } while(0)

#define MEXPRINTF(...)                                          \
  do {                                                          \
    mexPrintf("%s (%0.3f): ", S_MEX_NAME.c_str(),               \
              zTimer.elapsed< double >());                        \
    mexPrintf(__VA_ARGS__);                                     \
    mexPrintf("\n");                                            \
  } while(0)

#define MEXPRINTFFLUSH(...)                                     \
  do {                                                          \
    MEXPRINTF(__VA_ARGS__);                                     \
    MEXFLUSH();                                                 \
  } while(0)

#define MEXFLUSH()     mexEvalString("drawnow\n")

#define DECLARE_MEX_ARRAY_VARS(_VAR_, _PRHS_, _DTYPE_)                  \
  const mxArray *_VAR_               = _PRHS_;                          \
  if (!MexType<_DTYPE_>::CHECK(_PRHS_))                                 \
    mexErrMsgTxt("Argument " # _PRHS_ " is not " # _DTYPE_);            \
  const mwSize   _VAR_ ## NoDims     = mxGetNumberOfDimensions(_VAR_);  \
  const mwSize  *_VAR_ ## Dims       = mxGetDimensions(_VAR_);          \
  const mwSize   _VAR_ ## NoElements = mxGetNumberOfElements(_VAR_);    \
  const _DTYPE_ *_VAR_ ## Data       = (const _DTYPE_ *)mxGetData(_VAR_)

#define DECLARE_MEX_SCALAR(_VAR_, _PRHS_, _DTYPE_)              \
  const _DTYPE_ _VAR_ = (const _DTYPE_)mxGetScalar(_PRHS_)

#define DECLARE_MEX_DOUBLE(_VAR_, _PRHS_)       \
  DECLARE_MEX_SCALAR(_VAR_, _PRHS_, double)

#define DECLARE_MEX_INT(_VAR_, _PRHS_)          \
  DECLARE_MEX_SCALAR(_VAR_, _PRHS_, int)

#define MEX_ASSERT(_BE_TRUE_, _MSG_)            \
  if (!(_BE_TRUE_)) mexErrMsgTxt(_MSG_)

#define DECLARE_MEX_RET_ARRAY(_VAR_, _PLHS_, _DTYPE_, ...)              \
  mwSize _VAR_ ## Dims[] = { __VA_ARGS__ };                             \
  mwSize _VAR_ ## NoDims       = ARRAYSIZE(_VAR_ ## Dims);              \
  _PLHS_ = mxCreateNumericArray(_VAR_ ## NoDims, _VAR_ ## Dims,         \
                                MexType<_DTYPE_>::CLASS, mxREAL);       \
  if (_PLHS_ == NULL) mexErrMsgTxt("Unable to output" # _PLHS_);        \
  _DTYPE_ *_VAR_ ## Data = (_DTYPE_*)mxGetData(_PLHS_)

#define DECLARE_MEX_RET_ARRAY_V(_VAR_, _PLHS_, _NDIM_, _DIMS_, _DTYPE_) \
  _PLHS_ = mxCreateNumericArray(_NDIM_, _DIMS_,                         \
                                MexType<_DTYPE_>::CLASS, mxREAL);       \
  if (_PLHS_ == NULL) mexErrMsgTxt("Unable to output" # _PLHS_);        \
  mwSize _VAR_ ## NoElements = mxGetNumberOfElements(_PLHS_);           \
  _DTYPE_ *_VAR_ ## Data = (_DTYPE_*)mxGetData(_PLHS_)


#define MEX_RET_SCALAR(_VAR_)                   \
  mxCreateDoubleScalar((double)_VAR_);

#define MEX_ASSERT_NDIM(_VAR_, _NDIM_)                  \
  MEX_ASSERT(_VAR_ ## NoDims == _NDIM_,                 \
             #_VAR_ " doesn't have " #_NDIM_ " dims")

#define MEX_ASSERT_DIM(_VAR_, _DIM_, _DIMSZE_)          \
  MEX_ASSERT(_VAR_ ## Dims[_DIM_ - 1 ] == _DIMSZE_,     \
             #_VAR_ " [" #_DIM_ "] != " #_DIMSZE_)

#define MEX_ASSERT_NELEMENTS(_VAR_, _NELEM_)                    \
  MEX_ASSERT(_VAR_ ## NoElements == _NELEM_,                    \
             #_VAR_ " doesn't have " #_NELEM_ " elems")


#define MEX_ASSERT_EQ(_VAR1_, _VAR2_)                   \
  MEX_ASSERT(_VAR1_ == _VAR2_, #_VAR1_ " != " #_VAR2_)

#define DECLARE_MEX_RET_CELL(_VAR_, _PLHS_, ...)        \
  mwSize _VAR_ ## Dims[] = { __VA_ARGS__ };             \
  mwSize _VAR_ ## NoDims = ARRAYSIZE(_VAR_ ## Dims);    \
  _PLHS_ = mxCreateCellArray(_VAR_ ## NoDims,           \
                             _VAR_ ## Dims);            \
  if (_PLHS_ == NULL)                                   \
    mexErrMsgTxt("Unable to output" # _PLHS_);          \
  mxArray* _VAR_ = _PLHS_

#define MAKE_MEX_ARRAY(_VAR_, _DTYPE_, ...)                             \
  mwSize _VAR_ ## Dims[] = { __VA_ARGS__ };                             \
  mwSize _VAR_ ## NoDims = ARRAYSIZE(_VAR_ ## Dims);                    \
  mxArray* _VAR_ = mxCreateNumericArray(_VAR_ ## NoDims,                \
                                        _VAR_ ## Dims,                  \
                                        MexType<_DTYPE_>::CLASS,        \
                                        mxREAL);                        \
  const mwSize _VAR_ ## NoElements = mxGetNumberOfElements(_VAR_);      \
  if (_VAR_ == NULL) mexErrMsgTxt("Unable to output" # _VAR_);          \
  _DTYPE_ *_VAR_ ## Data = (_DTYPE_*)mxGetData(_VAR_)


#define MEX_RETURN do { MEXPRINTF("DONE"); return; } while(0)

std::string getMexString(const mxArray *prhs) {
  std::string ret("");
  if (!mxIsChar(prhs)) return ret;
  mxChar *chars = mxGetChars(prhs);
  int len = mxGetNumberOfElements(prhs);
  ret.resize(len);
  for (int i=0; i<len; i++)
    ret[i] = (unsigned char)(chars[i] % 256);
  return ret;
}

#define VEC2MEX_ARRAY(_VAR_, _DTYPE_, _VECT_)                           \
  MAKE_MEX_ARRAY(_VAR_, _DTYPE_, (_VECT_).size(), 1);                   \
  std::copy((_VECT_).begin(), (_VECT_).end(), _VAR_ ## Data)


#endif
