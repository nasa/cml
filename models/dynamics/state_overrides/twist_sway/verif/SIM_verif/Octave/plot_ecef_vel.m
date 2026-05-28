function fig = plot_enu_vel(fig, data, profile)
  legend_arr =  {'x','y','z'};

  hf = figure(fig);
  clf
  for ii = 1:3
    subplot(3,1,ii)
    [hax,h1,h2] = plotyy(data(:,1),data(:,4+ii),data(:,1),data(:,7+ii));
    set (h1, "color", "b");
    set (h2, "color", "r");
    if (ii == 2)
      ylabel(hax(1),'Position Perturbation / m','color','b');
      ylabel(hax(2),'Velocity Perturbation / m s^{-1}','color','r');
    endif

    set(hax,{'ycolor'},{'k';'r'});
    grid
    xlabel('Time / s')
  end
  subplot(3,1,1)
  title(strcat('3-axis ECEF PV Perturbations for ',profile,' Profile'))
  subplot(3,1,2)


  filename = strcat('figure_',num2str(fig),'.png');
  saveas( fig, filename);
  fig++;
endfunction

