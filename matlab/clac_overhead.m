pathprefix = 'period-';
fileprefix = 'random';
filepostfix = '_pboo_runtimes.csv';

periods = 30 : 1 : 100;
random_num = 15;

results = zeros(random_num, numel(periods));

j = 1;
for p = periods
    
    for i = 0 : random_num-1
        
        filename = [pathprefix, num2str(p), '/', fileprefix, num2str(i), filepostfix];
        [overheads, totaltime] = getOverhead(filename);
        
        results(i+1, j) = 100*sum(overheads)/totaltime;
        
    end
    
    j = j + 1;
    
end





