data_sim7=csvread('../RUN_07_mid_run_switch_index/log_lu_winds.csv',1,0);
data_sim2=csvread('../RUN_02_preload_6_nospec/log_lu_winds.csv',1,0);
data_sim1=csvread('../RUN_01_preload_5_nospec/log_lu_winds.csv',1,0);


filename_base = 'RUN_07_figure_';


fig = 1;
figure(fig)
clf
hold on
plot(data_sim2(1:40,2)/1000,data_sim2(1:40,13),'bo-')
plot(data_sim1(1:40,2)/1000,data_sim1(1:40,13),'ko-')
plot(data_sim7(:,2)/1000,data_sim7(:,13),'ro-')
grid on
title('Variation with Altitude of West-to-East Wind')
xlabel('Altitude / km')
ylabel('Wind velocity / ms^{-1}')
axis([0 80 -30 120])
legend(['RUN\_02';'RUN\_01';'Switching';],'Location','NorthWest')
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
hold on
plot(data_sim2(1:40,2)/1000,data_sim2(1:40,12),'bo-')
plot(data_sim1(1:40,2)/1000,data_sim1(1:40,12),'ko-')
plot(data_sim7(:,2)/1000,data_sim7(:,12),'ro-')
grid on
title('Variation with Altitude of South-to-North Wind')
xlabel('Altitude / km')
ylabel('Wind velocity / ms^{-1}')
axis([0 80 -20 40])
legend(['RUN\_02';'RUN\_01';'Switching';],'Location','NorthWest')
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
hold on
plot(data_sim2(1:40,2)/1000,data_sim2(1:40,14),'bo-')
plot(data_sim1(1:40,2)/1000,data_sim1(1:40,14),'ko-')
plot(data_sim7(:,2)/1000,data_sim7(:,14),'ro-')
grid on
title('Variation with Altitude of Downward Wind')
xlabel('Altitude / km')
ylabel('Wind velocity / ms^{-1}')
axis([0 80 -5 2])
legend(['RUN\_02';'RUN\_01';'Switching';],'Location','SouthWest')
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
