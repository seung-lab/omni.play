function loadImages
    loadZebraFish();
%     loadMouse();
end

function loadMouse()
    doLoadImages(1024, 1024, 1, 1000, '03 glomerulus1_altMag', 'mouseGlomerulus.h5','uint16');
end

function loadZebraFish
    doLoadImages(4096, 4096, 0, 473, 'fishOB', 'zebraFishImgStack.h5', 'uint8');
end

function doLoadImages(xSize, ySize, zStart, zEnd, baseTIFFname, hdf5fileName, dataType )
    zSize = zEnd;
    if(0 == zStart)
       zSize = zSize + 1; 
    end
        
    imgStack = zeros(xSize,ySize,zSize,dataType);
    for ind = zStart:zEnd
        fname=sprintf('%s.%04d.tif', baseTIFFname, ind );
        fprintf('loading %s\n', fname);
        zOffset = ind;
        if(0 == zStart)
            zOffset = ind + 1; 
        end
        imgStack(:,:,zOffset) = imread(fname);
    end
    fprintf('writing h5 file...');
    hdf5write(hdf5fileName, '/chan', imgStack );
    fprintf('done\n');
end
