runs = ['01738';'01750';'01800';'02000';'02200';'02500';'03000';'05000';'07000';'10000'];

num_runs = size(runs,1)

%Edit this for a subset:
run_ix_start = 1
run_ix_end = num_runs

for run_ix = 1: num_runs
  clear radius
  radius0 = runs(run_ix,:);
  radius = radius0;
  if (radius(1) == '0')
    radius = radius0(2:end);
  else
    radius = radius0;
  end


  clear data
  filename = strcat('../SET_150x150/RUN_',radius0,'/log_test_data.csv')
  data =  csvread(filename, 1,1);

  clear lon
  clear lat
  clear atlas
  clear atlas_alignment

  lon = 0:360;
  lat = -90:90;

  lon_size = size(lon,2);
  lat_size = size(lat,2);
  atlas(  lat_size, lon_size) = 0;
  atlas_alignment(  lat_size, lon_size) = 0;
  for lat_ix = 1:lat_size
    for lon_ix = 1:lon_size
      row = (lat_ix-1)*lon_size + lon_ix;
      atlas(lat_ix,lon_ix) = data(row, 4);
      atlas_alignment(lat_ix,lon_ix) = rad2deg(acos(-data(row, 5)));
    end
  end

  figure(1, 'position',[0,0,2000,1000])
  clf
  h1=pcolor(lon,lat,atlas);
  set(h1, 'EdgeColor', 'None')
  set(gca, 'FontSize',13)
  title(strcat('GRGM900C: Magnitude of Gravitational Acceleration at Radius:',radius,'km and Field: 150x150'),'fontsize',20)
  xlabel('Longitude / degrees','fontsize',20)
  ylabel('Latitude / degrees','fontsize',20)
  caxis([min(min(atlas)), max(max(atlas))])
  c1 = colorbar();
  ylabel(c1, 'magnitude / m s^{-2}','fontsize',20);

  print (strcat('gravity_map_',radius,'_150x150.png'),"-S2000,1000")

  figure(2, 'position',[0,1000,2000,1000])
  clf
  h1=pcolor(lon,lat,atlas_alignment);
  set(h1, 'EdgeColor', 'None')
  set(gca, 'FontSize',13)
  title(strcat('GRGM900C: Alignment Angle Between Gravity Vector and Radial Vector at Radius:',radius,'km and Field: 150x150'),'fontsize',20)
  xlabel('Longitude / degrees','fontsize',20)
  ylabel('Latitude / degrees','fontsize',20)
  caxis([min(min(atlas_alignment)), max(max(atlas_alignment))])
  c1 = colorbar();
  ylabel(c1, 'angle / degrees ','fontsize',20);

  print (strcat('alignment_map_',radius,'_150x150.png'),"-S2000,1000")

end
