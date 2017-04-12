function [overheads, totaltime] = getOverhead(filename)

m = csvread(filename);

main_thread = 0;

totaltime = m(m(:,1)==main_thread, 3);

overheads = zeros(1, 4);

for i = 1:4  
    overheads(i) = sum(m(m(:,1)==i, 3));  
end

% worker_num = max(m(:,1)==worker, 2) + 1;
% 
% for i = 1 : worker_num
%     
%     this_worker_times = m( m(:,1)==worker&m(:,2)==i-1, 3 );
%     overheads(5) = overheads(5) + max(this_worker_times) - min(this_worker_times); 
% end
    
    
    
    
    
    
    
    