data0 =  csvread('../RUN_00_off/log_basic_data.csv',1,0);
data1 =  csvread('../RUN_01_baseline/log_basic_data.csv',1,0);
data2 =  csvread('../RUN_02_derivative_update/log_basic_data.csv',1,0);
data3 =  csvread('../RUN_03_trans_motion/log_basic_data.csv',1,0);
data4a = csvread('../RUN_04a_velocity_effect_baseline/log_basic_data.csv',1,0);
data4b = csvread('../RUN_04b_velocity_effect_on/log_basic_data.csv',1,0);
data4c = csvread('../RUN_04c_velocity_effect_on_midsim/log_basic_data.csv',1,0);
data4d = csvread('../RUN_04d_velocity_effect_off_midsim/log_basic_data.csv',1,0);
data4e = csvread('../RUN_04e_velocity_effect_with_rot/log_basic_data.csv',1,0);
data4f = csvread('../RUN_04f_velocity_effect_with_rot_off_midsim/log_basic_data.csv',1,0);
data5a = csvread('../RUN_05a_order1/log_basic_data.csv',1,0);
data5b = csvread('../RUN_05b_order2_var_mass_rate/log_basic_data.csv',1,0);
data5c = csvread('../RUN_05c_order4/log_basic_data.csv',1,0);
data6a =  csvread('../RUN_06a_deactivate_baseline/log_basic_data.csv',1,0);
data6b =  csvread('../RUN_06b_deactivate_velocity_effect_off/log_basic_data.csv',1,0);
data6c =  csvread('../RUN_06c_deactivate_velocity_effect_on/log_basic_data.csv',1,0);

fig = 1;
figure(fig)
for ii = 1:3
  subplot(3,1,ii)
  plot(data3(:,1),data3(:,8+ii));
  grid on
  xlabel('Time /s')
  ylabel('Force/ N')
end
subplot(3,1,1)
title('Variation with Time of the Pseudo-force Vector');
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;


figure(fig)
for ii = 1:3
  subplot(3,1,ii)
  plot(data3(:,1),data3(:,11+ii));
  grid on
  xlabel('Time /s')
  ylabel('Accel/ m s^{-2}')
end
subplot(3,1,1)
title('Variation with Time of the Translational-acceleration Vector');
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);

title('Variation with Time of the Translational-acceleration Vector (x-, y-axes)');
subplot(3,1,3)
plot(data3(:,1),data3(:,15));
grid on
xlabel('Time /s')
ylabel('Angle / deg')
title('Variation with Time of the Rotation Angle');
filename = strcat('figure_',num2str(fig),'a.png');
saveas( fig, filename);
fig++;


figure(fig)
ax_lab = {'x','y'};
for ii = 1:2
  subplot(3,1,ii)
  plot(data3(:,1),data3(:,6+ii));
  grid on
  xlabel('Time /s')
  ylabel('Velocity/ m s^{-1}')
  title(strcat('Variation with Time of the Translational-velocity Vector (',ax_lab(ii),'-axis)'));
end
subplot(3,1,3)
plot(data3(:,1),data3(:,15));
grid on
xlabel('Time /s')
ylabel('Angle / deg')
title('Variation with Time of the Rotation Angle');
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
for ii = 1:2
  subplot(3,1,ii)
  plot(data3(:,15),data3(:,11+ii),'r');
  grid on
  xlabel('Rotation Angle / deg')
  ylabel('Accel/ m s^{-2}')
  title(strcat('Variation with Rotation Angle of the Translational-acceleration Vector (',ax_lab(ii),'-axis)'));
end
acc_mag = sqrt(data3(:,12).*data3(:,12) + data3(:,13).*data3(:,13));
subplot(3,1,3)
plot(data3(:,15),acc_mag,'r');
grid on
xlabel('Rotation Angle / deg')
ylabel('Accel/ m s^{-2}')
title('Variation with Rotation Angle of the Translational-acceleration Magnitude');
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;


figure(fig)
clf
plot(data3(:,5),data3(:,6),'r');
grid on
xlabel('X-axis Position / m')
ylabel('Y-axis Position / m')
title('Variation with X-position of Y-position');
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;

