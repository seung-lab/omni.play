% Author: Matt Wimer
% Misnamed and coded poorle, this code converts affgraphs to
% tiffs
%sum an hdf5 graph into an in-out graph
function hdf5totiff(h5inputfile, p)

blksz = [100 100 100];


% open input file
[ndims, input_size, max_size]=get_hdf5_size(h5inputfile, 'main');
input_dataID=H5F.open(h5inputfile,'H5F_ACC_RDONLY','H5P_DEFAULT');
input_datasetID=H5D.open(input_dataID, 'main');
input_dataspaceID=H5D.get_space(input_datasetID);

begin_coords=ones([1, ndims]);
end_coords=input_size';

blksz = [input_size(1) input_size(2) 1];

%% connect components of the blocks
%for iblk = begin_coords(1):blksz(1):end_coords(1),
%        for jblk = begin_coords(2):blksz(2):end_coords(2),
%                for kblk = begin_coords(3):blksz(3):end_coords(3),

for blk = 1:1:input_size(3)
                        % get this block of the component file
                        block_begin_coords=[1 1 blk];
                        block_end_coords=[input_size(1) input_size(2) blk];
                        g_block=get_hdf5(input_datasetID, input_dataspaceID, [block_begin_coords 1], [block_end_coords 3]);

                        %inout_block=single(sum(g_block,4)./input_size(4));
                        inout_block = mean (g_block, 4);
                       
                        % write out new components
                        %write_hdf5(output_datasetID, output_dataspaceID, block_begin_coords, block_end_coords, inout_block);                        
                        
                        imwrite(inout_block,sprintf('%s%03d.tif',p,blk),'TIFF','Compression','none');

                        
                        fprintf(1, '.');
%                end
%        end
end

H5D.close(input_datasetID);
H5F.close(input_dataID);

fprintf(1, '\n');
