#include "segglview.h"

#include "volume/omMipVolume.h"
#include "volume/omVolumeManager.h"
#include "system/omException.h"

#include <QPainter>
#include "system/omDebug.h"

#define DEBUG 0

 SegGLview::SegGLview(QWidget * parent):QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::AlphaChannel),
	  parent)
{
	setFocusPolicy(Qt::ClickFocus);
	setMouseTracking(true);

	setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

	setMinimumWidth(200);
	setMinimumHeight(200);

	//      struct the_view *view;

	//      view = calloc(sizeof(view), 1);
	if ((view = (struct the_view *)malloc(sizeof(struct the_view))) == NULL) {
		printf("ERROR ALLOCATING view\n");
	}

	view->screen.x = 0;
	view->screen.y = 0;
	view->screen.w = 0;
	view->screen.h = 0;

	view->cursor_x = 0;
	view->cursor_y = 0;

	//      view->world_camera = view->world_req;
	view->world_camera = view->screen;
	view->shadow_camera = view->world_camera;

	dragx = startx = 0;
	dragy = starty = 0;

	panning = zooming = 0;

	near = -1;
	far = 1;

	level = 0;
	depth = 0;

	center_x = 0;
	center_y = 0;

	hasbeenset = false;

	update_viewport();

	//      cache.clear();

	location_string = "zoom level: ";
	location_string.append(QString::number(level));
	location_string.append("\n");
	location_string.append("stack position: ");
	location_string.append(QString::number(depth));

	setAutoFillBackground(false);

}

void SegGLview::setView(string ortho_view)
{

	ortho = ortho_view;

}

string SegGLview::getView()
{
	return ortho;
}

void SegGLview::setIDs(OmId vol, OmId chan, OmId seg)
{

	volume_id = vol;
	channel_id = chan;
	seg_id = seg;

}

void SegGLview::setMipVolume(OmSegmentation & seg, OmChannel & chan)
{
	segmentation = &seg;
	channel = &chan;
	hasbeenset = true;

	channel_tileLength = channel->GetChunkDimension();
	seg_tileLength = segmentation->GetChunkDimension();

	channel_extent = channel->GetSourceDataExtent();
	segmentation_extent = segmentation->GetSourceDataExtent();

	channel_max_level = (channel->GetRootLevel()) - 1;
	segmentation_max_level = (segmentation->GetRootLevel()) - 1;

	channel->GetMipDataFormat(chan_mSamplesPerVoxel, chan_mBytesPerSample);
	segmentation->GetMipDataFormat(seg_mSamplesPerVoxel, seg_mBytesPerSample);

	if (ortho == "xy") {
		xz_pos_start = Vector2 < int >(-100, 0);
		xz_pos_end = Vector2 < int >(channel_extent.getMax().x, 0);

		yz_pos_start = Vector2 < int >(0, -100);
		yz_pos_end = Vector2 < int >(0, channel_extent.getMax().y);
	}

	if (ortho == "xz") {
		xy_pos_start = Vector2 < int >(-100, 0);
		xy_pos_end = Vector2 < int >(channel_extent.getMax().x, 0);

		yz_pos_start = Vector2 < int >(0, -100);
		yz_pos_end = Vector2 < int >(0, channel_extent.getMax().y);
	}

	if (ortho == "yz") {
		xy_pos_start = Vector2 < int >(0, -100);
		xy_pos_end = Vector2 < int >(0, channel_extent.getMax().y);

		xz_pos_start = Vector2 < int >(-100, 0);
		xz_pos_end = Vector2 < int >(channel_extent.getMax().x, 0);
	}

}

void SegGLview::getVolumeProperties()
{

}

void SegGLview::initializeGL()
// The initializeGL() function is called just once, before paintGL() is called.
{
//      glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // White Background
	view->screen.w = size().width();
	view->screen.h = size().height();

	// use width and height to calculate center
	if (DEBUG)
		cout << "view->screen.w: " << view->screen.w << "     view->screen.h" << view->screen.h << endl;

	int center_w = (view->screen.w) / 2;
	int center_h = (view->screen.h) / 2;

	if (DEBUG)
		cout << "half view->screen.w: " << center_w << "     view->screen.h: " << center_h << endl;

	view->shadow_camera = view->screen;
	update_viewport();

	// Have omMipVolume *mipVol
	// get omMipChunk (0,0,0,0)
	if (DEBUG)
		cout << "hasbeenset: " << hasbeenset << endl;

	glMatrixMode(GL_MODELVIEW);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	extract_data_initial();
}

void SegGLview::extract_data()
{
	extract_data(depth);
}

