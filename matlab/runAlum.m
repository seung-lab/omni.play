function runAlum(matFileNameAndPath)

loThreshold     = .3;
hiThreshold     = .95;
sizeThreshold   = 1000;
absLowThreshold = .3;

[dirName, matFileName] = fileparts(matFileNameAndPath);

%boundary trained networks 709,710; malis networks 711,712
watershed('conn709');
watershed('conn710');
watershed('conn711');
watershed('conn712');
saveChannelData();

printLineSep();
display('all done!');

    function saveChannelData()
        printLineSep();
        display('saving channel...');

        if(isempty(whos('-file', matFileNameAndPath, '-regexp', 'im')))
            display(sprintf('did not find %s in mat file\n', 'im'));
            return;
        end

        display('loading...');
        d = load(matFileNameAndPath, 'im');

        hdf5FileName = sprintf('%s_channel.hdf5', matFileName);

        printHalfLineSep();
        display('writing HDF5 file...');
        hdf5write(fullfile(dirName, hdf5FileName), ...
            '/chanSingle', d.('im'));

        clear('im');

        display('done!');
    end

    function watershed(connName)
        printLineSep();
        display(sprintf('running watershed on %s...', connName));

        if(isempty(whos('-file', matFileNameAndPath, '-regexp', connName)))
            display(sprintf('did not find %s in mat file\n', connName));
            return;
        end

        display('loading...');
        d = load(matFileNameAndPath, connName);

        printHalfLineSep();
        [seg graph graphValues dend dendValues] = QuickieWS(d.(connName), .3, .95, 1000, .3);
		      %loThreshold, hiThreshold, sizeThreshold, absLowThreshold);


        hdf5FileName = ...
            sprintf('%s_%s_%0.2f_%0.2f_%d_%0.2f.hdf5', ...
            matFileName, connName, loThreshold, hiThreshold, sizeThreshold, ...
            absLowThreshold);

        printHalfLineSep();
        display('writing HDF5 file...');
        outputFolder = fullfile(dirName, connName);
        mkdir(outputFolder);
        hdf5write(fullfile(outputFolder, hdf5FileName), ...
            '/main', seg, ...
            '/dend', dend, ...
            '/graph', graph, ...
            '/dendValues', dendValues, ...
            '/affGraphSingle', d.(connName));

        clear d seg graph graphValues dend dendValues;

        display('done!');
    end
end

function printLineSep
    display('******************************************');
end

function printHalfLineSep
    display('********************');
end
