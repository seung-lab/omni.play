/// @cond DOXYGEN_IGNORE

#ifndef _OM_COLORMAP_H_
#define _OM_COLORMAP_H_

#include <QColor>

class OmColormap : public OmManageableObject {
public:

	void Load (char * file);
	void Save (char * file);
	QColor GetColor (SEGMENT_DATA_TYPE id);
	void SetColor (SEGMENT_DATA_TYPE id, QColor color);

private:
	set <OmColormapperRule> mColormapperRules;

};

//	ar & mColormapperRules;



#endif

/// @endcond
