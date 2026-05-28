data_sim10=csvread('../RUN_10_alt_bias/log_lu_winds.csv',1,0);
data_sim1=csvread('../RUN_01_preload_5_nospec/log_lu_winds.csv',1,0);


filename_base = 'RUN_10_figure_';


fig = 1;
figure(fig)
clf
hold on
plot(data_sim1(1:8,2)/1000,data_sim1(1:8,13),'ko-')
plot(data_sim10(:,2)/1000,data_sim10(:,13),'ro-')
grid on
title('Variation with Altitude of West-to-East Wind')
xlabel('Altitude / km')
ylabel('Wind velocity / ms^{-1}')
%axis([0 100 -30 120])
legend(['RUN\_01';'RUN\_10';],'Location','NorthWest')
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
hold on
plot(data_sim1(1:8,2)/1000,data_sim1(1:8,12),'ko-')
plot(data_sim10(:,2)/1000,data_sim10(:,12),'ro-')
grid on
title('Variation with Altitude of South-to-North Wind')
xlabel('Altitude / km')
ylabel('Wind velocity / ms^{-1}')
%axis([0 100 -20 8])
legend(['RUN\_01';'RUN\_10';],'Location','NorthWest')
set(gca, "linewidth", 1, "fontsize", 12)
filename = strcat(filename_base,num2str(fig),'.png');
saveas( fig, filename);
fig++;

