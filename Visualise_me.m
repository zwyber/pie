Info = load('CSV/test_03.csv');
IterationSteps = 10;
Radii = [1,1];
maxR = max(Radii);
CircCorners = 100;
Circs = zeros(length(Radii)*2, CircCorners);
t = linspace(0, 2*pi, CircCorners);
for ii=1:2:(length(Radii)*2)
    Circs(ii,:) = cos(t).*Radii((ii+1)/2);
    Circs(ii+1,:) = sin(t).*Radii((ii+1)/2);
end
colours = zeros(length(Radii),3);
colours(1,:) = [0.2, 0.2, 1];
colours(2,:) = [1, 0.2, 0.2];
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
for m=1:IterationSteps:size(Info,1)
    %plot(Info(m,1), Info(m,2), '.',Info(m,3), Info(m,4),'.');
    %pdecirc(Info(m,1),Info(m,2),Radii(1));
    %pdecirc(Info(m,3),Info(m,4),Radii(2));
%     viscircles(Info(m,1:2),Radii(1));
%     viscircles(Info(m,3:4),Radii(2));
%    plot(0,0,'.');
    for qq = 1:2:size(Circs,1)
        fill(Circs(qq,:)+Info(m,qq), Circs(qq+1,:)+Info(m,qq+1), colours((qq+1)/2,:));
        hold on
    end
    axis equal;
    ylim([minY-maxR,maxY+maxR]);
    xlim([minX-maxR,maxX+maxR]);
    set(gca,'nextplot','replacechildren');
    set(gcf,'Renderer','zbuffer');
    F = getframe(fig);
    writeVideo(mov, F);
    hold off
end
close(mov);