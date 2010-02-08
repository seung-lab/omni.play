function comp = markerWatershed(conn,nhood,comp,grow_mask,low_threshold)
% MARKERWATERSHED	marker-based watershed segmentation of an affinity graph
% comp = markerWatershed(conn,nhood,mrkr,grow_mask,low_threshold)
% conn			a 4d 'conn' style connectivity graph
% nhood			the neighborhood associated with the 'conn' graph giving the interpretation of the edges (default is the nearest neighbor 6-connectivity corresponding to 3 edges per voxel)
% mrkr			marker segmentation (with zeros for regions to be grown into)
% grow_mask		a binary mask of pixels that are allowed to be grown into (default all ones)
% low_threshold	stopping threshold for watershed growing (default 0.5)


%% developed and maintained by Srinivas C. Turaga <sturaga@mit.edu>
%% do not distribute without permission.

% 6-connectivity is the default
if ~exist('nhood','var') || isempty(nhood),
	nhood = -eye(ndims(conn)-1);
end

% all zero pixels are grow-able
if ~exist('grow_mask','var') || isempty(grow_mask),
	grow_mask = comp==0;
end

% 0.5 is the default
if ~exist('low_threshold','var') || isempty(low_threshold),
	low_threshold = 0;
end


% % mark grow-able pixels by 0, but mark non-grow-able 0 pixels as -1
% comp(comp==0) = -1;
% comp(grow_mask) = 0;

conn = single(conn);
comp = uint32(comp);
nhood = double(nhood);
grow_mask = logical(grow_mask);
low_threshold = double(low_threshold);
comp = markerWatershedmex(conn,nhood,comp,grow_mask,low_threshold);
