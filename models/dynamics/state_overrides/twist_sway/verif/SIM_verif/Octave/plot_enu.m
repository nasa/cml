function fig = plot_enu(fig, data, profile)
  legend_arr =  {'East','North','Up'};

  hf = figure(fig);
  clf
  plot(data(:,1),data(:,5),'k','Linewidth',1)
  hold on
  [hax,h1,h2] = plotyy(data(:,1),data(:,6),data(:,1),data(:,7));
  set (h1, "color", "b");
  set (h2, "color", "r");
  ylabel(hax(1),'Horizontal Perturbation / m','color','b');
  ylabel(hax(2),'Vertical Perturbation / m','color','r');
  set(hax,{'ycolor'},{'k';'r'});
  grid
  title(strcat('ENU Perturbations for ',profile,' Profile'))
  xlabel('Time / s')
  legend( legend_arr,'Location','SouthEastOutside');

  filename = strcat('figure_',num2str(fig),'.png');
  saveas( fig, filename);
  fig++;
endfunction
