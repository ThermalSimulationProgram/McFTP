function [] = plotResults(periods, results)

figure;
for i = 1 : size(results,1)
    scatter(periods, results(i,:), 30, [0.9, 0.15,0.5123],'filled');
    hold on
    grid on
    xlabel('task period [ms]');
    ylabel('total overhead [%]', 'Rotation', 90);
end