function segmentForWimer( dust_seg_threshold )

    paths.root    = '/home/omni/data/auto_segmentation/full';
    
        paths.project = fullfile( paths.root,    'full' );
%    [ paths, conn, im ] = loadMegavoxel( paths );
%   [ paths, conn, im ] = load10Megavoxel( paths );
    
    paths.tiffs   = fullfile( paths.project, 'tiffs' );
    makeDir( paths.tiffs );
    paths.diary   = fullfile( paths.project, 'log.txt' );
    diary( paths.diary );
    diary on;

%     print_img_info( conn );
    
%     thresholds = [.3:.05:.95 .97 .98 .99];
    thresholds = [ .95 ];
%     seg_imgs = run_auto_segmentation( conn, thresholds );
    
%     if ~exist('dust_seg_threshold', 'var') || isempty( dust_seg_threshold )
         dust_seg_threshold = 400;
%     end
%     thres_seg_img = threshold_segment( seg_imgs{ 95 }, dust_seg_threshold );

    fprintf('writing images...');
    writeRawTiffs( paths, im );
    writeAvgConnGraphTiffs( paths, conn );
%     writeSegTiffs( paths, seg_imgs );
    writeThresSegTiffs( paths, seg_img, dust_seg_threshold );
    
    fprintf('done\n');
    diary off;
end

function print_img_info( img )
    x = size(img,1);
    y = size(img,2);
    z = size(img,3);
    d = size(img,4);
    num_voxels = x*y*z;
    num_mega_voxels = num_voxels / 1000000;
    fprintf('incoming image size: %g x %g x %g x %g (%g voxels == %g megavoxels)\n', x,y,z,d,num_voxels, num_mega_voxels);
end

function seg_imgs = run_auto_segmentation( conn, thresholds, seg_threshold )

    seg_imgs = {};
    
    for k = 1:length(thresholds),
        threshold = thresholds(k);
        
        tic;
        fprintf('Running connected components at threshold %g ...', threshold );
        if ~exist('seg_threshold', 'var') || empty( seg_threshold )
            [ seg, cmpSz ] = connectedComponents( conn>threshold );    
        else
            [ seg, cmpSz ] = connectedComponents( conn>threshold, '', seg_threshold );
        end
        fprintf(' done. ');
        toc

        tic;
        fprintf('Running marker-based watershed %g ...', threshold);
        si
        fprintf(' done. ');
        toc
    end

end

function thres_seg_img = threshold_segment( seg_img, seg_threshold )

    printLine();
    fprintf('dusting threshold of %g\n', seg_threshold );
    segment_numbers = unique( seg_img );
    num_segments    = length( segment_numbers ) ;
    fprintf('found %d segments\n', num_segments );
    fprintf('\tdusting...');
    
    tic;
    thres_seg_img = do_threshold_segment( seg_img, seg_threshold, segment_numbers, num_segments );
    
    fprintf('done\nfound %d segments after dusting\n', length( unique( thres_seg_img ) ) );
    toc
    printLine();
end

function thres_seg_img = do_threshold_segment( seg_img, seg_threshold, segment_numbers, num_segments )

    thres_seg_img = seg_img;
    
    max_seg_num = max( segment_numbers );

    for i = 1 : max_seg_num
       pixels_indices_in_seg = find( seg_img == i );
       num_pixels_in_seg = length( pixels_indices_in_seg );
       if num_pixels_in_seg < seg_threshold
           thres_seg_img( pixels_indices_in_seg ) = 0;
       end
    end
end

function thres_seg_img = do_threshold_segment_faster( seg_img, seg_threshold, segment_numbers, num_segments )

    thres_seg_img = seg_img;
    
    max_seg_num = max( segment_numbers );

    segment_indicies  = zeros( max_seg_num, 4000, 'uint32' );
    segment_index_len = zeros( max_seg_num, 1,    'uint32' );
    
    for i = 1 : length( seg_img(:) )
       seg_num = seg_img( i );
       if 0 == seg_num
          continue; 
       end
       segment_index_len( seg_num ) =  segment_index_len( seg_num ) + 1;
       segment_indicies( seg_num, segment_index_len( seg_num )  ) = i;
    end
    
    for seg_num = 1 : max_seg_num
       if 0 == segment_index_len( seg_num )
           continue;
       end
       if  segment_index_len( seg_num ) < seg_threshold
           thres_seg_img( segment_indicies( seg_num ) ) = 0;
       end
    end
    
