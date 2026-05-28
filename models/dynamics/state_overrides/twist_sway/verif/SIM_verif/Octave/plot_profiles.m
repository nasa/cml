function fig = plot_profiles(fig, data, profile)
  legend_arr =  {'Fast Perturbation Bound','Oscillation within Fast',...
                 'Slow Perturbation', 'Total Perturbation'};

  sinusoid = squeeze(data(:,2) - data(:,8));
  hf = figure(fig);
  clf
  plot(data(:,1),data(:,5),'Linewidth',1)
  hold on
  plot(data(:,1),sinusoid(:))
  plot(data(:,1),data(:,8),'Linewidth',1)
  plot(data(:,1),data(:,2))
  grid
  title(strcat('Linear Perturbations In the Parallel Direction for ',profile,' Profile'))
  xlabel('Time / s')
  ylabel('Perturbation / m')
  legend( legend_arr,'Location','NorthEastOutside');
  filename = strcat('figure_',num2str(fig),'.png');
  saveas( fig, filename);


  fig++;
  sinusoid = squeeze(data(:,3) - data(:,9));
  hf = figure(fig);
  clf
  plot(data(:,1),data(:,6),'Linewidth',1)
  hold on
  plot(data(:,1),sinusoid(:))
  plot(data(:,1),data(:,9),'Linewidth',1)
  plot(data(:,1),data(:,3))
  grid
  title(strcat('Linear Perturbations In the Normal Direction for ',profile,' Profile'))
  xlabel('Time / s')
  ylabel('Perturbation / m')
  legend( legend_arr,'Location','NorthEastOutside')
  filename = strcat('figure_',num2str(fig),'.png');
  saveas( fig, filename);


  fig++;
  sinusoid = squeeze(data(:,4) - data(:,10));
  hf = figure(fig);
  clf
  plot(data(:,1),data(:,7),'Linewidth',1)
  hold on
  plot(data(:,1),sinusoid(:))
  plot(data(:,1),data(:,10),'Linewidth',1)
  plot(data(:,1),data(:,4),'--','Linewidth',1)
  grid
  title(strcat('Twist Angle Perturbations for ',profile,' Profile'))
  xlabel('Time / s')
  ylabel('Perturbation / rad')
  legend( legend_arr,'Location','NorthEastOutside')
  filename = strcat('figure_',num2str(fig),'.png');
  saveas( fig, filename);
  fig++;
endfunction
