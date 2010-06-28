function loadImages()

zebraImgStack = zeros(4096,4096);

for ind = 0:479
    fname=sprintf('fishOB.%04d.tif', ind );
    fprintf('loading %s\n', fname);
    zebraImgStack(:,:,1+ind) = imread(fname);
end

end