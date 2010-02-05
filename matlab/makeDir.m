function makeDir( dir )
    if ~exist( dir, 'dir' )
       mkdir( dir );
    end
end