end

function writeAvgConnGraphTiffs( paths, conn_img )
    nslice = size(conn_img, 3);
    for islice=1:nslice
        mconn = mean (conn_img, 4);
        fname = sprintf('ag%03d.tif', islice);
        fnameAndPath = fullfile( paths.tiffs, fname );
        imwrite(mconn(:,:,islice), fnameAndPath, 'TIFF', 'Compression', 'none' );
    end
end

function writeRawTiffs( paths, raw_img )
    nslice = size (raw_img, 3);
    for islice=1:nslice
        fname = sprintf('raw%03d.tif', islice);
        fnameAndPath = fullfile( paths.tiffs, fname );
        imwrite(raw_img(:,:,islice), fnameAndPath, 'TIFF', 'Compression', 'none');
    end
end

function writeSegTiffs( paths, seg_img )
    nslice = size (seg_img, 3);
    for islice=1:nslice 
        slice = uint16( seg_img(:,:,islice) );
        fname = sprintf('segmentation%03d.tif', islice);
        fnameAndPath = fullfile( paths.tiffs, fname );
        imwrite( slice, fnameAndPath, 'TIFF','Compression', 'none');
    end
end

function writeThresSegTiffs( paths, seg_img, seg_threshold )
    nslice = size (seg_img, 3);
    for islice=1:nslice 
        slice = uint16( seg_img(:,:,islice) );
        fname = sprintf('segmentation_nodust_%d--%03d.tif', seg_threshold, islice);
        fnameAndPath = fullfile( paths.tiffs, fname );
        imwrite( slice, fnameAndPath, 'TIFF','Compression', 'none');
    end
end

function [ paths, conn, im ] = loadMegavoxel( paths )
    paths.project = fullfile( paths.root,    'megavoxel' );
    small_fname   = fullfile( paths.project, 'megavoxel.mat' );
   
    data_dim = 13:112;
    [ conn, im ] = doLoadData( paths, small_fname, data_dim );
end

function [ paths, conn, im ] = load10Megavoxel( paths )
    paths.project = fullfile( paths.root,    '10megavoxel' );
    small_fname   = fullfile( paths.project, '10megavoxel.mat' );
   
    data_dim = 13:336;
    [ conn, im ] = doLoadData( paths, small_fname, data_dim );
end

function [ conn, im ] = doLoadData( paths, small_fname, data_dim )

    makeDir( paths.project );

    if exist( small_fname, 'file')
        fprintf('using data cache...')
        load( small_fname, 'conn', 'im' );
        fprintf('done\n');
        return;
    else
        fprintf('loading raw data--will take some time!\n');
    end
    
    large_fname = paths.data;
    fprintf('loading large file: conn...');
    load( large_fname, 'conn' );
    conn = conn( data_dim, data_dim, data_dim, : );
    fprintf('done\n'); 

    fprintf('loading large file: im...');
    load( large_fname, 'im' );
    im = im( data_dim, data_dim, data_dim, : );
    fprintf('done\n');
    
    fprintf('saving reduced image volume...');
    save( small_fname, 'conn', 'im' );
    fprintf('\n');
end

% function createSegNumberLookupTable
    % hashmap = zeros(2^24,1);
    % hashedImage = zeros(size(seg95));    
    % counter = 0;
    % for i = 1 : length(seg95(:))
    %     if 0 ~= seg95(i)
    %         if 0 == hashmap( seg95(i) )
    %             hashmap( seg95(i) ) = counter;
    %             counter = counter + 1;
    %         end
    %             hashedImage( i ) = hashmap( seg95(i) );
    %     end
    % end
    %%
% end

function makeDir( dir )
    if ~exist( dir, 'dir' )
       mkdir( dir );
    end
end

function printLine()
    fprintf('----------------\n');
end