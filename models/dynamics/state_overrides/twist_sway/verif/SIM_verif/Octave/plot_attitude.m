function fig = plot_attitude(fig, data, profile)
  figure(fig)
  clf
  plot(data(:,1),data(:,14:16))
  grid
  title(strcat('Variation with Time of Components of TS x-axis Expressed in ',profile))
  xlabel('Time /s')
  ylabel('Components')
  legend({'x-axis rotation','y-axis rotation','z-axis rotation'},'Location','NorthEastOutside')
  filename = strcat('figure_',num2str(fig),'.png');
  saveas( fig, filename);
  fig++;

  figure(fig)
  clf
  for ii = 1:3
    subplot(3,1,ii)
    [hax,h1,h2] = plotyy(data(:,1),data(:,10+ii),data(:,1),data(:,13+ii));
    grid
    set (h1, "color", "b");
    set (h2, "color", "r");
    if (ii == 2)
      ylabel(hax(1),'Attitude Rate / rad s^{-1}','color','b');
      ylabel(hax(2),'TS vector component','color','r');
    endif
  end
  subplot(3,1,1)
  title(strcat('Variation with Time of Attitude Rate of TS Relative to ',profile))
  subplot(3,1,3)
  xlabel('Time /s')
  filename = strcat('figure_',num2str(fig),'.png');
  saveas( fig, filename);
  fig++;

endfunction