void SegGLview::extract_data(int desired_depth)
{

	// viewport_bbox tells us current viewport
	// so, let's extract all the MipCoords that are contained within the current viewport

	//for(int mylevel = level ; mylevel <= level ; mylevel++) {

	// WE HAVE THE DESIRED DEPTH, CURRENT VIEWPORT, AND DATA EXTENT

	//      desired_depth = 150;

	DataBbox flat_data;
	if (ortho == "xy")
		flat_data =
		    DataBbox(Vector3 < int >(channel_extent.getMin().x, channel_extent.getMin().y, desired_depth),
			     Vector3 < int >(channel_extent.getMax().x, channel_extent.getMax().y, desired_depth));
	else if (ortho == "xz")
		flat_data =
		    DataBbox(Vector3 < int >(channel_extent.getMin().x, channel_extent.getMin().z, desired_depth),
			     Vector3 < int >(channel_extent.getMax().x, channel_extent.getMax().z, desired_depth));
	else
		flat_data =
		    DataBbox(Vector3 < int >(channel_extent.getMin().z, channel_extent.getMin().y, desired_depth),
			     Vector3 < int >(channel_extent.getMax().z, channel_extent.getMax().y, desired_depth));

	//      for(int i = 0 ; i < 3 ; i++) {

	//              double fake_level = i * 1.0;    
	//              
	//              cout << "level: " << fake_level << endl;

	// STEP 1.  Figure out new bounding box for data retrieval
	DataBbox data_to_retrieve;
	//      if(mylevel != 0)
	//              data_to_retrieve = merge_bounding_boxes(flat_data);
	//      else
	data_to_retrieve = merge_bounding_boxes(flat_data);
	if (DEBUG) {

		cout << "LEVEL = " << level << endl;
		cout << "viewport = " << viewport_bbox << endl;
		cout << "flat_data = " << flat_data << endl;
		cout << "data_to_retrieve = " << data_to_retrieve << endl;
	}

	DataBbox adjusted_data_to_retrieve;
	if (ortho == "xy")
		adjusted_data_to_retrieve =
		    DataBbox(Vector3 <
			     int >(data_to_retrieve.getMin().x, data_to_retrieve.getMin().y,
				   data_to_retrieve.getMin().z), Vector3 < int >(data_to_retrieve.getMax().x,
										 data_to_retrieve.getMax().y,
										 data_to_retrieve.getMax().z));
	else if (ortho == "xz")
		adjusted_data_to_retrieve =
		    DataBbox(Vector3 <
			     int >(data_to_retrieve.getMin().x, data_to_retrieve.getMin().z,
				   data_to_retrieve.getMin().y), Vector3 < int >(data_to_retrieve.getMax().x,
										 data_to_retrieve.getMax().z,
										 data_to_retrieve.getMax().y));
	else
		adjusted_data_to_retrieve =
		    DataBbox(Vector3 <
			     int >(data_to_retrieve.getMin().z, data_to_retrieve.getMin().y,
				   data_to_retrieve.getMin().x), Vector3 < int >(data_to_retrieve.getMax().z,
										 data_to_retrieve.getMax().y,
										 data_to_retrieve.getMax().x));

	if (DEBUG)
		cout << "adjusted_data_to_retrieve = " << adjusted_data_to_retrieve << endl;

	// STEP 5.  Figure out start and finish OmMipChunkCoords.
	OmMipChunkCoord start_mipcoord;
	OmMipChunkCoord end_mipcoord;

	int mylevel;
	if (level < 0)
		mylevel = 0;
	else if (level == 0)
		mylevel = level;
	else
		mylevel = 0;

	start_mipcoord = channel->DataToMipCoord(mylevel, adjusted_data_to_retrieve.getMin());
	end_mipcoord = channel->DataToMipCoord(mylevel, adjusted_data_to_retrieve.getMax());

	if (DEBUG)
		cout << "find mipcoords " << start_mipcoord << " -----> " << end_mipcoord << endl;

	// STEP 5.  Check if slices are already in mastermap, if not then load them in
	if (ortho == "xy") {

		for (int y = start_mipcoord.get < 2 > (); y <= end_mipcoord.get < 2 > (); y++) {

			for (int x = start_mipcoord.get < 1 > (); x <= end_mipcoord.get < 1 > (); x++) {
				for (int i = 0; i <= 0; i++) {

					if (!(channelmap.contains(OmMipChunkCoord(level, x, y, desired_depth + i)))) {
						if (DEBUG)
							cout << "mastermap does not contain " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

						shared_ptr < OmMipChunk > my_chunk = shared_ptr < OnMipChunk > ();
						channel->GetChunk(my_chunk,
								  (OmMipChunkCoord
								   (mylevel, x, y, end_mipcoord.get < 3 > ())));
						//                              cout << "self coord of the chunk: " << my_chunk->mSelfCoord << endl;
						//                              cout << "location: " << mipvol->ChunkDirectoryPath(mip_coord) << endl;

						int actual_depth = (desired_depth + i) % channel_tileLength;

						if (DEBUG)
							cout << "actual_depth = " << actual_depth << endl;

						Vector2 < int >tile_dims;
						void *void_data;
						void_data =
						    my_chunk->ExtractDataSlice(VOL_XY_PLANE, actual_depth, tile_dims);

						char *data = (char *)void_data;

						int tex_num =
						    setTexture(data, tile_dims,
							       OmMipChunkCoord(mylevel, x, y,
									       end_mipcoord.get < 3 > ()),
							       desired_depth + i);
						if (DEBUG)
							cout << "tile dims: " << tile_dims << endl;
						delete data;

						channelmap.insert((OmMipChunkCoord(level, x, y, desired_depth + i)),
								  tex_num);

						if (DEBUG)
							cout << "inserted " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
						//                              mastermap.insert(tex_num, OmMipChunkCoord(mip_coord.get<0>(), mip_coord.get<1>(), mip_coord.get<2>(), depth));                          
					}

					if (!(segmap.contains(OmMipChunkCoord(level, x, y, desired_depth + i)))) {
						if (DEBUG)
							cout << "mastermap does not contain " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

						shared_ptr < OmMipChunk > my_chunk = shared_ptr < OnMipChunk > ();
						segmentation->GetChunk(my_chunk,
								       (OmMipChunkCoord
									(mylevel, x, y, end_mipcoord.get < 3 > ())));
						//                              cout << "self coord of the chunk: " << my_chunk->mSelfCoord << endl;
						//                              cout << "location: " << mipvol->ChunkDirectoryPath(mip_coord) << endl;
						shared_ptr < OmMipChunk > my_c_chunk = shared_ptr < OnMipChunk > ();
						channel->GetChunk(my_c_chunk,
								  (OmMipChunkCoord
								   (mylevel, x, y, end_mipcoord.get < 3 > ())));

						int actual_depth = (desired_depth + i) % seg_tileLength;

						if (DEBUG)
							cout << "actual_depth = " << actual_depth << endl;

						Vector2 < int >tile_dims;
						void *void_data;
						void *void_c_data;
						void_data =
						    my_chunk->ExtractDataSlice(VOL_XY_PLANE, actual_depth, tile_dims);
						void_c_data =
						    my_c_chunk->ExtractDataSlice(VOL_XY_PLANE, actual_depth, tile_dims);

						char *data = (char *)void_data;
						char *c_data = (char *)void_c_data;

						int tex_num =
						    setSegTexture(data, c_data, tile_dims,
								  OmMipChunkCoord(mylevel, x, y,
										  end_mipcoord.get < 3 > ()),
								  desired_depth + i);
						if (DEBUG)
							cout << "tile dims: " << tile_dims << endl;
						delete data;
						delete c_data;

						segmap.insert((OmMipChunkCoord(level, x, y, desired_depth + i)),
							      tex_num);

						if (DEBUG)
							cout << "inserted " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
						//                              mastermap.insert(tex_num, OmMipChunkCoord(mip_coord.get<0>(), mip_coord.get<1>(), mip_coord.get<2>(), depth));                          
					}

					else if (DEBUG) {

						cout << "DEPTH DEPTH DEPTH = " << desired_depth + i << endl;
						cout << "mastermap does contain " <<
						    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
					}
				}

			}
		}
	}

	else if (ortho == "xz") {

		for (int y = start_mipcoord.get < 3 > (); y <= end_mipcoord.get < 3 > (); y++) {

			for (int x = start_mipcoord.get < 1 > (); x <= end_mipcoord.get < 1 > (); x++) {
				for (int i = 0; i <= 0; i++) {
					if (!(channelmap.contains(OmMipChunkCoord(level, x, y, desired_depth + i)))) {

						if (DEBUG)
							cout << "mastermap does not contain " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

						shared_ptr < OmMipChunk > my_chunk = shared_ptr < OnMipChunk > ();
						channel->GetChunk(my_chunk,
								  (OmMipChunkCoord
								   (mylevel, x, end_mipcoord.get < 2 > (), y)));
						//                              cout << "self coord of the chunk: " << my_chunk->mSelfCoord << endl;
						//                              cout << "location: " << mipvol->ChunkDirectoryPath(mip_coord) << endl;

						int actual_depth = (desired_depth + i) % channel_tileLength;

						if (DEBUG)
							cout << "actual_depth = " << actual_depth << endl;

						Vector2 < int >tile_dims;
						void *void_data;
						void_data =
						    my_chunk->ExtractDataSlice(VOL_XZ_PLANE, actual_depth, tile_dims);

						char *data = (char *)void_data;

						int tex_num =
						    setTexture(data, tile_dims,
							       OmMipChunkCoord(mylevel, x, end_mipcoord.get < 2 > (),
									       y), desired_depth + i);
						if (DEBUG)
							cout << "tile dims: " << tile_dims << endl;
						delete data;

						channelmap.insert((OmMipChunkCoord(level, x, y, desired_depth + i)),
								  tex_num);

						if (DEBUG)
							cout << "inserted " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
						//                              mastermap.insert(tex_num, OmMipChunkCoord(mip_coord.get<0>(), mip_coord.get<1>(), mip_coord.get<2>(), depth));                          

					}
					if (!(segmap.contains(OmMipChunkCoord(level, x, y, desired_depth + i)))) {

						if (DEBUG)
							cout << "mastermap does not contain " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

						shared_ptr < OmMipChunk > my_chunk = shared_ptr < OnMipChunk > ();
						segmentation->GetChunk(my_chunk,
								       (OmMipChunkCoord
									(mylevel, x, end_mipcoord.get < 2 > (), y)));
						//                              cout << "self coord of the chunk: " << my_chunk->mSelfCoord << endl;
						//                              cout << "location: " << mipvol->ChunkDirectoryPath(mip_coord) << endl;
						shared_ptr < OmMipChunk > my_c_chunk = shared_ptr < OnMipChunk > ();
						channel->GetChunk(my_c_chunk,
								  (OmMipChunkCoord
								   (mylevel, x, end_mipcoord.get < 2 > (), y)));

						int actual_depth = (desired_depth + i) % channel_tileLength;

						if (DEBUG)
							cout << "actual_depth = " << actual_depth << endl;

						Vector2 < int >tile_dims;
						void *void_data;
						void *void_c_data;
						void_data =
						    my_chunk->ExtractDataSlice(VOL_XZ_PLANE, actual_depth, tile_dims);
						void_c_data =
						    my_chunk->ExtractDataSlice(VOL_XZ_PLANE, actual_depth, tile_dims);

						char *data = (char *)void_data;
						char *c_data = (char *)void_c_data;

						int tex_num =
						    setSegTexture(data, c_data, tile_dims,
								  OmMipChunkCoord(mylevel, x, end_mipcoord.get < 2 > (),
										  y), desired_depth + i);
						if (DEBUG)
							cout << "tile dims: " << tile_dims << endl;
						delete data;
						delete c_data;

						segmap.insert((OmMipChunkCoord(level, x, y, desired_depth + i)),
							      tex_num);

						if (DEBUG)
							cout << "inserted " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
						//                              mastermap.insert(tex_num, OmMipChunkCoord(mip_coord.get<0>(), mip_coord.get<1>(), mip_coord.get<2>(), depth));                          

					} else if (DEBUG)
						cout << "mastermap does contain " <<
						    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

				}
			}
		}
	} else {

		for (int y = start_mipcoord.get < 2 > (); y <= end_mipcoord.get < 2 > (); y++) {

			for (int x = start_mipcoord.get < 3 > (); x <= end_mipcoord.get < 3 > (); x++) {

				for (int i = 0; i <= 0; i++) {
					if (!(channelmap.contains(OmMipChunkCoord(level, x, y, desired_depth + i)))) {

						if (DEBUG)
							cout << "mastermap does not contain " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

						shared_ptr < OmMipChunk > my_chunk = shared_ptr < OnMipChunk > ();
						channel->GetChunk(my_chunk,
								  (OmMipChunkCoord
								   (mylevel, end_mipcoord.get < 1 > (), y, x)));

						int actual_depth = (desired_depth + i) % channel_tileLength;
						if (DEBUG)
							cout << "actual_depth = " << actual_depth << endl;

						Vector2 < int >tile_dims;
						void *void_data;
						void_data =
						    my_chunk->ExtractDataSlice(VOL_YZ_PLANE, actual_depth, tile_dims);

						char *data = (char *)void_data;

						int tex_num =
						    setTexture(data, tile_dims,
							       (OmMipChunkCoord
								(mylevel, end_mipcoord.get < 1 > (), y, x)),
							       desired_depth + i);
						if (DEBUG)
							cout << "tile dims: " << tile_dims << endl;
						delete data;

						channelmap.insert((OmMipChunkCoord(level, x, y, desired_depth + i)),
								  tex_num);

						if (DEBUG)
							cout << "inserted " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
						//                              mastermap.insert(tex_num, OmMipChunkCoord(mip_coord.get<0>(), mip_coord.get<1>(), mip_coord.get<2>(), depth));                          

					}
					if (!(segmap.contains(OmMipChunkCoord(level, x, y, desired_depth + i)))) {

						if (DEBUG)
							cout << "mastermap does not contain " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

						shared_ptr < OmMipChunk > my_chunk = shared_ptr < OnMipChunk > ();
						segmentation->GetChunk(my_chunk,
								       (OmMipChunkCoord
									(mylevel, end_mipcoord.get < 1 > (), y, x)));
						shared_ptr < OmMipChunk > my_c_chunk = shared_ptr < OnMipChunk > ();
						channel->GetChunk(my_c_chunk,
								  (OmMipChunkCoord
								   (mylevel, end_mipcoord.get < 1 > (), y, x)));

						int actual_depth = (desired_depth + i) % channel_tileLength;
						if (DEBUG)
							cout << "actual_depth = " << actual_depth << endl;

						Vector2 < int >tile_dims;
						void *void_data;
						void *void_c_data;
						void_data =
						    my_chunk->ExtractDataSlice(VOL_YZ_PLANE, actual_depth, tile_dims);
						void_c_data =
						    my_c_chunk->ExtractDataSlice(VOL_YZ_PLANE, actual_depth, tile_dims);

						char *data = (char *)void_data;
						char *c_data = (char *)void_c_data;

						int tex_num =
						    setSegTexture(data, c_data, tile_dims,
								  (OmMipChunkCoord
								   (mylevel, end_mipcoord.get < 1 > (), y, x)),
								  desired_depth + i);
						if (DEBUG)
							cout << "tile dims: " << tile_dims << endl;
						delete data;
						delete c_data;

						segmap.insert((OmMipChunkCoord(level, x, y, desired_depth + i)),
							      tex_num);

						if (DEBUG)
							cout << "inserted " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
						//                              mastermap.insert(tex_num, OmMipChunkCoord(mip_coord.get<0>(), mip_coord.get<1>(), mip_coord.get<2>(), depth));                          

					} else if (DEBUG)
						cout << "mastermap does contain " <<
						    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

				}
			}
		}

	}

	//                              int tex_num = setTexture(data, tile_dims);

	//}     

	//level = 0;
}

