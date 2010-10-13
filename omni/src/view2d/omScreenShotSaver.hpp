#ifndef OM_SCREEN_SHOT_SAVER_HPP
#define OM_SCREEN_SHOT_SAVER_HPP

#include <stdio.h>
#include <QImage>

class OmScreenShotSaver{
public:
	OmScreenShotSaver()
		: mSlide(0)
		, mEmitMovie(false)
	{}

	void toggleEmitMovie(){
		mEmitMovie = !mEmitMovie;
	}

	void saveTiles(const QImage& screenImage)
	{
		if(!mEmitMovie) {
			return;
		}

		const QString file = QString("omniss-%1.png").arg(mSlide);
		if (!screenImage.save(file, "png")){
			printf("could not save file %s\n", qPrintable(file));
		}
		++mSlide;
	}

private:
        unsigned int mSlide;
        bool mEmitMovie;
};

#endif
