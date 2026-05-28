function fig = plot_polar(fig, data, profile)
  figure(fig)
  clf
  plot(data(:,2),data(:,3))
  title(strcat('Linear Perturbations In the Normal and Parallel Direction for ',profile,' Profile'))
  xlabel('Parallel Perturbation / m')
  ylabel('Normal Perturbation / m')
  filename = strcat('figure_',num2str(fig),'.png');
  saveas( fig, filename);
  fig++;
endfunction
