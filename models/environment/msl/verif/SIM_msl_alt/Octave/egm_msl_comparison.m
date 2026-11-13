% First, run the two seep runs with the resultions turned up to 1 degree

% read data
data_egm = csvread("../../../../EGM2008/verif/SIM_verif/RUN_sweep_10x10/log_test_data.csv",1,0);
data_msl = csvread("../RUN_sweep/log_test_data.csv",1,0);
delta = data_egm(:,4) + data_msl(:,5);

% convert to 2-D array
msl_grid(361,181)=0;
egm_grid(361,181) = 0;
ix = 1;
for jj = 1:181
  for ii = 1:361
    msl_grid(ii,jj) = data_msl(ix,5);
    egm_grid(ii,jj) = data_egm(ix,4);
    ix = ix+1;
  end
end

% shift the longitudes to create an "atlas" view
for ii = 1:181
  msl_atlas(ii,:) = msl_grid(ii+180,:);
  msl_atlas(ii+180,:) = msl_grid(ii,:);
  egm_atlas(ii,:) = egm_grid(ii+180,:);
  egm_atlas(ii+180,:) = egm_grid(ii,:);
end
msl_atlas = -msl_atlas;
delta_atlas = egm_atlas - msl_atlas;

% set lat and lon arrays
for ii = 1:181
  lat(ii) = ii-91;
  lon(ii) = ii -181;
  lon(ii+180) = ii-1;
end

% plot EGM Geoid datum wrt WGS-84
figure(1)
h1 = pcolor(lon,lat,egm_atlas');
set(h1, 'EdgeColor', 'None')
set(gca, 'FontSize',16)
title('Height Delta between EGM2008 Geoid and WGS-84 Ellipsoid; atlas view','fontsize',18)
xlabel('Longitude / degrees','fontsize',18)
ylabel('Latitude / degrees','fontsize',18)
caxis([-100 90])
colorbar('FontSize',16)
%saveas(1, "egm_wgs_atlas.pdf");

% plot MSL datum wrt WGS-84
figure(2)
h2=pcolor(lon,lat,msl_atlas');
set(h2, 'EdgeColor', 'None')
set(gca, 'FontSize',16)
title('Height Delta between MSL datum and WGS-84 Ellipsoid; atlas view','fontsize',18)
xlabel('Longitude / degrees','fontsize',18)
ylabel('Latitude / degrees','fontsize',18)
caxis([-100 90])
colorbar('FontSize',16)
%saveas(2, "msl_wgs_atlas.png");

% plot deltas
figure(3)
h3=pcolor(lon,lat,delta_atlas');
set(h3, 'EdgeColor', 'None')
set(gca, 'FontSize',16)
title('Height Delta between EGM2008 Geoid and MSL datum; atlas view','fontsize',18)
xlabel('Longitude / degrees','fontsize',18)
ylabel('Latitude / degrees','fontsize',18)
caxis([-100 90])
colorbar('FontSize',16)
%saveas(3, "egm_msl_atlas.png");

% plot deltas
figure(4)
h3=pcolor(lon,lat,delta_atlas');
set(h3, 'EdgeColor', 'None')
set(gca, 'FontSize',16)
title('Height Delta between EGM2008 Geoid and MSL datum; atlas view','fontsize',18)
xlabel('Longitude / degrees','fontsize',18)
ylabel('Latitude / degrees','fontsize',18)
colorbar('FontSize',16)
%saveas(4, "egm_msl_atlas1.png");

% plot point-deltas
figure(5)
clf
hold on
plot(data_egm(:,4),'k.','Linewidth',1)
plot(-data_msl(:,5),'b.')
plot(delta,'r.','Linewidth',1)
title('Height Deltas; individual points','fontsize',18)
ylabel('Height difference / m','fontsize',18)
set(gca, 'FontSize',16)
grid on
%saveas(5, "egm_msl_data_pts.png");

% plot histogram
figure(6)
hist(data_egm(:,4),100)
set(gca, 'FontSize',16)
title('Height Delta between EGM2008 Geoid and WGS-84 Ellipsoid; histogram','fontsize',18)
xlabel('Height difference / m','fontsize',18)
ylabel('Frequency','fontsize',18)
%saveas(6, "egm_wgs_histo.png");

% plot histogram
figure(7)
hist(-data_msl(:,5),100)
set(gca, 'FontSize',16)
title('Height Delta between MSL datum and WGS-84 Ellipsoid; histogram','fontsize',18)
xlabel('Height difference / m','fontsize',18)
ylabel('Frequency','fontsize',18)
%saveas(7, "msl_wgs_histo.png");

% plot histogram
figure(8)
hist(delta,100)
set(gca, 'FontSize',16)
title('Height Delta between  EGM2008 Geoid and MSL datum; histogram','fontsize',18)
xlabel('Height difference / m','fontsize',18)
ylabel('Frequency','fontsize',18)
axis([-150 100 0 10000])
%saveas(8, "egm_msl_histo.png");

% plot histogram
figure(9)
hist(delta,100)
set(gca, 'FontSize',16)
title('Height Delta between  EGM2008 Geoid and MSL datum; histogram','fontsize',18)
xlabel('Height difference / m','fontsize',18)
ylabel('Frequency','fontsize',18)
%saveas(9, "egm_msl_histo1.png");
