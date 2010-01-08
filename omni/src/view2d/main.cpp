
//#include <QApplication> 
//#include <QWidget> 
//
//#include "common/omGl.h"
//
//#include "omTile.h"
//#include "omCachingTile.h"
//#include "omTextureID.h"
//
//#include "common/omStd.h"
//#include "volume/omVolumeTypes.h"
//#include "system/omProject.h"
//
//
//#include <boost/shared_ptr.hpp>
//using boost::shared_ptr;

#include <QApplication>

#include "mainwindow.h"

#include "common/omGl.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	// Q_INIT_RESOURCE(dockwidgets);
	MainWindow mainWin;
	mainWin.show();
	return app.exec();
}


/*
void CachingTest();
void DataBboxTest();
void SpaceBboxTest();

int main(int argc, char *argv[])
{
	
	
	QApplication app(argc, argv);
	
	OmProject::Load("/Users/rachel/Code/Repos/trunk_02_06_09/bin/","project.omni");
	
	
	CachingTest();
	//DataBboxTest();
	SpaceBboxTest();
	
	// OmView2d *widget = new OmView2d(XY_VIEW, CHANNEL, 1, 1);
	QWidget *widget = new QWidget();
	
	cout << "View Box = " << OmStateManager::GetViewBbox() << endl;
	cout << "View Center = " << OmStateManager::GetViewCenter() << endl;
	
	if(OmStateManager::GetViewBbox().isEmpty())
		cout << "view box is empty" << endl;
	else
		cout << "view box is not empty" << endl;
	
	widget->show(); 
	
	
	
	return app.exec();
	
}


void CachingTest() {
	
	OmChannel &mChannel = OmVolume::GetChannel(1);
	
	
	
	OmTile *omniTile = new OmTile(XY_VIEW, CHANNEL, 1, &mChannel);
	
	DataBbox mDataBbox = mChannel.GetSourceDataExtent();
	SpaceBbox mSpaceBbox = OmVolume::NormToSpaceBbox(mChannel.DataToNormBbox(mDataBbox));
	
	cout << "data bbox = " << mDataBbox << endl;
	cout << "space bbox = " << mSpaceBbox << endl;
	
	
	OmTileCoordinate mTileCoord = OmTileCoordinate(2, SpaceCoord(.75,.75,.75));	
	
	OmMipChunkCoord mMipCoord = omniTile->TileToMipCoord(mTileCoord);
	cout << "mip coord = " << mMipCoord << endl;
	
	int depth = omniTile->GetDepth(mTileCoord);
	cout << "depth = " << depth << endl;
	
	
	
	OmCachingTile *cache = new OmCachingTile(XZ_VIEW, CHANNEL, 1, &mChannel);
	
	shared_ptr<OmTextureID> gotten_id = cache->GetTextureID(mTileCoord);
	
	cout << "GLUint texture ID = " << gotten_id->GetTextureID() << endl;
	cout << "size = " << gotten_id->GetSize() << endl;
}

void DataBboxTest() {
	
	OmChannel &mChannel = OmVolume::GetChannel(1);
	int leafdim = mChannel.GetChunkDimension();
	
	DataBbox data_bbox = DataBbox(Vector3<int>(0,0,0), Vector3<int>(150,150,0));
	
	
	
	int ctr = 0;
	int newctr = 0;
	
	int y_min = data_bbox.getMin().y;
	int y_max = data_bbox.getMax().y;
	int x_min = data_bbox.getMin().x;
	int x_max = data_bbox.getMax().x;
	
	
	for (int y = y_min ; y <= y_max ; y = y + leafdim) {
		for (int x = x_min; x <= x_max ; x = x + leafdim) {
			
			DataCoord data_coord = Vector3<int>(x, y, 0);
			cout << "data coord = " << data_coord << endl;
			cout << "spatial coord = " << OmVolume::NormToSpaceCoord(mChannel.DataToNormCoord(data_coord)) << endl;
			cout << "mip coord = " << mChannel.DataToMipCoord(0, data_coord) << endl;
			
		}
	}
}

void SpaceBboxTest() {
	
	OmChannel &mChannel = OmVolume::GetChannel(1);
	int leafdim = mChannel.GetChunkDimension();
	
	// try mip coord to norm bbox to space bbox
	OmMipChunkCoord test1mipcoord= OmMipChunkCoord(1, Vector3<int>(0,0,0));
	SpaceBbox test1spacebbox = OmVolume::NormToSpaceBbox(mChannel.MipCoordToNormBbox(test1mipcoord));
	
	cout << "level 1 mip chunk coord = " << test1mipcoord << endl;
	cout << "corresponding space bbox = " << test1spacebbox << endl;
	
	OmMipChunkCoord test2mipcoord= OmMipChunkCoord(0, Vector3<int>(0,0,0));
	SpaceBbox test2spacebbox = OmVolume::NormToSpaceBbox(mChannel.MipCoordToNormBbox(test2mipcoord));
	
	cout << "level 0 mip chunk coord = " << test2mipcoord << endl;
	cout << "corresponding space bbox = " << test2spacebbox << endl;
}
*/
