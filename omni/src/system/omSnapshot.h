#ifndef OM_SNAPSHOT_H
#define OM_SNAPSHOT_H

/*
 *	Snapshot Object
 *	
 *	Matthew Wimer - mwimer@mit.edu - 11/13/09
 */

#include "system/omSystemTypes.h"
#include "system/omManageableObject.h"
#include "view2d/omThreadedCachingTile.h"
#include <QImage>

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;

class OmSnapshot {

 public:
	OmSnapshot(QImage &);
 private:
	static int const mCount = 0;
};

#endif
