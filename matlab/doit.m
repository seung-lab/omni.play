function doit( conn)
    dbstop if error
    
    paths.root    = '/home/omni/data/e2006/new_neural_net/';
    paths.project = fullfile( paths.root,    'full' );
    makeDir( paths.project );
    paths.tiffs   = fullfile( paths.project, 'tiffs' );
    makeDir( paths.tiffs );

    threshold = 0.97;
    fprintf('Running connected components at threshold %g ...', threshold );
    tic;
    [ seg, cmpSz ] = connectedComponents( conn>threshold, '', 10 ); 
    fprintf(' done. ');
    toc

    fprintf('Running marker-based watershed %g ...', threshold);
    tic;
    seg_imgs = markerWatershed( conn, mknhood2(1), seg, seg==0, 0.5 );
    fprintf(' done. ');
    toc

    save(sprintf('full_run_%d.mat', threshold*100));

    hdf5write(['e2006_' num2str(threshold*100) '.h5'],'/main',seg_imgs);

    fprintf('done\n');
end

