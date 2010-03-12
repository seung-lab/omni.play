%threshold = .95;
zblksz = 200;
dustsz = 10;
low_threshold = .5;

tic,fprintf(['Running connected components...'])
seg = connectedComponents(conn>threshold,[],dustsz);
fprintf([' done. ']),toc
for k = 1:zblksz:size(conn,3),
	idx = k:min(size(conn,3),k+zblksz-1);
	fprintf(['idx=[' num2str(idx(1)) ' ' num2str(idx(end)) ']\n'])
	tic,fprintf(['Running marker-based watershed...'])
	seg(:,:,idx) = markerWatershed(conn(:,:,idx,:),[],seg(:,:,idx),[],low_threshold);
	fprintf([' done. ']),toc
end

save('-v7.3',['e1088_eight_giga_voxel_crop_' num2str(threshold*100)],'conn','seg','threshold','zblksz','dustsz','low_threshold')
hdf5write(['e1088_eight_giga_voxel_crop_' num2str(threshold*100) '.h5'],'/main',seg);
