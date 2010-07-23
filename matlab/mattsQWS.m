
function mattsQWS
	fid = fopen('mattsBuild.cmd', 'w');

	conn = hdf5read('/home/sturaga/exhibit/608/e2006_im_histeq_epoch1769.h5', '/main');
	conn = conn(15:550+15,15:550+15,15:550+15,:);

	chan = hdf5read('/home/sturaga/net_sets/E2006/point_cloud/im.h5', '/main');
	chan = chan(15:550+15,15:550+15,15:550+15,:);

	for inc = 100:100:400
		% Careful, these need to be on the outside most loop.
		chan = chan(1:550-inc,1:550-inc,1:550-inc,:);
		conn = conn(1:550-inc,1:550-inc,1:550-inc,:);

		for absmin = [0.9 .6 .3]
			for hithres = [.995 .980 1.0 .990] 
				for threshold = [3000 300 30]
					LoThreshold=0.1, HiThreshold=hithres, SizeThreshold=threshold;
					[seg graph graphValues dend dendValues] = QuickieWS(conn, LoThreshold, HiThreshold, SizeThreshold, absmin);

					f = sprintf ('meshinator/new-%d-%d-%f-%f', 550-inc, threshold, hithres, absmin);
					fname = sprintf ('/home/mwimer/%s.h5', f);
					fprintf (fid, 'create:/home/mwimer/%s.omni\n', f);
					fprintf (fid, 'loadHDF5seg:/home/mwimer/%s.h5\n', f);
					fprintf (fid, 'mesh\n');
					fprintf (fid, 'loadHDF5chan:/home/mwimer/%s.h5\n', f);
					fprintf (fid, 'close\n');

					hdf5write(fname, '/main', seg, '/dend', dend, '/graph', graph, '/dendValues', dendValues, '/chanSingle', chan);
				end
			end
		end
	end
end

%format
%create:/Users/purcaro/Omni/purcaro_testing/regionGraphs/i1088_150_graph.omni
%loadHDF5seg:/Users/purcaro/Omni/data/regionGraphs/i1088_150_graph.h5
%mesh

