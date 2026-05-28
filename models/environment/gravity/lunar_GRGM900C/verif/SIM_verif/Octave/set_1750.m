runs = ['0x0';'4x4';'16x16';'64x64';'150x150';'600x600';'900x900';'150x150_025'];

num_runs = size(runs,1)

%Edit this for a subset:
run_ix_start = 1
run_ix_end = num_runs

for run_ix = 1: num_runs
  fidelity = runs(run_ix,:)

  clear data
  filename = strcat('../SET_1750/RUN_',fidelity,'/log_test_data.csv')
  data =  csvread(filename, 1,1);

  clear lon
  clear lat
  clear atlas

  if (run_ix <= 7)
    lon = 0:360;
    lat = -90:90;
  else
    lon = 0:0.25:360;
    lat = -90:0.25:90;
  end

  lon_size = size(lon,2);
  lat_size = size(lat,2);
  atlas(  lat_size, lon_size) = 0;
  for lat_ix = 1:lat_size
    for lon_ix = 1:lon_size
      row = (lat_ix-1)*lon_size + lon_ix;
      atlas(lat_ix,lon_ix) = data(row, 4);
    end
  end

  figure(1, 'position',[0,0,2000,1000])
  clf
  h1=pcolor(lon,lat,atlas);
  set(h1, 'EdgeColor', 'None')
  set(gca, 'FontSize',13)
  title(strcat('GRGM900C: Magnitude of Gravitational Acceleration at Radius 1750 km and Field:',fidelity),'fontsize',20)
  xlabel('Longitude / degrees','fontsize',20)
  ylabel('Latitude / degrees','fontsize',20)
  caxis([min(min(atlas)), max(max(atlas))])
  c1 = colorbar();
  ylabel(c1, 'magnitude / m s^{-2}','fontsize',20);

  % don't try printing the quarter-degree resolution image, it is huge.
  if (run_ix <=7)
    print (strcat('gravity_map_1750_',fidelity,'.png'),"-S2000,1000")
  end
end