void SegGLview::extract_data_initial()
{
	int desired_depth = 0;

	// viewport_bbox tells us current viewport
	// so, let's extract all the MipCoords that are contained within the current viewport

	//for(int mylevel = level ; mylevel <= level ; mylevel++) {

	// WE HAVE THE DESIRED DEPTH, CURRENT VIEWPORT, AND DATA EXTENT

	//      desired_depth = 150;

	DataBbox flat_data;
	if (ortho == "xy")
		flat_data =
		    DataBbox(Vector3 < int >(channel_extent.getMin().x, channel_extent.getMin().y, desired_depth),
			     Vector3 < int >(channel_extent.getMax().x, channel_extent.getMax().y, desired_depth));
	else if (ortho == "xz")
		flat_data =
		    DataBbox(Vector3 < int >(channel_extent.getMin().x, channel_extent.getMin().z, desired_depth),
			     Vector3 < int >(channel_extent.getMax().x, channel_extent.getMax().z, desired_depth));
	else
		flat_data =
		    DataBbox(Vector3 < int >(channel_extent.getMin().z, channel_extent.getMin().y, desired_depth),
			     Vector3 < int >(channel_extent.getMax().z, channel_extent.getMax().y, desired_depth));

	//      for(int i = 0 ; i < 3 ; i++) {

	//              double fake_level = i * 1.0;    
	//              
	//              cout << "level: " << fake_level << endl;

	// STEP 1.  Figure out new bounding box for data retrieval
	DataBbox data_to_retrieve;
	//      if(mylevel != 0)
	//              data_to_retrieve = merge_bounding_boxes(flat_data);
	//      else
	data_to_retrieve = merge_bounding_boxes(flat_data);
	if (DEBUG) {

		cout << "LEVEL = " << level << endl;
		cout << "viewport = " << viewport_bbox << endl;
		cout << "flat_data = " << flat_data << endl;
		cout << "data_to_retrieve = " << data_to_retrieve << endl;
	}

	DataBbox adjusted_data_to_retrieve;
	if (ortho == "xy")
		adjusted_data_to_retrieve =
		    DataBbox(Vector3 <
			     int >(data_to_retrieve.getMin().x, data_to_retrieve.getMin().y,
				   data_to_retrieve.getMin().z), Vector3 < int >(data_to_retrieve.getMax().x,
										 data_to_retrieve.getMax().y,
										 data_to_retrieve.getMax().z));
	else if (ortho == "xz")
		adjusted_data_to_retrieve =
		    DataBbox(Vector3 <
			     int >(data_to_retrieve.getMin().x, data_to_retrieve.getMin().z,
				   data_to_retrieve.getMin().y), Vector3 < int >(data_to_retrieve.getMax().x,
										 data_to_retrieve.getMax().z,
										 data_to_retrieve.getMax().y));
	else
		adjusted_data_to_retrieve =
		    DataBbox(Vector3 <
			     int >(data_to_retrieve.getMin().z, data_to_retrieve.getMin().y,
				   data_to_retrieve.getMin().x), Vector3 < int >(data_to_retrieve.getMax().z,
										 data_to_retrieve.getMax().y,
										 data_to_retrieve.getMax().x));

	if (DEBUG)
		cout << "adjusted_data_to_retrieve = " << adjusted_data_to_retrieve << endl;

	// STEP 5.  Figure out start and finish OmMipChunkCoords.
	OmMipChunkCoord start_mipcoord;
	OmMipChunkCoord end_mipcoord;

	int mylevel;
	if (level < 0)
		mylevel = 0;
	else if (level == 0)
		mylevel = level;
	else
		mylevel = 0;

	start_mipcoord = channel->DataToMipCoord(mylevel, adjusted_data_to_retrieve.getMin());
	end_mipcoord = channel->DataToMipCoord(mylevel, adjusted_data_to_retrieve.getMax());

	if (DEBUG)
		cout << "find mipcoords " << start_mipcoord << " -----> " << end_mipcoord << endl;

	// STEP 5.  Check if slices are already in mastermap, if not then load them in
	if (ortho == "xy") {

		for (int y = start_mipcoord.get < 2 > (); y <= end_mipcoord.get < 2 > (); y++) {

			for (int x = start_mipcoord.get < 1 > (); x <= end_mipcoord.get < 1 > (); x++) {
				for (int i = 0; i <= 0; i++) {

					if (!(channelmap.contains(OmMipChunkCoord(level, x, y, desired_depth + i)))) {
						if (DEBUG)
							cout << "mastermap does not contain " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

						shared_ptr < OmMipChunk > my_chunk = shared_ptr < OnMipChunk > ();
						channel->GetChunk(my_chunk,
								  (OmMipChunkCoord
								   (mylevel, x, y, end_mipcoord.get < 3 > ())));
						//                              cout << "self coord of the chunk: " << my_chunk->mSelfCoord << endl;
						//                              cout << "location: " << mipvol->ChunkDirectoryPath(mip_coord) << endl;

						int actual_depth = (desired_depth + i) % channel_tileLength;

						if (DEBUG)
							cout << "actual_depth = " << actual_depth << endl;

						Vector2 < int >tile_dims;
						void *void_data;
						void_data =
						    my_chunk->ExtractDataSlice(VOL_XY_PLANE, actual_depth, tile_dims);

						char *data = (char *)void_data;

						int tex_num =
						    setTexture(data, tile_dims,
							       OmMipChunkCoord(mylevel, x, y,
									       end_mipcoord.get < 3 > ()),
							       desired_depth + i);
						if (DEBUG)
							cout << "tile dims: " << tile_dims << endl;
						delete data;

						channelmap.insert((OmMipChunkCoord(level, x, y, desired_depth + i)),
								  tex_num);

						if (DEBUG)
							cout << "inserted " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
						//                              mastermap.insert(tex_num, OmMipChunkCoord(mip_coord.get<0>(), mip_coord.get<1>(), mip_coord.get<2>(), depth));                          
					}

					if (!(segmap.contains(OmMipChunkCoord(level, x, y, desired_depth + i)))) {
						if (DEBUG)
							cout << "mastermap does not contain " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

						shared_ptr < OmMipChunk > my_chunk = shared_ptr < OnMipChunk > ();
						segmentation->GetChunk(my_chunk,
								       (OmMipChunkCoord
									(mylevel, x, y, end_mipcoord.get < 3 > ())));
						//                              cout << "self coord of the chunk: " << my_chunk->mSelfCoord << endl;
						//                              cout << "location: " << mipvol->ChunkDirectoryPath(mip_coord) << endl;
						shared_ptr < OmMipChunk > my_c_chunk = shared_ptr < OnMipChunk > ();
						channel->GetChunk(my_c_chunk,
								  (OmMipChunkCoord
								   (mylevel, x, y, end_mipcoord.get < 3 > ())));

						int actual_depth = (desired_depth + i) % seg_tileLength;

						if (DEBUG)
							cout << "actual_depth = " << actual_depth << endl;

						Vector2 < int >tile_dims;
						void *void_data;
						void *void_c_data;
						void_data =
						    my_chunk->ExtractDataSlice(VOL_XY_PLANE, actual_depth, tile_dims);
						void_c_data =
						    my_c_chunk->ExtractDataSlice(VOL_XY_PLANE, actual_depth, tile_dims);

						char *data = (char *)void_data;
						char *c_data = (char *)void_c_data;

						int tex_num =
						    setSegTexture(data, c_data, tile_dims,
								  OmMipChunkCoord(mylevel, x, y,
										  end_mipcoord.get < 3 > ()),
								  desired_depth + i);
						if (DEBUG)
							cout << "tile dims: " << tile_dims << endl;
						delete data;
						delete c_data;

						segmap.insert((OmMipChunkCoord(level, x, y, desired_depth + i)),
							      tex_num);

						if (DEBUG)
							cout << "inserted " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
						//                              mastermap.insert(tex_num, OmMipChunkCoord(mip_coord.get<0>(), mip_coord.get<1>(), mip_coord.get<2>(), depth));                          
					}

					else if (DEBUG) {

						cout << "DEPTH DEPTH DEPTH = " << desired_depth + i << endl;
						cout << "mastermap does contain " <<
						    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
					}
				}

			}
		}
	}

	else if (ortho == "xz") {

		for (int y = start_mipcoord.get < 3 > (); y <= end_mipcoord.get < 3 > (); y++) {

			for (int x = start_mipcoord.get < 1 > (); x <= end_mipcoord.get < 1 > (); x++) {
				for (int i = 0; i <= 0; i++) {
					if (!(channelmap.contains(OmMipChunkCoord(level, x, y, desired_depth + i)))) {

						if (DEBUG)
							cout << "mastermap does not contain " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

						shared_ptr < OmMipChunk > my_chunk = shared_ptr < OnMipChunk > ();
						channel->GetChunk(my_chunk,
								  (OmMipChunkCoord
								   (mylevel, x, end_mipcoord.get < 2 > (), y)));
						//                              cout << "self coord of the chunk: " << my_chunk->mSelfCoord << endl;
						//                              cout << "location: " << mipvol->ChunkDirectoryPath(mip_coord) << endl;

						int actual_depth = (desired_depth + i) % channel_tileLength;

						if (DEBUG)
							cout << "actual_depth = " << actual_depth << endl;

						Vector2 < int >tile_dims;
						void *void_data;
						void_data =
						    my_chunk->ExtractDataSlice(VOL_XZ_PLANE, actual_depth, tile_dims);

						char *data = (char *)void_data;

						int tex_num =
						    setTexture(data, tile_dims,
							       OmMipChunkCoord(mylevel, x, end_mipcoord.get < 2 > (),
									       y), desired_depth + i);
						if (DEBUG)
							cout << "tile dims: " << tile_dims << endl;
						delete data;

						channelmap.insert((OmMipChunkCoord(level, x, y, desired_depth + i)),
								  tex_num);

						if (DEBUG)
							cout << "inserted " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
						//                              mastermap.insert(tex_num, OmMipChunkCoord(mip_coord.get<0>(), mip_coord.get<1>(), mip_coord.get<2>(), depth));                          

					}
					if (!(segmap.contains(OmMipChunkCoord(level, x, y, desired_depth + i)))) {

						if (DEBUG)
							cout << "mastermap does not contain " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

						shared_ptr < OmMipChunk > my_chunk = shared_ptr < OnMipChunk > ();
						segmentation->GetChunk(my_chunk,
								       (OmMipChunkCoord
									(mylevel, x, end_mipcoord.get < 2 > (), y)));
						//                              cout << "self coord of the chunk: " << my_chunk->mSelfCoord << endl;
						//                              cout << "location: " << mipvol->ChunkDirectoryPath(mip_coord) << endl;
						shared_ptr < OmMipChunk > my_c_chunk = shared_ptr < OnMipChunk > ();
						channel->GetChunk(my_c_chunk,
								  (OmMipChunkCoord
								   (mylevel, x, end_mipcoord.get < 2 > (), y)));

						int actual_depth = (desired_depth + i) % channel_tileLength;

						if (DEBUG)
							cout << "actual_depth = " << actual_depth << endl;

						Vector2 < int >tile_dims;
						void *void_data;
						void *void_c_data;
						void_data =
						    my_chunk->ExtractDataSlice(VOL_XZ_PLANE, actual_depth, tile_dims);
						void_c_data =
						    my_chunk->ExtractDataSlice(VOL_XZ_PLANE, actual_depth, tile_dims);

						char *data = (char *)void_data;
						char *c_data = (char *)void_c_data;

						int tex_num =
						    setSegTexture(data, c_data, tile_dims,
								  OmMipChunkCoord(mylevel, x, end_mipcoord.get < 2 > (),
										  y), desired_depth + i);
						if (DEBUG)
							cout << "tile dims: " << tile_dims << endl;
						delete data;
						delete c_data;

						segmap.insert((OmMipChunkCoord(level, x, y, desired_depth + i)),
							      tex_num);

						if (DEBUG)
							cout << "inserted " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
						//                              mastermap.insert(tex_num, OmMipChunkCoord(mip_coord.get<0>(), mip_coord.get<1>(), mip_coord.get<2>(), depth));                          

					} else if (DEBUG)
						cout << "mastermap does contain " <<
						    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

				}
			}
		}
	} else {

		for (int y = start_mipcoord.get < 2 > (); y <= end_mipcoord.get < 2 > (); y++) {

			for (int x = start_mipcoord.get < 3 > (); x <= end_mipcoord.get < 3 > (); x++) {

				for (int i = 0; i <= 0; i++) {
					if (!(channelmap.contains(OmMipChunkCoord(level, x, y, desired_depth + i)))) {

						if (DEBUG)
							cout << "mastermap does not contain " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

						shared_ptr < OmMipChunk > my_chunk = shared_ptr < OnMipChunk > ();
						channel->GetChunk(my_chunk,
								  (OmMipChunkCoord
								   (mylevel, end_mipcoord.get < 1 > (), y, x)));

						int actual_depth = (desired_depth + i) % channel_tileLength;
						if (DEBUG)
							cout << "actual_depth = " << actual_depth << endl;

						Vector2 < int >tile_dims;
						void *void_data;
						void_data =
						    my_chunk->ExtractDataSlice(VOL_YZ_PLANE, actual_depth, tile_dims);

						char *data = (char *)void_data;

						int tex_num =
						    setTexture(data, tile_dims,
							       (OmMipChunkCoord
								(mylevel, end_mipcoord.get < 1 > (), y, x)),
							       desired_depth + i);
						if (DEBUG)
							cout << "tile dims: " << tile_dims << endl;
						delete data;

						channelmap.insert((OmMipChunkCoord(level, x, y, desired_depth + i)),
								  tex_num);

						if (DEBUG)
							cout << "inserted " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
						//                              mastermap.insert(tex_num, OmMipChunkCoord(mip_coord.get<0>(), mip_coord.get<1>(), mip_coord.get<2>(), depth));                          

					}
					if (!(segmap.contains(OmMipChunkCoord(level, x, y, desired_depth + i)))) {

						if (DEBUG)
							cout << "mastermap does not contain " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

						shared_ptr < OmMipChunk > my_chunk = shared_ptr < OnMipChunk > ();
						segmentation->GetChunk(my_chunk,
								       (OmMipChunkCoord
									(mylevel, end_mipcoord.get < 1 > (), y, x)));
						shared_ptr < OmMipChunk > my_c_chunk = shared_ptr < OnMipChunk > ();
						channel->GetChunk(my_c_chunk,
								  (OmMipChunkCoord
								   (mylevel, end_mipcoord.get < 1 > (), y, x)));

						int actual_depth = (desired_depth + i) % channel_tileLength;
						if (DEBUG)
							cout << "actual_depth = " << actual_depth << endl;

						Vector2 < int >tile_dims;
						void *void_data;
						void *void_c_data;
						void_data =
						    my_chunk->ExtractDataSlice(VOL_YZ_PLANE, actual_depth, tile_dims);
						void_c_data =
						    my_c_chunk->ExtractDataSlice(VOL_YZ_PLANE, actual_depth, tile_dims);

						char *data = (char *)void_data;
						char *c_data = (char *)void_c_data;

						int tex_num =
						    setSegTexture(data, c_data, tile_dims,
								  (OmMipChunkCoord
								   (mylevel, end_mipcoord.get < 1 > (), y, x)),
								  desired_depth + i);
						if (DEBUG)
							cout << "tile dims: " << tile_dims << endl;
						delete data;
						delete c_data;

						segmap.insert((OmMipChunkCoord(level, x, y, desired_depth + i)),
							      tex_num);

						if (DEBUG)
							cout << "inserted " <<
							    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;
						//                              mastermap.insert(tex_num, OmMipChunkCoord(mip_coord.get<0>(), mip_coord.get<1>(), mip_coord.get<2>(), depth));                          

					} else if (DEBUG)
						cout << "mastermap does contain " <<
						    (OmMipChunkCoord(level, x, y, desired_depth + i)) << endl;

				}
			}
		}

	}

	//                              int tex_num = setTexture(data, tile_dims);

	//}     

	//level = 0;
}

