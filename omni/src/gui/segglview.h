#ifndef SEGGLVIEW_H
#define SEGGLVIEW_H

#include "common/omStd.h"
#include "volume/omVolumeTypes.h"
#include "volume/omMipChunkCoord.h"

#include <QGLWidget>
#include <QMap>
#include <QtGui/QKeyEvent>

#include "system/omGenericManager.h"

#include <fstream>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <boost/tuple/tuple_comparison.hpp>
using boost::tuple;

class OmMipVolume;
class OmSegmentation;
class OmChannel;

class SegGLview:public QGLWidget {
 Q_OBJECT public:
	SegGLview(QWidget * parent = 0);

	void setView(string ortho_view);
	void setIDs(OmId vol, OmId chan, OmId seg);
	void setMipVolume(OmSegmentation & seg, OmChannel & chan);

	void update_xy_line(int dir);
	void update_xz_line(int dir);
	void update_yz_line(int dir);

	void view_zoom(int future_zoom);
	void view_scrub(int future_depth);

	string getView();

 protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintEvent(QPaintEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void keyPressEvent(QKeyEvent * event);

 private:
	void getVolumeProperties();
	void extract_data();
	void extract_data(int desired_depth);
	void extract_data_initial();
	bool check_depth(int future_depth);
	int setTexture(char *imageData, Vector2 < int >dims, const OmMipChunkCoord & mipCoord, int depth);
	int setSegTexture(char *imageData, char *channelData, Vector2 < int >dims, const OmMipChunkCoord & mipCoord,
			  int depth);
	void draw();

	void view_pan(int dx, int dy, int screencoords);

	void animate_cursors();
	void view_set_cursor(int x, int y);

	void update_viewport();
	DataBbox merge_bounding_boxes(DataBbox desired_data);
	DataBbox merge_bounding_boxes(DataBbox desired_data, double desired_level);

	void setMyColorMap(char *imageData, char *channelData, Vector2 < int >dims, const OmMipChunkCoord & mipCoord,
			   int depth);

	struct rect {
		double x, y;
		double w, h;
	};

	struct the_view {
		struct rect screen;	/* Screen coordinates */
		double aspect;	/* Screen aspect ration w/h */
		struct rect world_camera;	/* World user panned/zoomed to */
		struct rect shadow_camera;
		double cursor_x, cursor_y;	/* Center world coordinates for zooming */
	};

	struct the_view *view;

	// variables for mouse event positions
	int startx, starty;
	int dragx, dragy;

	int panning, zooming;

	// clipping plane variables
	int near, far;

	// orthogonal view
	string ortho;

	// tile settings
	int channel_tileLength;
	int seg_tileLength;

	// current level (0 is max zoom level)
	double level;
	// current depth (0 is top, closest to the viewer)
	int depth;

	// current VIEWPORT
	AxisAlignedBoundingBox < int >viewport_bbox;

	// current center
	int center_x;
	int center_y;

	OmMipVolume *channel;
	OmMipVolume *segmentation;

	// key : OmMipChunkCoord
	// value: tile texture ID
	QMap < OmMipChunkCoord, int >channelmap;
	QMap < OmMipChunkCoord, int >segmap;

	OmId volume_id;
	OmId channel_id;
	OmId seg_id;

	Vector2 < int >xy_pos_start;
	Vector2 < int >xy_pos_end;

	Vector2 < int >xz_pos_start;
	Vector2 < int >xz_pos_end;

	Vector2 < int >yz_pos_start;
	Vector2 < int >yz_pos_end;

	DataBbox channel_extent;
	DataBbox segmentation_extent;

	int channel_max_level;
	int segmentation_max_level;

	bool hasbeenset;

	// SOURCE DATA PROPERTIES
	int chan_mBytesPerSample;
	int chan_mSamplesPerVoxel;	// if 1 then grayscale, if 3 then color

	int seg_mBytesPerSample;
	int seg_mSamplesPerVoxel;	// if 1 then grayscale, if 3 then color

	QString location_string;

};

#endif
