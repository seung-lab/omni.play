fid = fopen('testSizes3.txt', 'w');
fprintf(fid, 'digraph hierarchy {\n');

for ind = 1:size(dend)
    node1 = dend(ind, 1);
    node2 = dend(ind, 2);
    size1 = sizes( node1, 2);
    if( size1 < 0.01 )
        size1 = 0.01;
    end
    size2 = sizes( node2, 2);
    if( size2 < 0.01 )
        size2 = 0.01;
    end
    nodeName1 = sprintf('%d %0.2f', node1, size1 );
    nodeName2 = sprintf('%d %0.2f', node2, size2 );
    threshold = dend(ind, 3);
    fprintf(fid, '%d [shape=circle,width=%0.2f,label="%s"];\n', node1, size1, nodeName1 );
    fprintf(fid, '%d [shape=circle,width=%0.2f,label="%s"];\n', node2, size2, nodeName2 );
    
    if( threshold < 0.33 )
        fcolor = 'blue';
    elseif( threshold < 0.66)
        fcolor = 'green';
    else
        fcolor = 'red';
    end
    fprintf(fid, '%d->%d [arrowsize=%f, fillcolor="%s"]\n', node1, node2, threshold, fcolor);
end

fprintf(fid, '\n}\n');

fclose(fid);

display('done!');