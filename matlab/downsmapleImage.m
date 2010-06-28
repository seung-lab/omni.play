function downsmapleImage(data)

    xSize = size(data,1);
    ySize = size(data,2);
    zSize = size(data,3);

    fprintf('allocating memory...');
    newData = zeros(xSize/2, ySize/2, zSize, 'uint8');
    fprintf('done\n');
    
    for ind = 1:zSize
        fprintf('downsampling slice %d...', ind);
        newData(:,:,ind) = uint8((uint16(data(1:2:end,1:2:end,ind)) + ...
                                  uint16(data(2:2:end,1:2:end,ind)) + ...
                                  uint16(data(1:2:end,2:2:end,ind)) + ...
                                  uint16(data(2:2:end,2:2:end,ind)))/4);
        fprintf('done\n');
    end

    fprintf('writing h5 file...');
    hdf5write('zebraFishImgStack.downsmapled2.h5', '/chanSingle', newData);
    fprintf('done\n');
end