% Author: Matt Wimer, Purcaro
% converts hdf5 channel data to a tiff stack

function hdf5channelToTiff(h5inputfile)

paths.root    = '/home/omni/data/e1088/';
paths.project = fullfile( paths.root,    'convert' );
makeDir( paths.project );
paths.tiffs   = fullfile( paths.project, 'tiffs' );
makeDir( paths.tiffs );

path.channelPath = fullfile( paths.tiffs, 'channel' );
makeDir( path.channelPath );

[ndims, input_size, max_size]=get_hdf5_size(h5inputfile, 'main');
input_dataID=H5F.open(h5inputfile,'H5F_ACC_RDONLY','H5P_DEFAULT');
input_datasetID=H5D.open(input_dataID, 'main');
input_dataspaceID=H5D.get_space(input_datasetID);

numSlices = input_size(3);

for sliceNum = 1:numSlices
    % get this block of the component file
    block_begin_coords=[1 1 sliceNum];
    block_end_coords=[input_size(1) input_size(2) sliceNum];
    data = get_hdf5(input_datasetID, input_dataspaceID, [block_begin_coords 1], [block_end_coords 3]);

%     data_min = min( data(:));
%     data_max = max( data(:));
%     if( data_min < 0 || data_max > 1 )
%         fprintf('min: %f, max: %f\n', data_min, data_max);
%     end
    
     scaled_block = data;
     fname = fullfile( path.channelPath, sprintf('channel--%03d.tif', sliceNum));
     imwrite( scaled_block, fname, 'TIFF', 'Compression', 'none');
    
     fprintf('%d of %d\n', sliceNum, numSlices );
end

H5D.close(input_datasetID);
H5F.close(input_dataID);

fprintf('done!\n');
