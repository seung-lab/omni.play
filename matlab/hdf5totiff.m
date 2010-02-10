% Author: Matt Wimer
% Misnamed and coded poorle, this code converts affgraphs to
% tiffs
%sum an hdf5 graph into an in-out graph
function hdf5totiff(h5inputfile, p)

[ndims, input_size, max_size]=get_hdf5_size(h5inputfile, 'main');
input_dataID=H5F.open(h5inputfile,'H5F_ACC_RDONLY','H5P_DEFAULT');
input_datasetID=H5D.open(input_dataID, 'main');
input_dataspaceID=H5D.get_space(input_datasetID);

for blk = 1:1:input_size(3)
    % get this block of the component file
    block_begin_coords=[1 1 blk];
    block_end_coords=[input_size(1) input_size(2) blk];
    g_block=get_hdf5(input_datasetID, input_dataspaceID, [block_begin_coords 1], [block_end_coords 3]);
    
%     inout_block = mean (g_block, 4);
     
    imwrite(g_block(:,:,:,1),sprintf('%s--1--%03d.tif',p,blk),'TIFF','Compression','none');
    imwrite(g_block(:,:,:,2),sprintf('%s--2--%03d.tif',p,blk),'TIFF','Compression','none');
    imwrite(g_block(:,:,:,3),sprintf('%s--3--%03d.tif',p,blk),'TIFF','Compression','none');
    
    if mod(blk,50)
       fprintf('\n'); 
    end
    fprintf(1, '.');
end

H5D.close(input_datasetID);
H5F.close(input_dataID);

fprintf(1, '\n');
