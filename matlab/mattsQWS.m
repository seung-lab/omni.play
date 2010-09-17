% The test plan tests the watershed generation parameters
% call then run:
% ../omni/bin/omni --headless=mattsBuild.cmd

function mattsQWS
	fid = fopen('mattsBuild.cmd', 'w');

	conn = hdf5read('/home/mwimer/e2006_im_histeq_epoch1769.h5', '/main');
	%conn = conn(15:550+15,15:550+15,15:550+15,:);

	chan = hdf5read('/home/mwimer/im.h5', '/main');
	%chan = chan(15:550+15,15:550+15,15:550+15,:);

	for inc = 00:100:000
		% Careful, these need to be on the outside most loop.
		%chan = chan(1:550-inc,1:550-inc,1:550-inc,:);
		%conn = conn(1:550-inc,1:550-inc,1:550-inc,:);
	
		%filter = conn;
		%for q = 1:1:10
			%temp = resample(conn, 1, q);
			%filter = filter +resample(temp, q, 1);
		%end
		%filter = filter / 10;
		%conn = filter;
	
		for SizeThreshold = [250 30]
			for HiThreshold = [.99 .98 1] 
				for absLowThreshold = [.3]
					LoThreshold=0.1;
					[seg graph graphValues dend dendValues] = QuickieWS(conn, LoThreshold, HiThreshold, SizeThreshold, absLowThreshold);

					f = sprintf ('e2006-big-%d-%f-%f', SizeThreshold, HiThreshold, absLowThreshold);
					fname = sprintf ('/home/mwimer/%s.h5', f);
					fprintf (fid, 'create:/home/mwimer/%s.omni\n', f);
					fprintf (fid, 'loadHDF5seg:/home/mwimer/%s.h5\n', f);
					fprintf (fid, 'mesh\n');
					fprintf (fid, 'loadHDF5chann:/home/mwimer/%s.h5\n', f);
					fprintf (fid, 'close\n');

					hdf5write(fname, 
						  '/main', seg, 
						  '/dend', dend, 
						  '/graph', graph, 
						  '/dendValues', dendValues, 
						  '/chanSingle', chan, 
						  '/affGraphSingle', conn);
				end
			end
		end
	end
end



