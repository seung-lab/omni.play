function flyOverTwo(qq, qq2)

[d1 d2 d3] = size(qq);

close all;
figure;


for x = [1:d1]
    subplot(1, 2, 1);
    imagesc(reshape(rem(qq(x, :, :), 64), d2, d3));
    subplot(1,2,2);
    imagesc(reshape(rem(qq2(x, :, :), 64), d2, d3));
    drawnow
    pause(0.02)
end

for x = [1:d3]
    subplot(1,2,1);
    imagesc(reshape(rem(qq(:, :, x), 64), d1, d2));
    subplot(1,2,2);
    imagesc(reshape(rem(qq2(:, :, x), 64), d1, d2));
    drawnow
    pause(0.02)
end

for x = [1:d2]
    subplot(1,2,1);
    imagesc(reshape(rem(qq(:, x, :), 64), d1, d3));
    subplot(1,2,2);
    imagesc(reshape(rem(qq2(:, x, :), 64), d1, d3));
    drawnow
    pause(0.02)
end
