Info = load('test_03.csv');

Radii = [1,2];

%plot(Info(:,1), Info(:,2), Info(:,3), Info(:,4));

minX = min(min(Info(:,1:2:(size(Info,2)-1))));
maxX = max(max(Info(:,1:2:(size(Info,2)-1))));
minY = min(min(Info(:,2:2:size(Info,2))));
maxY = max(max(Info(:,2:2:size(Info,2))));


%% Video Plot
fig = figure;
set(fig,'position',[10,10, 720, 480]);
mov = VideoWriter('output.avi');
open(mov);
for m=1:size(Info,1)
    plot(Info(m,1), Info(m,2), '.',Info(m,3), Info(m,4),'.');
    ylim([minY,maxY]);
    xlim([minX,maxX]);
    axis equal;
    F = getframe(fig);
    writeVideo(mov, F);
end
close(mov);