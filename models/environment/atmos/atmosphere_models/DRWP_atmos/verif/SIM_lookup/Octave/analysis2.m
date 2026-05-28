data_calib=csvread('../Binaries/DRWP_w_comp__prof_2.csv',1,0);
data_sim=csvread('../RUN_02_preload_6_nospec/log_lu_winds.csv',1,0);


filename_base = 'RUN_02_figure_';


fig = 1;
figure(fig)
clf
hold on
plot(data_calib(:,1)/1000,data_calib(:,2),'r-')
plot(data_sim(:,2)/1000,data_sim(:,4),'ko')
plot(data_sim(:,2)/1000,data_sim(:,13),'bx')
grid on
title('Variation with Altitude of West-to-East Wind')
xlabel('Altitude / km')
ylabel('Wind velocity / ms^{-1}')
axis([0 105 -30 150])
legend(['Raw data';'u-component';'NED[1] component'],'Location','NorthWest')
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
hold on
plot(data_calib(:,1)/1000,data_calib(:,3),'r-')
plot(data_sim(:,2)/1000,data_sim(:,5),'ko')
plot(data_sim(:,2)/1000,data_sim(:,12),'bx')
grid on
title('Variation with Altitude of South-to-North Wind')
xlabel('Altitude / km')
ylabel('Wind velocity / ms^{-1}')
axis([0 105 -20 110])
legend(['Raw data';'v-component';'NED[0] component'],'Location','NorthWest')
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
hold on
plot(data_calib(:,1)/1000,-data_calib(:,4),'r-')
plot(data_sim(:,2)/1000,-data_sim(:,6),'ko')
plot(data_sim(:,2)/1000,data_sim(:,14),'bx')
grid on
title('Variation with Altitude of Downward Wind')
xlabel('Altitude / km')
ylabel('Wind velocity / ms^{-1}')
axis([0 105 -5 5])
legend(['-Raw data';'-w-component';'NED[2] component'],'Location','NorthWest')
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
hold on
semilogy(data_calib(:,1)/1000,data_calib(:,6),'r-')
semilogy(data_sim(:,2)/1000,data_sim(:,7),'ko')
grid on
title('Variation with Altitude of Mass Density')
xlabel('Altitude / km')
ylabel('Density / kg m^{-3}')
axis([0 105 1E-7 2])
legend(['Raw data';'computed data'],'Location','NorthEast')
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
hold on
semilogy(data_calib(:,1)/1000,data_calib(:,7)/1000,'r-')
semilogy(data_sim(:,2)/1000,data_sim(:,8)/1000,'ko')
grid on
title('Variation with Altitude of Static Pressure')
xlabel('Altitude / km')
ylabel('Pressure / kPa')
axis([0 105 1E-5 200])
legend(['Raw data';'computed data'],'Location','NorthEast')
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
hold on
plot(data_calib(:,1)/1000,data_calib(:,5),'r-')
plot(data_sim(:,2)/1000,data_sim(:,9),'ko')
grid on
title('Variation with Altitude of Temperature')
xlabel('Altitude / km')
ylabel('Temperature / K')
axis([0 105 180 300])
legend(['Raw data';'computed data'],'Location','NorthWest')
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
hold on
plot(data_sim(:,2)/1000,data_sim(:,11),'ko-')
grid on
title('Variation with Altitude of Speed of Sound')
xlabel('Altitude / km')
ylabel('Speed of Sound / ms^{-1}')
axis([0 105 270 350])
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
hold on
plot(data_sim(:,2)/1000,data_sim(:,16),'ko-')
grid on
title('Variation with Altitude of Wind Speed')
xlabel('Altitude / km')
ylabel('Wind speed / ms^{-1}')
axis([0 105 0 150])
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
hold on
plot(data_calib(129:132,1)/1000,data_calib(129:132,3),'ro-')
plot(data_sim(3,2)/1000,data_sim(3,5),'kx')
grid on
title('Data Point Relation Between Computed and Raw Data')
xlabel('Altitude / km')
ylabel('U-component Wind velocity / ms^{-1}')
legend(['Raw Data';'Computed Data'],'Location','SouthEast')
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;
