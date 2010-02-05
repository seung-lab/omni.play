function collapse

    for k = 1:size(seg_imgs(:))
        if seg_imgs(k) == 0
           continue;
        end
       if cmpSz(seg_imgs(k)) < 400
            seg_imgs(k)=0;
       end
    end
end