int SegGLview::setTexture(char *imageData, Vector2 < int >dims, const OmMipChunkCoord & mipCoord, int depth)
{

	// glEnable (GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLuint texture;
	// allocate a texture name
	glGenTextures(1, &texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	if (chan_mSamplesPerVoxel == 1)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, dims.x, dims.y, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
			     imageData);
	else if (chan_mSamplesPerVoxel == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dims.x, dims.y, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

	if (DEBUG)
		cout << "texture set. id = " << texture << endl;

	if (ortho == "yz") {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.5, 0.5, 0.0);
		glRotatef(-90, 0.0, 0.0, 1.0);
		glTranslatef(-0.5, -0.5, 0.0);
		glMatrixMode(GL_MODELVIEW);
	}
	return texture;
}

int SegGLview::setSegTexture(char *imageData, char *channelData, Vector2 < int >dims, const OmMipChunkCoord & mipCoord,
			     int depth)
{
	// glEnable (GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLuint texture;
	// allocate a texture name
	glGenTextures(1, &texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	setMyColorMap(imageData, channelData, dims, mipCoord, depth);
	//glTexImage2D (GL_TEXTURE_2D, 0, GL_LUMINANCE, dims.x, dims.y, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, imageData);

	if (DEBUG)
		cout << "texture set. id = " << texture << endl;

	if (ortho == "yz") {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.5, 0.5, 0.0);
		glRotatef(-90, 0.0, 0.0, 1.0);
		glTranslatef(-0.5, -0.5, 0.0);
		glMatrixMode(GL_MODELVIEW);
	}
	return texture;
}

