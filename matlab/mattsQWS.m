function mattsQWS
	conn  = hdf5read('/home/sturaga/exhibit/608/e2006_im_histeq_epoch1769.h5', '/main');
	LoThreshold=0.1, HiThreshold=0.98, SizeThreshold=200;
	[seg graph graphValues dend dendValues] = QuickieWS(conn, LoThreshold, HiThreshold, SizeThreshold);
	clear conn;
	chan = hdf5read('/home/sturaga/net_sets/E2006/point_cloud/im.h5', '/main');
	hdf5write('/home/mwimer/mattsQWS.h5', '/main', seg, '/dend', dend, '/graph', graph, '/dendValues', dendValues, '/chanSingle', chan);
end
