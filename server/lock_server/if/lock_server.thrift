include "fb303/if/fb303.thrift"

namespace cpp omni.server

service lockServer extends fb303.FacebookService
{
  void acquire(1:string client_id, 2:string lock_id),
  void acquireRead(1:string client_id, 2:string lock_id),
  void acquireWrite(1:string client_id, 2:string lock_id),
  void release(1:string client_id, 2:string lock_id),
  void releaseRead(1:string client_id, 2:string lock_id),
  void releaseWrite(1:string client_id, 2:string lock_id),
  bool tryAcquireRead(1:string client_id, 2:string lock_id),
  bool tryAcquireWrite(1:string client_id, 2:string lock_id),
}