void SegGLview::resizeGL(int width, int height)
// Set up the OpenGL viewport, projection, and any other settings that depend on the widget's size. 
{
	view->screen.w = width;
	view->screen.h = height;
	view->aspect = (double)width / (double)height;
	//      view->world_req.h = view->world_req.w / view->aspect;

	//      view->world_req = view->screen;
	if (level < 0) {

		view->world_camera.w = (width / (abs(level) * 2.0));
		view->world_camera.h = (height / (abs(level) * 2.0));

		view->shadow_camera.w = (width / (abs(level) * 2.0));
		view->shadow_camera.h = (height / (abs(level) * 2.0));
	} else if (level > 0) {
		view->world_camera.w = (width * (abs(level) * 2.0));
		view->world_camera.h = (height * (abs(level) * 2.0));

		view->shadow_camera.w = (width * (abs(level) * 2.0));
		view->shadow_camera.h = (height * (abs(level) * 2.0));
	} else {
		view->world_camera.w = width;
		view->world_camera.h = height;

		view->shadow_camera.w = width;
		view->shadow_camera.h = height;
	}

	update_viewport();
//      updateGL();
}

//void SegGLview::paintGL() 
//{     
//      
//      
//      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//      
//      glMatrixMode (GL_PROJECTION);
//      glViewport(view->screen.x, view->screen.y, view->screen.w, view->screen.h);
//    glLoadIdentity ();
//      
//      glOrtho(        view->world_camera.x,                   /* left */
//                      view->world_camera.x + view->world_camera.w,    /* right */
//                      view->world_camera.y + view->world_camera.h,    /* bottom */
//                      view->world_camera.y,                   /* top */
//                      near, far );
//      
//      
//      if(DEBUG)
//              cout << "DEPTH = " << depth << endl;
//      
//      
//      glMatrixMode(GL_MODELVIEW);
//      glLoadIdentity();
//      
//      
//      draw();
//      
//      
//      glMatrixMode(GL_PROJECTION);
//      glLoadIdentity();
//      glOrtho(0, view->screen.w, 0, view->screen.h, near, far);
//      
//      //      cout << "view->screen: (" << view->screen.x << ", " << view->screen.y << ")   "  << "w: " << view->screen.w << "   h: " << view->screen.h << endl;
//      //      cout << "view->camera: (" << view->world_camera.x << ", " << view->world_camera.y << ")   "  << "w: " << view->world_camera.w << "   h: " << view->world_camera.h << endl;
//      
//      if(DEBUG) {
//              
//              cout << "level = " << level << endl;
//              cout << "depth = " << depth << endl;
//      }
//      
//      //      cout << "viewport = " << viewport_bbox << endl;
//
//      extract_data();
//}

void SegGLview::paintEvent(QPaintEvent * event)
{
	makeCurrent();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glViewport(view->screen.x, view->screen.y, view->screen.w, view->screen.h);
	glLoadIdentity();

	glOrtho(view->world_camera.x,	/* left */
		view->world_camera.x + view->world_camera.w,	/* right */
		view->world_camera.y + view->world_camera.h,	/* bottom */
		view->world_camera.y,	/* top */
		near, far);

	if (DEBUG)
		cout << "DEPTH = " << depth << endl;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

//      cout << "ORTHO: " << ortho << endl;
//      cout << "viewport: " << viewport_bbox << endl;
//      

	// extract_data();
	draw();

	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, view->screen.w, 0, view->screen.h, near, far);

	//      cout << "view->screen: (" << view->screen.x << ", " << view->screen.y << ")   "  << "w: " << view->screen.w << "   h: " << view->screen.h << endl;
	//      cout << "view->camera: (" << view->world_camera.x << ", " << view->world_camera.y << ")   "  << "w: " << view->world_camera.w << "   h: " << view->world_camera.h << endl;

	if (DEBUG) {

		cout << "level = " << level << endl;
		cout << "depth = " << depth << endl;
	}
	//      cout << "viewport = " << viewport_bbox << endl;

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	QPainter painter(this);

	// Painting the text at the bottom and around the sides
	painter.fillRect(QRect(10, view->screen.h - 35, 250, 20), QColor(Qt::black));
	painter.setPen(QColor(Qt::white));
	painter.drawText(QPoint(20, view->screen.h - 20), location_string);

	QPen the_pen;
	if (ortho == "xy") {
		the_pen.setColor(QColor(Qt::blue));
	} else if (ortho == "yz") {
		the_pen.setColor(QColor(Qt::red));
	} else if (ortho == "xz") {
		the_pen.setColor(QColor(Qt::green));
	}

	if (hasFocus())
		the_pen.setWidth(5);

	painter.setPen(the_pen);

	painter.drawRect(view->screen.x, view->screen.y, view->screen.w - 1, view->screen.h - 1);

	painter.end();
}

