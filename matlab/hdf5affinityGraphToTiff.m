% Author: Matt Wimer, Purcaro
% converts affinity graphs to three tiff stacks

function hdf5affinityGraphToTiff(h5inputfile)

paths.root    = '/home/omni/data/e1088/';
paths.project = fullfile( paths.root,    'convert' );
makeDir( paths.project );
paths.tiffs   = fullfile( paths.project, 'tiffs' );
makeDir( paths.tiffs );

path.affinityGraph1 = fullfile( paths.tiffs, 'affinityGraph1' );
makeDir( path.affinityGraph1 );
path.affinityGraph2 = fullfile( paths.tiffs, 'affinityGraph2' );
makeDir( path.affinityGraph2 );
path.affinityGraph3 = fullfile( paths.tiffs, 'affinityGraph3' );
makeDir( path.affinityGraph3 );

[ndims, input_size, max_size]=get_hdf5_size(h5inputfile, 'main');
input_dataID=H5F.open(h5inputfile,'H5F_ACC_RDONLY','H5P_DEFAULT');
input_datasetID=H5D.open(input_dataID, 'main');
input_dataspaceID=H5D.get_space(input_datasetID);

for sliceNum = 1:1:input_size(3)
    % get this block of the component file
    block_begin_coords=[1 1 sliceNum];
    block_end_coords=[input_size(1) input_size(2) sliceNum];
    g_block=get_hdf5(input_datasetID, input_dataspaceID, [block_begin_coords 1], [block_end_coords 3]);
    
    scaled_block = uint8(255*g_block);
    imwrite( scaled_block(:,:,:,1), fullfile( path.affinityGraph1, sprintf('ag--1--%03d.tif', sliceNum)), 'TIFF', 'Compression', 'none');
    imwrite( scaled_block(:,:,:,2), fullfile( path.affinityGraph2, sprintf('ag--2--%03d.tif', sliceNum)), 'TIFF', 'Compression', 'none');
    imwrite( scaled_block(:,:,:,3), fullfile( path.affinityGraph3, sprintf('ag--3--%03d.tif', sliceNum)), 'TIFF', 'Compression', 'none');
    
    fprintf('%d of %d\n', sliceNum, input_size(3) );
end

H5D.close(input_datasetID);
H5F.close(input_dataID);

fprintf('done!\n');