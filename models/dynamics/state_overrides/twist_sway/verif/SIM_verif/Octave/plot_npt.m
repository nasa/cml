function fig = plot_npt(fig, data, profile)
  legend_arr =  {'Parallel','Normal','Twist'};

  hf = figure(fig);
  clf
  plot(data(:,1),data(:,2),'k')
  hold on
  [hax,h1,h2] = plotyy(data(:,1),data(:,3),data(:,1),data(:,4));
  set (h1, "color", "b");
  set (h2, "color", "r");
  ylabel(hax(1),'Linear Perturbation / m','color','b');
  ylabel(hax(2),'Twist Perturbation / rad','color','r');
  set(hax,{'ycolor'},{'k';'r'});
  grid
  title(strcat('Perturbations for ',profile,' Profile'))
  xlabel('Time / s')
  legend( legend_arr,'Location','NorthEastOutside');
  filename = strcat('figure_',num2str(fig),'.png');
  saveas( fig, filename)
  fig++;
endfunction
