#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "omTileCoord.h"
#include "omTextureID.h"


class Drawable {
public:
        Drawable (int x, int y, int tileLength, 
		  OmTileCoord tileCoord, float zoomFactor, 
		  OmTextureIDPtr gotten_id);

	~Drawable ();

        int x;
	int y;
	int tileLength;
	OmTileCoord tileCoord;
	float zoomFactor;
        OmTextureIDPtr gotten_id;

	bool mGood;
};

#endif