%%%%%%%%
% RUN_04
%%%%%%%%
figure(fig)
subplot(3,1,1)
title('Variation with Time of the Pseudo-dv Vector');
hold on
for ii = 1:3
  subplot(3,1,ii)
  plot(data4a(:,1),data4a(:,15+ii),'b','Linewidth',1.5);
  hold on
  plot(data4b(:,1),data4b(:,15+ii),'r','Linewidth',1.5);
  plot(data4c(:,1),data4c(:,15+ii),'k--','Linewidth',1);
  plot(data4d(:,1),data4d(:,15+ii),'g--','Linewidth',1);
  grid on
  xlabel('Time /s')
  ylabel('pseudo dv / m s^{-1}')
  legend({'4a pseudo-dv off','4b pseudo-dv on','4c pseudo-dv off-on','4d pseudo-dv on-off'},'Location','EastOutside')
end
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
for ii = 1:2
  subplot(2,1,ii)
  plot(data4a(:,1),data4a(:,6+ii),'b','Linewidth',1.5);
  hold on
  plot(data4b(:,1),data4b(:,6+ii),'r','Linewidth',1.5);
  plot(data4c(:,1),data4c(:,6+ii),'k--','Linewidth',1);
  plot(data4d(:,1),data4d(:,6+ii),'g--','Linewidth',1);
  grid on
  xlabel('Time /s')
  ylabel('body velocity / m s^{-1}')
  title(strcat('Variation with Time of the Velocity Vector (',ax_lab(ii),'-axis)'));
  legend({'4a pseudo-dv off','4b pseudo-dv on','4c pseudo-dv off-on','4d pseudo-dv on-off'},'Location','EastOutside')
end
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
for ii = 1:2
  subplot(2,1,ii)
  plot(data4a(:,1),data4a(:,4+ii),'b','Linewidth',1.5);
  hold on
  plot(data4b(:,1),data4b(:,4+ii),'r','Linewidth',1.5);
  plot(data4c(:,1),data4c(:,4+ii),'k--','Linewidth',1);
  plot(data4d(:,1),data4d(:,4+ii),'g--','Linewidth',1);
  grid on
  xlabel('Time /s')
  ylabel('body position / m')
  title(strcat('Variation with Time of the Position Vector (',ax_lab(ii),'-axis)'));
  legend({'4a pseudo-dv off','4b pseudo-dv on','4c pseudo-dv off-on','4d pseudo-dv on-off'},'Location','EastOutside')
end
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
hold on
for ii = 1:3
  plot(data4e(:,1),data4e(:,15+ii));
end
grid on
xlabel('Time /s')
ylabel('pseudo dv / m s^{-1}')
title('Variation with Time of the Pseudo-dv Vector for RUN 04e');
legend({'x','y','z'},'Location','EastOutside')
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
for ii = 1:2
  subplot(3,1,ii)
  plot(data3(:,1),data3(:,4+ii),'b','Linewidth',1.5);
  hold on
  plot(data4e(:,1),data4e(:,4+ii),'r','Linewidth',1.5);
  grid on
  xlabel('Time /s')
  ylabel('body position / m')
  title(strcat('Variation with Time of the Position Vector (',ax_lab(ii),'-axis)'));
  legend({'3 pseudo-dv off','4e pseudo-dv on'},'Location','EastOutside')
end
subplot(3,1,3)
plot(data3(:,1),data3(:,15));
hold on
plot(data4e(:,1),data4e(:,15),'r','Linewidth',1.5);
legend({'3 pseudo-dv off','4e pseudo-dv on'},'Location','EastOutside')
grid on
xlabel('Time /s')
ylabel('Angle / deg')
title('Variation with Time of the Rotation Angle');
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
for ii = 1:2
  subplot(3,1,ii)
  plot(data3(:,1),data3(:,6+ii),'b','Linewidth',1.5);
  hold on
  plot(data4e(:,1),data4e(:,6+ii),'r','Linewidth',1.5);
  grid on
  xlabel('Time /s')
  ylabel('body velocity / m s^{-1}')
  title(strcat('Variation with Time of the Velocity Vector (',ax_lab(ii),'-axis)'));
  legend({'3 pseudo-dv off','4e pseudo-dv on'},'Location','EastOutside')
