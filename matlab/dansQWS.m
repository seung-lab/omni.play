% The test plan tests the watershed generation parameters
% call then run:
% ../omni/bin/omni --headless=dansBuild.cmd

function dansQWS
	outdir = '/home/scratch';
	fid = fopen('dansBuild.cmd', 'w');

	conn = hdf5read('/Users/mwimer/danielsData.h5', '/main');
	conn = conn(15:200+15,15:200+15,15:200+15,:);

	chan = hdf5read('/Users/mwimer/danielsChan.h5', '/main');
	chan = chan(15:200+15,15:200+15,15:200+15,:);

	for inc = 000:100:100
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
	
		for SizeThreshold = [70000 30000 6000]
			for HiThreshold = [.90 .86 .88 .95] 
				for absLowThreshold = [.5 .35 .4]
					LoThreshold=0.1;
					[seg graph graphValues dend dendValues] = QuickieWS(conn, LoThreshold, HiThreshold, SizeThreshold, absLowThreshold);

					f = sprintf ('danstest2-%d-%d-%f-%d', 200-inc, SizeThreshold, HiThreshold, absLowThreshold);
					fname = sprintf ('%s/%s.h5', outdir, f);
					fprintf (fid, 'create:%s/%s.omni\n', outdir, f);
					fprintf (fid, 'loadHDF5seg:%s/%s.h5\n', outdir, f);
					fprintf (fid, 'mesh\n');
					fprintf (fid, 'loadHDF5chann:%s/%s.h5\n', outdir, f);
					fprintf (fid, 'close\n');

					hdf5write(fname, '/main', seg, '/dend', dend, '/graph', graph, '/dendValues', dendValues, '/chanSingle', chan, '/affGraphSingle', conn);
				end
			end
		end
	end
end



