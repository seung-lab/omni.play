% The test plan tests the watershed generation parameters
% call then run:
% ../omni/bin/omni --headless=dansBuild.cmd

function dansQWS
	fid = fopen('dansBuild.cmd', 'w');

	conn = hdf5read('/home/mwimer/danielsData.h5', '/main');
	conn = conn(15:200+15,15:200+15,15:200+15,:);

	chan = hdf5read('/home/mwimer/danielsChan.h5', '/main');
	chan = chan(15:200+15,15:200+15,15:200+15,:);

	for inc = 200:100:200
		% Careful, these need to be on the outside most loop.
		chan = chan(1:200-inc,1:200-inc,1:200-inc,:);
		conn = conn(1:200-inc,1:200-inc,1:200-inc,:);
	
		%filter = conn;
		%for q = 1:1:10
			%temp = resample(conn, 1, q);
			%filter = filter +resample(temp, q, 1);
		%end
		%filter = filter / 10;
		%conn = filter;
	
		for SizeThreshold = [6000 30000]
			for HiThreshold = [.80 .81 .85] 
				for absLowThreshold = [.3 .35]
					LoThreshold=0.1;
					[seg graph graphValues dend dendValues] = QuickieWS(conn, LoThreshold, HiThreshold, SizeThreshold, absLowThreshold);

					f = sprintf ('dans-%d-%d-%f-%d', 200-inc, SizeThreshold, HiThreshold, absLowThreshold);
					fname = sprintf ('/home/mwimer/%s.h5', f);
					fprintf (fid, 'create:/home/mwimer/%s.omni\n', f);
					fprintf (fid, 'loadHDF5seg:/home/mwimer/%s.h5\n', f);
					fprintf (fid, 'mesh\n');
					fprintf (fid, 'loadHDF5chann:/home/mwimer/%s.h5\n', f);
					fprintf (fid, 'close\n');

					hdf5write(fname, '/main', seg, '/dend', dend, '/graph', graph, '/dendValues', dendValues, '/chanSingle', chan, '/affGraphSingle', conn);
				end
			end
		end
	end
end



