function nhood = mknhood2(d)
% Makes nhood structures for some most used dense graphs.
% mknhood2(1) makes the nhood for a 6-connected lattice graph
% mknhood2(1.8) makes the nhood for a 26-connected lattice graph
%
% The neighborhood reference for the dense graph representation we use
% nhood(1,:) is a 3 vector that describe the node that conn(:,:,:,1) connects to
% so to use it: conn(23,12,42,3) is the edge between node [23 12 42] and [23 12 42]+nhood(3,:)
% See? It's simple! nhood is just the offset vector that the edge corresponds to.

[i,j,k] = meshgrid(-ceil(d):ceil(d),-ceil(d):ceil(d),-ceil(d):ceil(d));
idxkeep = find((i.^2+j.^2+k.^2)<=d.^2);
i=i(idxkeep); j=j(idxkeep); k=k(idxkeep);
zeroIdx = ceil(numel(i)/2);

% flipud and order (j,i,k) so that mknhood2(1) matches mknhood(6)
nhood = flipud([j(1:zeroIdx-1) i(1:zeroIdx-1) k(1:zeroIdx-1)]);
