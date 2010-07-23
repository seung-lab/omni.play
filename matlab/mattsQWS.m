
function mattsQWS
	fid = fopen('mattsBuild.cmd', 'w');

	conn = hdf5read('/home/mwimer/e2006_im_histeq_epoch1769.h5', '/main');
	conn = conn(15:550+15,15:550+15,15:550+15,:);

	chan = hdf5read('/home/mwimer/im.h5', '/main');
	chan = chan(15:550+15,15:550+15,15:550+15,:);

	for inc = 400:100:400
		% Careful, these need to be on the outside most loop.
		chan = chan(1:550-inc,1:550-inc,1:550-inc,:);
		conn = conn(1:550-inc,1:550-inc,1:550-inc,:);

		for absmin = [0.9 .6 .3]
			for hithres = [.995 .982 1.0] 
				for threshold = [30 3000 300 60 15]
					LoThreshold=0.1, HiThreshold=hithres, SizeThreshold=threshold;
					[seg graph graphValues dend dendValues] = QuickieWS(conn, LoThreshold, HiThreshold, SizeThreshold, absmin);

					f = sprintf ('test-%d-%d-%f-%f', 550-inc, threshold, hithres, absmin);
					fname = sprintf ('/home/mwimer/%s.h5', f);
					fprintf (fid, 'create:/home/mwimer/%s.omni\n', f);
					fprintf (fid, 'loadHDF5seg:/home/mwimer/%s.h5\n', f);
					fprintf (fid, 'mesh\n');
					fprintf (fid, 'loadHDF5chan:/home/mwimer/%s.h5\n', f);
					fprintf (fid, 'close\n');

					hdf5write(fname, '/main', seg, '/dend', dend, '/graph', graph, '/dendValues', dendValues, '/chanSingle', chan, '/affGraphSingle', conn);
				end
			end
		end
	end
end

%format
%create:/Users/purcaro/Omni/purcaro_testing/regionGraphs/i1088_150_graph.omni
%loadHDF5seg:/Users/purcaro/Omni/data/regionGraphs/i1088_150_graph.h5
%mesh