void SegGLview::draw()
{

	if (ortho == "xy") {
		glColor3f(0.2F, 0.9F, 0.2F);
		glBegin(GL_LINES);
		glVertex3i(xz_pos_start.x, xz_pos_start.y, depth * 2);	// origin of the line
		glVertex3i(xz_pos_end.x, xz_pos_end.y, depth * 2);	// ending point of the line
		glEnd();

		glColor3f(0.9F, 0.2F, 0.2F);
		glBegin(GL_LINES);
		glVertex3i(yz_pos_start.x, yz_pos_start.y, depth * 2);	// origin of the line
		glVertex3i(yz_pos_end.x, yz_pos_end.y, depth * 2);	// ending point of the line
		glEnd();

	}

	if (ortho == "xz") {
		glColor3f(0.2F, 0.2F, 0.9F);
		glBegin(GL_LINES);
		glVertex3i(xy_pos_start.x, xy_pos_start.y, depth * 2);	// origin of the line
		glVertex3i(xy_pos_end.x, xy_pos_end.y, depth * 2);	// ending point of the line
		glEnd();

		glColor3f(0.9F, 0.2F, 0.2F);
		glBegin(GL_LINES);
		glVertex3i(yz_pos_start.x, yz_pos_start.y, depth * 2);	// origin of the line
		glVertex3i(yz_pos_end.x, yz_pos_end.y, depth * 2);	// ending point of the line
		glEnd();

	}

	if (ortho == "yz") {
		glColor3f(0.2F, 0.2F, 0.9F);
		glBegin(GL_LINES);
		glVertex3i(xy_pos_start.x, xy_pos_start.y, depth * 2);	// origin of the line
		glVertex3i(xy_pos_end.x, xy_pos_end.y, depth * 2);	// ending point of the line
		glEnd();

		glColor3f(0.2F, 0.9F, 0.2F);
		glBegin(GL_LINES);
		glVertex3i(xz_pos_start.x, xz_pos_start.y, depth * 2);	// origin of the line
		glVertex3i(xz_pos_end.x, xz_pos_end.y, depth * 2);	// ending point of the line
		glEnd();

	}
	// drawFace()

	glEnable(GL_TEXTURE_2D);	/* enable texture mapping */

//      QMapIterator<OmMipChunkCoord, int> i(channelmap);
//      while (i.hasNext()) {
//              
//              
//              i.next();
//              
//              if(i.key().get<0>() == level) {
//                      // cout << i.key() << ": " << i.value() << endl;
//                      
//                      glColor4f(1.0f,1.0f,1.0f,1.0f); 
//                      
//                      glBindTexture (GL_TEXTURE_2D, i.value());
//                      glBegin (GL_QUADS);
//                      
//                      
//                      //      cout << "tile width: " << the_tile.width << endl;
//                      //      cout << "tile height: " << the_tile.height << endl;
//                      
//                      //t.get<N>()
//                      // know coordinates are (0,0,0,1)
//                      //              if(ortho != "xz") {
//                      
//                      if(ortho != "yz") {
//                              
//                              glTexCoord2f (0.0f,0.0f); /* lower left corner of image */
//                              glVertex3f(i.key().get<1>() * channel_tileLength, i.key().get<2>() * channel_tileLength, (i.key().get<3>() * 2.0));
//                              glTexCoord2f (1.0f, 0.0f); /* lower right corner of image */
//                              glVertex3f((i.key().get<1>() * channel_tileLength + channel_tileLength), i.key().get<2>() * channel_tileLength, (i.key().get<3>() * 2.0));
//                              glTexCoord2f (1.0f, 1.0f); /* upper right corner of image */
//                              glVertex3f((i.key().get<1>() * channel_tileLength + channel_tileLength), (i.key().get<2>() * channel_tileLength + channel_tileLength),  (i.key().get<3>() * 2.0));
//                              glTexCoord2f (0.0f, 1.0f); /* upper left corner of image */
//                              glVertex3f(i.key().get<1>() * channel_tileLength, (i.key().get<2>() * channel_tileLength + channel_tileLength), (i.key().get<3>() * 2.0));
//                              glEnd ();
//                      }
//                      else if(ortho == "yz") {
//                              glTexCoord2f (0.0f,0.0f); /* lower left corner of image */
//                              glVertex3f((i.key().get<1>() * channel_tileLength + channel_tileLength), i.key().get<2>() * channel_tileLength, (i.key().get<3>() * 2.0));
//                              glTexCoord2f (1.0f, 0.0f); /* lower right corner of image */
//                              glVertex3f(i.key().get<1>() * channel_tileLength, i.key().get<2>() * channel_tileLength, (i.key().get<3>() * 2.0));
//                              glTexCoord2f (1.0f, 1.0f); /* upper right corner of image */
//                              glVertex3f(i.key().get<1>() * channel_tileLength, (i.key().get<2>() * channel_tileLength + channel_tileLength), (i.key().get<3>() * 2.0));
//                              glTexCoord2f (0.0f, 1.0f); /* upper left corner of image */
//                              glVertex3f((i.key().get<1>() * channel_tileLength + channel_tileLength), (i.key().get<2>() * channel_tileLength + channel_tileLength),  (i.key().get<3>() * 2.0));
//                              glEnd ();
//                      }
//                      
//                      
//                      // glFlush();
//                      glColor4f(1.0f,1.0f,1.0f,1.0f);
//                      //              }
//                      //      }
//              }
//      }

	QMapIterator < OmMipChunkCoord, int >is(segmap);
	while (is.hasNext()) {

		is.next();

		if (is.key().get < 0 > () == level) {

			// cout << i.key() << ": " << i.value() << endl;

			//                              glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

			glBindTexture(GL_TEXTURE_2D, is.value());
			glBegin(GL_QUADS);

			//      cout << "tile width: " << the_tile.width << endl;
			//      cout << "tile height: " << the_tile.height << endl;

			//t.get<N>()
			// know coordinates are (0,0,0,1)
			//              if(ortho != "xz") {

			if (ortho != "yz") {

				glTexCoord2f(0.0f, 0.0f);	/* lower left corner of image */
				glVertex3f(is.key().get < 1 > () * seg_tileLength,
					   is.key().get < 2 > () * seg_tileLength, (is.key().get < 3 > () * 2.0));
				glTexCoord2f(1.0f, 0.0f);	/* lower right corner of image */
				glVertex3f((is.key().get < 1 > () * seg_tileLength + seg_tileLength),
					   is.key().get < 2 > () * seg_tileLength, (is.key().get < 3 > () * 2.0));
				glTexCoord2f(1.0f, 1.0f);	/* upper right corner of image */
				glVertex3f((is.key().get < 1 > () * seg_tileLength + seg_tileLength),
					   (is.key().get < 2 > () * seg_tileLength + seg_tileLength),
					   (is.key().get < 3 > () * 2.0));
				glTexCoord2f(0.0f, 1.0f);	/* upper left corner of image */
				glVertex3f(is.key().get < 1 > () * seg_tileLength,
					   (is.key().get < 2 > () * seg_tileLength + seg_tileLength),
					   (is.key().get < 3 > () * 2.0));
				glEnd();
			} else if (ortho == "yz") {
				glTexCoord2f(0.0f, 0.0f);	/* lower left corner of image */
				glVertex3f((is.key().get < 1 > () * seg_tileLength + seg_tileLength),
					   is.key().get < 2 > () * seg_tileLength, (is.key().get < 3 > () * 2.0));
				glTexCoord2f(1.0f, 0.0f);	/* lower right corner of image */
				glVertex3f(is.key().get < 1 > () * seg_tileLength,
					   is.key().get < 2 > () * seg_tileLength, (is.key().get < 3 > () * 2.0));
				glTexCoord2f(1.0f, 1.0f);	/* upper right corner of image */
				glVertex3f(is.key().get < 1 > () * seg_tileLength,
					   (is.key().get < 2 > () * seg_tileLength + seg_tileLength),
					   (is.key().get < 3 > () * 2.0));
				glTexCoord2f(0.0f, 1.0f);	/* upper left corner of image */
				glVertex3f((is.key().get < 1 > () * seg_tileLength + seg_tileLength),
					   (is.key().get < 2 > () * seg_tileLength + seg_tileLength),
					   (is.key().get < 3 > () * 2.0));
				glEnd();
			}
			// glFlush();

			//              }
			//      }
		}
	}

	glDisable(GL_TEXTURE_2D);	/* disable texture mapping */
//      glDisable (GL_BLEND);

}

