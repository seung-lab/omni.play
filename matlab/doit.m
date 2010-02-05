function doit( conn )

    paths.root    = '/home/omni/data/auto_segmentation/';
    paths.project = fullfile( paths.root,    'full' );
    makeDir( paths.project );
    paths.tiffs   = fullfile( paths.project, 'tiffs' );
    makeDir( paths.tiffs );

    threshold = 0.97;
    fprintf('Running connected components at threshold %g ...', threshold );
    tic;
    [ seg, cmpSz ] = connectedComponents( conn>threshold ); 
    fprintf(' done. ');
    toc

    fprintf('Running marker-based watershed %g ...', threshold);
    tic;
    seg_imgs = markerWatershed( conn, mknhood2(1), seg, seg==0, 0.5 );
    fprintf(' done. ');
    toc
    
    dust_seg_threshold = 400;
    max_seg_number = find(cmpSz>dust_seg_threshold, 1, 'last' );
    %set dust to 0
    fprintf('Dusting at threshold %g ...', dust_seg_threshold );
    tic;
    seg_imgs( seg_imgs > max_seg_number ) = 0;
    fprintf(' done. ');
    toc
        
    save(sprintf('run_%d_%d.mat', seg_threshold*100, dust_seg_threshold));
    
    writeThresSegTiffs( paths, seg_imgs, dust_seg_threshold, threshold ); 
    
    fprintf('done\n');
end

function writeThresSegTiffs( paths, seg_imgs, dust_seg_threshold, seg_threshold )
    nslice = size (seg_imgs, 3);
    for islice=1:nslice 
        fname = sprintf('segmentation_%d--nodust_%d--%03d.tif', seg_threshold*100, dust_seg_threshold, islice);
        pathName = fullfile( paths.tiffs, sprintf('segmentation_%d--nodust_%d', seg_threshold*100, dust_seg_threshold));
        makeDir( pathName );
        fnameAndPath = fullfile( pathName, fname );
        imwrite( uint16( seg_imgs(:,:,islice) ), fnameAndPath, 'TIFF','Compression', 'none');
    end
end

