/**
 * Autogenerated by Thrift Compiler (0.8.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "fb303_types.h"

namespace facebook { namespace fb303 {

int _kfb_statusValues[] = {
  fb_status::DEAD,
  fb_status::STARTING,
  fb_status::ALIVE,
  fb_status::STOPPING,
  fb_status::STOPPED,
  fb_status::WARNING
};
const char* _kfb_statusNames[] = {
  "DEAD",
  "STARTING",
  "ALIVE",
  "STOPPING",
  "STOPPED",
  "WARNING"
};
const std::map<int, const char*> _fb_status_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(6, _kfb_statusValues, _kfb_statusNames), ::apache::thrift::TEnumIterator(-1, NULL, NULL));

}} // namespace
