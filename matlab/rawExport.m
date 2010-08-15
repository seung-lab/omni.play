function rawExport(pathToData, datasetToLoad)

chunkDim = 128;

d = load(pathToData, datasetToLoad);
dataNotOnChunkBoundary = d.(datasetToLoad);
clear d;

s = size(dataNotOnChunkBoundary);
fprintf('incoming affinity data: %d x %d x %d\n', s(1), s(2), s(3));
fprintf('padding matrix dimensions to multiples of 128; please wait...\n');
data = padarray(dataNotOnChunkBoundary, ...
		[chunkDim-(mod(s(1),chunkDim)) ...
		 chunkDim-(mod(s(2),chunkDim)) ...
		 chunkDim-(mod(s(3),chunkDim))], ...
		'post');
s = size(data);
fprintf('padded affinity data: %d x %d x %d\n', s(1), s(2), s(3));

fprintf('writing affinity data\n');
fid = fopen(sprintf('afffinity.%s.dat', datasetToLoad),'w');
fwrite(fid, data, 'single');
fclose(fid);

fprintf('writing blank watershed data\n');
fid = fopen(sprintf('watershed.%s.dat', datasetToLoad),'w');
fwrite(fid, zeros(s(1:3), 'uint32'), 'uint32');
fclose(fid);

end
