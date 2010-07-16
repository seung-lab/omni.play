/*
 * to compile (from this directory):
 *   $ g++ zargs.cpp example.cpp -I../../ -o ziArgsExample
 *
 * to figure out how to provide the arguments run
 *   $ ./ziArgsExample -h
 */

#include "zi/zargs/zargs.h"
#include <iostream>

using std::cout;

ZiARG_bool(      someBool,        true, "Sample bool argument");
ZiARG_bool(   anotherBool,       false, "Another bool argument");
ZiARG_int32(    someInt32,           0, "Sample int32 argument");
ZiARG_int64(    someInt64,           1, "Sample int64 argument");
ZiARG_uint64(  someUInt64,           2, "Sample uint64 argument");
ZiARG_double(  someDouble,         3.0, "Sample double argument");
ZiARG_float(    someFloat,         4.0, "Sample float argument");
ZiARG_string(  someString,  "string_5", "Sample string argument");

int main(int argc, char **argv) {

  // the third parameter indicates whether to delete the processed args
  // from the argc/argv (the rest of the args is left there

  zi::Args::ZiArguments::ParseArgs(argc, argv, true);

  cout << "Some bool:     " << ZiArg_someBool << "\n";
  cout << "Another bool:  " << ZiArg_anotherBool << "\n";
  cout << "Some int32:    " << ZiArg_someInt32 << "\n";
  cout << "Some int64:    " << ZiArg_someInt64 << "\n";
  cout << "Some uint64:   " << ZiArg_someUInt64 << "\n";
  cout << "Some double:   " << ZiArg_someDouble << "\n";
  cout << "Some float:    " << ZiArg_someFloat << "\n";
  cout << "Some string:   " << ZiArg_someString << "\n";

}