void SegGLview::mousePressEvent(QMouseEvent * event)
{
	dragx = startx = event->x();
	dragy = starty = event->y();

	panning = 1;
	zooming = 0;
}

void SegGLview::mouseReleaseEvent(QMouseEvent * event)
{
	int mousex = event->x();
	int mousey = event->y();

	//      if(mousex == startx && mousey == starty) {
	//              view_set_cursor(mousex, mousey);
	//              view_zoom(.75);
	//      }
	//      
	panning = 0;
	zooming = 0;

}

void SegGLview::mouseMoveEvent(QMouseEvent * event)
{
	// event->pos
	int mousex = event->x();
	int mousey = event->y();

	if (panning) {
		//              if((mousex == 0) || (mousex == win_w-1) ||
		//                 (ev.motion.y == 0) || (ev.motion.y == win_h-1)) {
		//                      dragx = win_w / 2;
		//                      dragy = win_h / 2;
		//                      SDL_WarpMouse(dragx, dragy);
		//              } else {
		view_pan(dragx - mousex, dragy - mousey, 0);
		dragx = mousex;
		dragy = mousey;
		//              }
	}
	if (zooming) {
		view_zoom(1.0 + (dragy - mousey) / 100.0);
		dragx = mousex;
		dragy = mousey;
	}

	view_set_cursor(event->x(), event->y());
}

//void SegGLview::mouseDoubleClickEvent(QMouseEvent *event) 
//{ 
//    
//} 

void SegGLview::keyPressEvent(QKeyEvent * event)
{

	switch (event->key()) {
	case Qt::Key_Minus:
		//level++;
		view_zoom(1);
		//                      updateGL();
		break;
	case Qt::Key_Equal:
		//level--;
		view_zoom(-1);
		//                      updateGL();
		break;
	case Qt::Key_Right:
		view_pan(+5, 0, 1);
		break;
	case Qt::Key_Left:
		view_pan(-5, 0, 1);
		break;
	case Qt::Key_Up:
		view_pan(0, -5, 1);
		break;
	case Qt::Key_Down:
		view_pan(0, +5, 1);
		break;
	case Qt::Key_W:
		// MOVE UP THE STACK, CLOSER TO VIEWER
		view_scrub(1);
		break;
	case Qt::Key_S:
		// MOVE DOWN THE STACK, FARTHER FROM VIEWER
		view_scrub(-1);
		break;
	default:
		QGLWidget::keyPressEvent(event);
	}
}

//void SegGLview::showEvent(QShowEvent *event)
//{
//      
//}

bool SegGLview::check_depth(int future_depth)
{
	return (!((future_depth > (channel_extent.getMax().z)) || (future_depth < 0)));
}

void SegGLview::view_zoom(int future_level)
{
#pragma mark <TODO: make this more sane>
	//      double dw, dh;
	//      
	//      dw = view->world_camera.w * zoom - view->world_camera.w;
	//      dh = view->world_camera.h * zoom - view->world_camera.h;
	//      
	//      view->world_camera.x -= dw * (view->cursor_x - view->world_camera.x) / view->world_camera.w;
	//      view->world_camera.y -= dh * (view->cursor_y - view->world_camera.y) / view->world_camera.h;
	//      view->world_camera.w += dw;
	//      view->world_camera.h += dh;
	//      
	//      update_viewport();
	//      
	//      updateGL();

	if ((level + future_level) > level) {
		// ZOOMING OUT

		view->shadow_camera.x = (view->shadow_camera.x * 2.0);
		view->shadow_camera.x = (view->shadow_camera.y * 2.0);
		view->shadow_camera.w = (view->shadow_camera.w * 2.0);
		view->shadow_camera.h = (view->shadow_camera.h * 2.0);

		view->world_camera.x = (view->world_camera.x * 2.0);
		view->world_camera.x = (view->world_camera.y * 2.0);
		view->world_camera.w = (view->world_camera.w * 2.0);
		view->world_camera.h = (view->world_camera.h * 2.0);

		//              viewport_bbox = AxisAlignedBoundingBox<int>(Vector3<int>(view->shadow_camera.x,view->shadow_camera.y,depth), Vector3<int>((view->shadow_camera.x + view->shadow_camera.w - 1), (view->shadow_camera.y + view->shadow_camera.h - 1), depth));
		//                      DataBbox scaled_viewport = DataBbox(viewport_bbox.getMin() * (level * 4.0), viewport_bbox.getMax() * (level * 4.0));

		level = level + future_level;

		if (DEBUG)
			cout << "view->camera: (" << view->shadow_camera.x << ", " << view->shadow_camera.
			    y << ")   " << "w: " << view->shadow_camera.w << "   h: " << view->shadow_camera.h << endl;

	} else if ((level + future_level) < level) {
		// ZOOMING IN

		view->shadow_camera.x = (view->shadow_camera.x / 2.0);
		view->shadow_camera.x = (view->shadow_camera.y / 2.0);
		view->shadow_camera.w = (view->shadow_camera.w / 2.0);
		view->shadow_camera.h = (view->shadow_camera.h / 2.0);

		view->world_camera.x = (view->world_camera.x / 2.0);
		view->world_camera.x = (view->world_camera.y / 2.0);
		view->world_camera.w = (view->world_camera.w / 2.0);
		view->world_camera.h = (view->world_camera.h / 2.0);

		//              viewport_bbox = AxisAlignedBoundingBox<int>(Vector3<int>(view->shadow_camera.x,view->shadow_camera.y,depth), Vector3<int>((view->shadow_camera.x + view->shadow_camera.w - 1), (view->shadow_camera.y + view->shadow_camera.h - 1), depth));
		//                      DataBbox scaled_viewport = DataBbox(viewport_bbox.getMin() * (level * 4.0), viewport_bbox.getMax() * (level * 4.0));

		level = level + future_level;

		if (DEBUG)
			cout << "view->camera: (" << view->shadow_camera.x << ", " << view->shadow_camera.
			    y << ")   " << "w: " << view->shadow_camera.w << "   h: " << view->shadow_camera.h << endl;

	}

	location_string = "zoom level: ";
	location_string.append(QString::number(level));
	location_string.append("\n");
	location_string.append("stack position: ");
	location_string.append(QString::number(depth));

	update_viewport();
//      updateGL();
	update();
}

void SegGLview::view_scrub(int future_depth)
{
	//      cout << "depth + " << future_depth << " in " << ortho << " VIEW" << endl;

	if (future_depth == 1) {
		near = near - 2;
		far = far - 2;

		if (check_depth(depth + 1))
			depth++;

		update_viewport();
//              updateGL();
		update();
	}

	else if (future_depth == -1) {
		near = near + 2;
		far = far + 2;

		if (check_depth(depth - 1))
			depth--;

		update_viewport();
		//updateGL();
		update();
	}
}

void SegGLview::view_pan(int dx, int dy, int screencoords)
{
	if (screencoords) {
		view->world_camera.x += dx;
		view->world_camera.y += dy;
	} else {
		view->world_camera.x += (double)dx / view->screen.w * view->world_camera.w;
		view->world_camera.y += (double)dy / view->screen.h * view->world_camera.h;
	}

	if (screencoords) {
		view->shadow_camera.x += dx;
		view->shadow_camera.y += dy;
	} else {
		view->shadow_camera.x += (double)dx / view->screen.w * view->shadow_camera.w;
		view->shadow_camera.y += (double)dy / view->screen.h * view->shadow_camera.h;
	}

	update_viewport();

	//updateGL();
	update();
}

void SegGLview::animate_cursors()
{

}

void SegGLview::view_set_cursor(int x, int y)
{
	struct rect rs, rw;

	rs.x = x;
	rs.y = y;

	//      view_screen_to_world(struct rect &rs, struct rect &rw);
	double f = view->world_camera.w / view->screen.w;

	rw.x = view->world_camera.x + rs.x * f;
	rw.y = view->world_camera.y + rs.y * f;
	rw.w = rs.w * f;
	rw.h = rs.h * f;
	//////////////

	view->cursor_x = rw.x;
	view->cursor_y = rw.y;
}

