function plot_subset(data, leg, title_, ind = 2:size(data, 2))
  plot(data(:, 1), data(:, ind))
  xlabel('Trigger variable (time)')
  ylabel('test\_object.var')
  set(gca, 'gridlinestyle','--','gridcolor',[0.8,0.8,0.8], 'GridAlpha',1, 'FontSize',10)
  grid
  min_data =  min(min(data(:,ind)));
  max_data =  max(max(data(:,ind)));

  cushion = 0.1*(max_data - min_data + 0.1);
  axis([0 max(data(:,1)) min_data - cushion max_data + cushion]);

  if (~isempty(leg))
    h = legend(leg(ind, :), 'Location', 'East');
    set(h, 'FontSize',10);
    axis([0 1.3*max(data(:,1)) min_data - cushion max_data + cushion]);
  end
  if (~isempty(title_))
    title(title_, 'FontSize',12)
  end
end
