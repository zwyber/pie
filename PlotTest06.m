Info = load('test_06.csv');
figure;
hold on
LEGEND = cell(1,size(Info,2));
title('Kinetic Energy');
POT = zeros(1,size(Info,1));
KIN = zeros(1,size(Info,1));
for ii =1:2:size(Info,2)
    plot(Info(:,ii));
    LEGEND{1,ii} = ['Kinetic Energy of Object #', num2str((ii+1)/2)];
    KIN(1,:) =  KIN(1,:)+Info(:,ii)';
    plot(Info(:,ii+1));
    LEGEND{1,ii+1} = ['Potential Energy of Object #', num2str((ii+1)/2)];
    POT(1,:) = POT(1,:)+Info(:,ii+1)';
end
xlabel('Iteration');
ylabel('Energy');
legend(LEGEND)
hold off
figure;
hold on
plot(KIN);
plot(POT);
plot(sum(Info,2));
xlabel('Iteration');
ylabel('Energy');
legend('Total Kinetic Energy','Total Potential Energy', 'Total Energy');
hold off