end
subplot(3,1,3)
plot(data3(:,1),data3(:,15));
hold on
plot(data4e(:,1),data4e(:,15),'r','Linewidth',1.5);
legend({'3 pseudo-dv off','4e pseudo-dv on'},'Location','EastOutside')
grid on
xlabel('Time /s')
ylabel('Angle / deg')
title('Variation with Time of the Rotation Angle');
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
plot(data3(:,5),data3(:,6),'b');
hold on
plot(data4e(:,5),data4e(:,6),'r');
grid on
legend({'3 pseudo-dv off','4e pseudo-dv on'},'Location','EastOutside')
xlabel('X-axis Position / m')
ylabel('Y-axis Position / m')
title('Variation with X-position of Y-position');
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
plot(data3(:,5),data3(:,6),'b');
hold on
plot(data4e(:,5),data4e(:,6),'r');
plot(data4f(:,5),data4f(:,6),'r--');
grid on
legend({'3 pseudo-dv off','4e pseudo-dv on','4f pseudo-dv partial'},'Location','EastOutside')
xlabel('X-axis Position / m')
ylabel('Y-axis Position / m')
title('Variation with X-position of Y-position');
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;


%%%%
figure(fig)
clf
plot(data1(:,1),data1(:,3));
hold on
plot(data5a(:,1),data5a(:,3));
plot(data5b(:,1),data5b(:,3));
plot(data5c(:,1),data5c(:,3));
grid on
legend({'1 const rate','5a order 1','5b order 2', '5c order 4'},'Location','EastOutside')
xlabel('Time / s')
ylabel('Mass / kg')
title('Variation with Time of Body Mass');
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
axis([0 0.12 10.99 11])
filename = strcat('figure_',num2str(fig),'a.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
plot(data1(:,1),data1(:,2));
hold on
plot(data5a(:,1),data5a(:,2));
plot(data5b(:,1),data5b(:,2));
plot(data5c(:,1),data5c(:,2));
grid on
legend({'1 const rate','5a order 1','5b order 2', '5c order 4'},'Location','EastOutside')
xlabel('Time / s')
ylabel('Angular Rate / rad s^{-1}')
title('Variation with Time of Attitude Rate');
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
axis([0 0.12 0.9 1])
filename = strcat('figure_',num2str(fig),'a.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
plot(data1(:,3),data1(:,2));
hold on
plot(data5a(:,3),data5a(:,2));
plot(data5b(:,3),data5b(:,2));
plot(data5c(:,3),data5c(:,2));
grid on
legend({'1 const rate','5a order 1','5b order 2', '5c order 4'},'Location','EastOutside')
xlabel('Mass / kg')
ylabel('Angular Rate / rad s^{-1}')
title('Variation with Mass of Attitude Rate');
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
axis([10.985 11 0.9 1])
filename = strcat('figure_',num2str(fig),'a.png');
saveas( fig, filename);
fig++;


%%%%
figure(fig)
clf
subplot(2,1,1)
title('Variation with Time of the Pseudo-force Vector (y-axis)');
ylabel('Force/ N')
hold on
subplot(2,1,2)
title('Variation with Time of the Pseudo-torque Vector (z-axis)');
ylabel('Torque / N m')
hold on
for ii = 1:2
  subplot(2,1,ii)
  plot(data6a(:,1),data6a(:,6+ii),'b','Linewidth',2);
  hold on
  plot(data6b(:,1),data6b(:,6+ii),'k','Linewidth',1);
  plot(data6c(:,1),data6c(:,6+ii),'r--','Linewidth',1);
  grid on
  xlabel('Time /s')
%  ylabel('Force/ N')
%  title(strcat('Variation with Time of the Pseudo-force Vector (',ax_lab(ii),'-axis)'));
  legend({'6a baseline','6b pseudo-dv off','6c pseudo-dv on'},'Location','EastOutside')
end
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;

figure(fig)
clf
for ii = 1:2
  subplot(2,1,ii)
  plot(data6a(:,1),data6a(:,4+ii),'b','Linewidth',2);
  hold on
  plot(data6b(:,1),data6b(:,4+ii),'k','Linewidth',1);
  plot(data6c(:,1),data6c(:,4+ii),'r--','Linewidth',1);
  grid on
  xlabel('Time /s')
  ylabel('velocity / m s^{-1}')
  title(strcat('Variation with Time of the Velocity Vector (',ax_lab(ii),'-axis)'));
  legend({'6a baseline','6b pseudo-dv off','6c pseudo-dv on'},'Location','EastOutside')
end
filename = strcat('figure_',num2str(fig),'.png');
saveas( fig, filename);
fig++;