void SegGLview::update_xy_line(int dir)
{

	if (ortho == "xz") {
		int val = xy_pos_start.y;
		xy_pos_start = Vector2 < int >(-100, val + dir);
		xy_pos_end = Vector2 < int >(channel_extent.getMax().x, val + dir);
	}

	if (ortho == "yz") {
		int val = xy_pos_start.x;
		xy_pos_start = Vector2 < int >(val + dir, -100);
		xy_pos_end = Vector2 < int >(val + dir, channel_extent.getMax().y);
	}
	//updateGL();
	update();
//      animate_cursors();
}

void SegGLview::update_xz_line(int dir)
{

	if (ortho == "xy") {
		int val = xz_pos_start.y;

		xz_pos_start = Vector2 < int >(-100, val + dir);
		xz_pos_end = Vector2 < int >(channel_extent.getMax().x, val + dir);
	}
	if (ortho == "yz") {
		int val = xz_pos_start.y;

		xz_pos_start = Vector2 < int >(-100, val + dir);
		xz_pos_end = Vector2 < int >(channel_extent.getMax().x, val + dir);
	}
	//updateGL();
	update();
//      animate_cursors();

}

void SegGLview::update_yz_line(int dir)
{

	if (ortho == "xy") {
		int val = yz_pos_start.x;

		yz_pos_start = Vector2 < int >(val + dir, -100);
		yz_pos_end = Vector2 < int >(val + dir, channel_extent.getMax().y);
	}

	if (ortho == "xz") {
		int val = yz_pos_start.x;

		yz_pos_start = Vector2 < int >(val + dir, -100);
		yz_pos_end = Vector2 < int >(val + dir, channel_extent.getMax().y);
	}
	//updateGL();
	update();
//      animate_cursors();

}

void SegGLview::update_viewport()
{
	viewport_bbox =
	    AxisAlignedBoundingBox < int >(Vector3 < int >(view->shadow_camera.x, view->shadow_camera.y, depth),
					   Vector3 < int >((view->shadow_camera.x + view->shadow_camera.w - 1),
							   (view->shadow_camera.y + view->shadow_camera.h - 1), depth));

	location_string = "zoom level: ";
	location_string.append(QString::number(level));
	location_string.append("\n");
	location_string.append("stack position: ");
	location_string.append(QString::number(depth));
}

DataBbox SegGLview::merge_bounding_boxes(DataBbox desired_data)
{

	Vector3 < int >beginning;
	Vector3 < int >end;

	// STEP 2.  We know the beginning of the MipChunkCoords we're going to pull.
	// key point: make sure they're not negative.  if so, set them to 0

	int beg1, beg2;
	if ((viewport_bbox.getMin().x < desired_data.getMin().x))
		beg1 = desired_data.getMin().x;
	else
		beg1 = viewport_bbox.getMin().x;

	if ((viewport_bbox.getMin().y < desired_data.getMin().y))
		beg2 = desired_data.getMin().y;
	else
		beg2 = viewport_bbox.getMin().y;

	beginning = Vector3 < int >(beg1, beg2, desired_data.getMin().z);

	// STEP 3.  Figure out the end of the MipChunkCoords we're going to pull.
	int end1, end2;
	if (viewport_bbox.getMax().x > desired_data.getMax().x)
		end1 = desired_data.getMax().x;
	else
		end1 = viewport_bbox.getMax().x;
	if (viewport_bbox.getMax().y > desired_data.getMax().y)
		end2 = desired_data.getMax().y;
	else
		end2 = viewport_bbox.getMax().y;

	end = Vector3 < int >(end1, end2, desired_data.getMax().z);

	return DataBbox(beginning, end);
}

DataBbox SegGLview::merge_bounding_boxes(DataBbox desired_data, double desired_level)
{
	if (DEBUG)
		cout << "merging bounding boxes" << endl;

	DataBbox scaled_viewport;

	if (desired_level > level) {
		DataBbox(viewport_bbox.getMin() * (level * 2.0), viewport_bbox.getMax() * (level * 2.0));
	} else {
		DataBbox(viewport_bbox.getMin() / (level * 2.0), viewport_bbox.getMax() / (level * 2.0));
	}

	Vector3 < int >beginning;
	Vector3 < int >end;

	// STEP 2.  We know the beginning of the MipChunkCoords we're going to pull.
	// key point: make sure they're not negative.  if so, set them to 0

	int beg1, beg2;
	if ((scaled_viewport.getMin().x < desired_data.getMin().x))
		beg1 = desired_data.getMin().x;
	else
		beg1 = scaled_viewport.getMin().x;

	if ((scaled_viewport.getMin().y < desired_data.getMin().y))
		beg2 = desired_data.getMin().y;
	else
		beg2 = scaled_viewport.getMin().y;

	beginning = Vector3 < int >(beg1, beg2, desired_data.getMin().z);

	// STEP 3.  Figure out the end of the MipChunkCoords we're going to pull.
	int end1, end2;
	if (scaled_viewport.getMax().x > desired_data.getMax().x)
		end1 = desired_data.getMax().x;
	else
		end1 = scaled_viewport.getMax().x;
	if (scaled_viewport.getMax().y > desired_data.getMax().y)
		end2 = desired_data.getMax().y;
	else
		end2 = scaled_viewport.getMax().y;

	end = Vector3 < int >(end1, end2, desired_data.getMax().z);

	return DataBbox(beginning, end);
}

void SegGLview::setMyColorMap(char *imageData, char *channelData, Vector2 < int >dims, const OmMipChunkCoord & mipCoord,
			      int depth)
{
	DataBbox data_bbox = segmentation->MipCoordToDataBbox(mipCoord);
//      cout << "mip coord = " << mipCoord << endl;
//      cout << "data bbox = " << data_bbox << endl;
	int my_depth = depth;

	unsigned char *data = new unsigned char[dims.x * dims.y * 4];

//      cout << "LENGTH of data = " << dims.x*dims.y*4 << endl;
//      cout << "SIZE of data_bbox = " << data_bbox.getDimensions() << endl;
	////////// just a try: need data coords
	// so for each coord in the DataBbox
	int ctr = 0;

	int z_min = data_bbox.getMin().z;
	int z_max = data_bbox.getMax().z;
	int y_min = data_bbox.getMin().y;
	int y_max = data_bbox.getMax().y;
	int x_min = data_bbox.getMin().x;
	int x_max = data_bbox.getMax().x;
	if (ortho == "xy") {
		z_min = my_depth;
		z_max = my_depth;
	} else if (ortho == "xz") {
		y_min = my_depth;
		y_max = my_depth;
	} else {
		x_min = my_depth;
		x_max = my_depth;
	}

	for (int z = z_min; z <= z_max; z++) {
		for (int y = y_min; y <= y_max; y++) {
			for (int x = x_min; x <= x_max; x++) {

//                              cout << "volume_id = " << volume_id << endl;
				OmSegmentation & current_seg =
				    OmVolumeManager::GetVolume(volume_id).GetSegmentation(seg_id);
//                              cout << "before error" << endl;
//                              cout << "vector = " << Vector3<int>(x,y,z) << endl;
				uint32_t id = current_seg.GetVoxelSegmentId(DataCoord(x, y, z));

				QColor newcolor;
				if (id == 0) {
					if (chan_mSamplesPerVoxel == 1) {
						//      glTexImage2D (GL_TEXTURE_2D, 0, GL_LUMINANCE, dims.x, dims.y, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, imageData);
						data[ctr] = channelData[ctr];
						data[ctr + 1] = channelData[ctr];
						data[ctr + 2] = channelData[ctr];
						data[ctr + 3] = 1.0;

						ctr = ctr + 4;
					} else if (chan_mSamplesPerVoxel == 3) {
						// glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, dims.x, dims.y, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
						data[ctr] = channelData[ctr];
						data[ctr + 1] = channelData[ctr + 1];
						data[ctr + 2] = channelData[ctr + 2];
						data[ctr + 3] = 1.0;

						ctr = ctr + 4;
					}
				} else {
					const Vector3 < float >&color =
					    OmVolumeManager::GetVolume(volume_id).GetSegmentation(seg_id).
					    GetSegment(id).GetColor();
					newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 50);
					//                                      cout << "newcolor:              R = " << newcolor.red() << " G = " << newcolor.green() << " B = " << newcolor.blue() << endl;
					//                                      cout << "done, ctr = " << ctr << endl;

					data[ctr] = newcolor.red();
					data[ctr + 1] = newcolor.green();
					data[ctr + 2] = newcolor.blue();
					data[ctr + 3] = newcolor.alphaF();

					ctr = ctr + 4;
				}
			}
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dims.x, dims.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

}
