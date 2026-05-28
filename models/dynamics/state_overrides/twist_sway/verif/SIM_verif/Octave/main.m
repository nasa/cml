data0 = csvread('../RUN_00_baseline/log_test_data.csv',1,0 );
data1a= csvread('../RUN_01a_GrowHoldDecay/log_test_data.csv',1,0 );
data1b= csvread('../RUN_01b_ImpulseDecay/log_test_data.csv',1,0 );
data1c= csvread('../RUN_01c_GrowDecay/log_test_data.csv',1,0 );
data3 = csvread('../RUN_03_random_seed/log_test_data.csv',1,0 );
data4 = csvread('../RUN_04_test_slow/log_test_data.csv',1,0 );
data5a= csvread('../RUN_05a_base_enu/log_test_data.csv',1,0 );
data5b= csvread('../RUN_05b_new_wind_direction/log_test_data.csv',1,0 );
data5c= csvread('../RUN_05c_change_wind_direction/log_test_data.csv',1,0 );
data6 = csvread('../RUN_06_closeout_tailoff/log_test_data.csv',1,0 );
data7 = csvread('../RUN_07_excessive_twist_sway/log_test_data.csv',1,0 );
data8a = csvread('../RUN_08a_ecef_xaxis/log_test_data.csv',1,0 );
data8b = csvread('../RUN_08b_ecef_yaxis/log_test_data.csv',1,0 );

fig=1;
fig = plot_profiles(fig, data1a,' GrowHoldDecay');
fig = plot_profiles(fig, data1b, ' ImpulseDecay');
fig = plot_profiles(fig, data1c,' GrowDecay');

fig = plot_npt(fig, data0(1:400,:), ' Baseline');
fig = plot_polar(fig, data0, ' Baseline');

fig = plot_npt(fig, data0, ' Baseline');
fig = plot_npt(fig, data3, ' Seed = 1');

fig = plot_profiles(fig, data4, ' Slow Test');

fig = plot_npt(fig, data5a, ' ENU Baseline');
fig = plot_enu(fig, data5a, ' ENU Wind Bearing 0');
fig = plot_enu(fig, data5b, ' ENU Wind Bearing 90');
fig = plot_enu(fig, data5c, ' ENU Wind Bearing 0->90');

fig = plot_enu_vel(fig, data5a, ' ENU Baseline');
fig = plot_attitude( fig, data5a, ' ENU');


fig = plot_npt(fig, data6, ' Close-out Tail-off');
del_data = data6;
del_data(:,2:4) = data0(1:length(data6),2:4);
fig = plot_npt(fig, del_data,' Baseline');
del_data(:,2:4) = data6(:,2:4)-data0(1:length(data6),2:4);
fig = plot_npt(fig, del_data, ' Delta');

fig = plot_npt(fig, data7, ' Excessive Twist-Sway');


fig = plot_ecef(fig, data8a, ' X-axis Positioned');
fig = plot_ecef(fig, data8b, ' Y-axis Positioned');
fig = plot_ecef_vel(fig, data8a, ' X-axis Positioned');
fig = plot_ecef_vel(fig, data8b, ' Y-axis Positioned');
fig = plot_attitude( fig, data8a, ' ECEF X-axis Positioned');
fig = plot_attitude( fig, data8b, ' ECEF Y-axis Positioned');
