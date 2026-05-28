function fig = plot_enu(fig, data, profile)
  legend_arr =  {'x','y','z'};

  hf = figure(fig);
  clf
  plot(data(:,1),data(:,5),'Linewidth',1)
  hold on
  plot(data(:,1),data(:,6),'Linewidth',1)
  plot(data(:,1),data(:,7),'Linewidth',1)
  ylabel('ECEF Perturbation / m');
  grid
  title(strcat('ECEF Perturbations for ',profile,' Profile'))
  xlabel('Time / s')
  legend( legend_arr,'Location','NorthEastOutside');

  filename = strcat('figure_',num2str(fig),'.png');
  saveas( fig, filename);
  fig++;
endfunction

