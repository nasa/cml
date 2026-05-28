fpath = fileparts(mfilename('fullpath'));
addpath(fpath);
printpath = [fpath filesep '..' filesep '..' filesep 'docs' filesep];
logname = 'log_fault_variables.csv';
plotsize = [600 300];
close all

%% SIM_fault
runs ={'linear','linear_int','linear_periodic','linear_invalid_comparator',...
       'noise_gaussian_narrow','noise_gaussian_wide',...
       'noise_uniform_narrow','noise_uniform_wide',...
       'new_rate_discontinuous', 'new_rate_continuous',...
       'disable_linear_direct','disable_linear_indirect',...
       'disable_stale_direct','disable_stale_indirect',...
       'string_trigger'
       % RUN_sinewave is separate, it has a different number of data points
       };
runs = strcat('RUN_',runs);
title_ = strrep( runs, '_', '\_');

clear data
for i=1:size(runs,2)
  filename = strcat('../SIM_fault/',runs{i},'/log_fault_variables.csv');
  data(i,:,:)=csvread(filename,1,0);
end

i=0;
figure(1)
clf
for j=1:4
  i++;
  subplot(4,1,j)
  plot_subset( squeeze(data(i,:,:)), '', title_{i})
end
print([printpath 'verif_linear.png'], '-dpng', '-S600,1200')
print([printpath 'verif_linear.png'], '-dpng', '-S600,1200')

figure(2)
clf
for j=1:4
  i++;
  subplot(4,1,j)
  plot_subset( squeeze(data(i,:,:)), '', title_{i})
end
print([printpath 'verif_random.png'], '-dpng', '-S600,1200')

figure(3)
clf
for j=1:2
  i++;
  subplot(1,2,j)
  plot_subset( squeeze(data(i,:,:)), '', title_{i})
end
print([printpath 'verif_new_rate.png'], '-dpng', '-S800,400')

figure(4)
clf
for j=1:4
  i++;
  subplot(4,1,j)
  plot_subset( squeeze(data(i,:,:)), '', title_{i})
end
print([printpath 'verif_disable.png'], '-dpng', '-S600,1200')

figure(5)
clf
i++;
plot_subset( squeeze(data(i,:,:)), '', title_{i})
print([printpath 'verif_string_trigger.png'], '-dpng', '-S300,300')


clear data_s
data_s=csvread('../SIM_fault/RUN_sinewave/log_fault_variables.csv',1,0);
figure(6)
clf
plot_subset( data_s(:,:), '', 'RUN\_sinewave')
x=[0,2; 2,2; 7,-0.4; 7,1; 11,1; 11,-0.1; 13,4.3; 16,2; 19,2; 21,2; 24,3.5;
24,1; 28,2; 28,3.5; 32,3.5; 32,3; 34,3; 34,2.5; 35,2; 42,2];
hold on
plot(x(:,1),x(:,2),'ro','Linewidth',1)
print([printpath 'verif_sinewave.png'], '-dpng', '-S600,600')






%% SIM_fault_manager
fig=6;
cd ../SIM_fault_manager
% set legend strings:
leg = ['time'; 'var\_char'; 'var\_uchar'; 'var\_sint'; 'var\_usint'; 'var\_int';
       'var\_uint'; 'var\_lint'; 'var\_ulint'; 'var\_llint'; 'var\_ullint';
       'var\_float'; 'var\_double'; 'var\_bool'];

% RUN_bias
data_bias = csvread(['RUN_bias/' logname], 1, 0);
fig++; %7
figure(fig)
clf
set(fig, "Position", [0 0 [1 6].*plotsize])
subplot(6, 1, 1)
plot_subset(data_bias, leg, 'Regular Triggers', [2 3 5 10 13])
subplot(6, 1, 2)
plot_subset(data_bias, leg, 'Modify Parameters', 4)
subplot(6, 1, 3)
plot_subset(data_bias, leg, 'Disable Fault / Disable Trigger', [12 7])
subplot(6, 1, 4)
plot_subset(data_bias, leg, 'Fire Limit', 6)
subplot(6, 1, 5)
plot_subset(data_bias, leg, 'Periodic Triggers', [8 9])
subplot(6, 1, 6)
plot_subset(data_bias, leg, 'Random Trigger', 11)
print([printpath 'verif_bias.png'], '-dpng', '-S800,1100')

% RUN_scale
data_scale = csvread(['RUN_scale/' logname], 1, 0);
fig++; %8
figure(fig)
clf
set(fig, "Position", [0 0 [1 3].*plotsize])
subplot(3, 1, 1:2);
plot_subset(data_scale, leg, 'RUN\_scale results', [2 3 5:13])
subplot(3, 1, 3)
plot_subset(data_scale, leg, '', 4)
print([printpath 'verif_scale.png'], '-dpng', '-S800,600')

% RUN_overwrite
data_ow = csvread(['RUN_overwrite/' logname], 1, 0);
fig++; %9
figure(fig)
clf
set(fig, "Position", [0 0 [1 5].*plotsize])
subplot(5, 1, 1);
plot_subset(data_ow, leg, 'RUN\_overwrite results', 2)
subplot(5, 1, 2:3);
plot_subset(data_ow, leg, '', [3:10 12 13])
subplot(5, 1, 4)
plot_subset(data_ow, leg, '', 11)
subplot(5, 1, 5)
plot_subset(data_ow, leg, '', 14)
print([printpath 'verif_overwrite.png'], '-dpng', '-S800,1000')

% RUN_stale
data_stale = csvread(['RUN_stale/' logname], 1, 0);
fig++; %10
figure(fig)
set(fig, "Position", [0 0 [1 5].*plotsize])
title('RUN\_stale results')
subplot(5, 1, 1);
plot_subset(data_stale, leg, 'RUN\_stale results', 2)
subplot(5, 1, 2:3);
plot_subset(data_stale, leg, '', [7:13])
subplot(5, 1, 4);
plot_subset(data_stale, leg, '', 3:6)
subplot(5, 1, 5)
plot_subset(data_stale, leg, '', 14)
print([printpath 'verif_stale.png'], '-dpng', '-S800,1000')

% RUN_white_noise
data_wn = csvread(['RUN_white_noise/' logname], 1, 0);
fig++; %11
figure(fig)
set(fig, "Position", [0 0 [1 2].*plotsize])
subplot(2, 1, 1)
plot_subset(data_wn, '', 'RUN\_white\_noise Results', 2)
ylabel('var\_float')
subplot(2, 1, 2)
plot_subset(data_wn, '', '', 3)
ylabel('var\_double')
print([printpath 'verif_white_noise.png'], '-dpng', '-S600,600')

% RUN_random_walk
data_rw = csvread(['RUN_random_walk/' logname], 1, 0);
fig++; %12
figure(fig)
set(fig, "Position", [0 0 [1 2].*plotsize])
subplot(2, 1, 1)
plot_subset(data_rw,'','RUN\_random\_walk Results', 2)
ylabel('var\_float')
subplot(2, 1, 2)
plot_subset(data_rw,'','', 3)
ylabel('var\_double')
print([printpath 'verif_random_walk.png'], '-dpng', '-S600,600')

% RUN_function
data_func = csvread(['RUN_function/' logname], 1, 0);
fig++; %13
figure(fig)
set(fig, "Position", [0 0 [1 4].*plotsize])
subplot(4,1,1)
plot_subset(data_func, leg, 'Linear with Char Wrap-around',2:3)
subplot(4,1,2)
plot_subset(data_func, leg, 'Linear',4:6)
subplot(4,1,3)
plot_subset(data_func, leg, 'Square Wave',7:8)
subplot(4,1,4)
plot_subset(data_func, leg, 'Square Wave with Phase-offset',9)
print([printpath 'verif_function1.png'], '-dpng', '-S800,800')

fig++; %14
figure(fig)
set(fig, "Position", [0 0 [1 4].*plotsize])
subplot(4,1,1)
plot_subset(data_func, leg, 'Square Wave with Linear Phase-offset',10)
subplot(4,1,2)
plot_subset(data_func, leg, 'Square Wave with Linear Amplitude and Frequency',11)
subplot(4,1,3)
plot_subset(data_func, leg, 'Triangle Wave',12)
subplot(4,1,4)
plot_subset(data_func, leg, 'Compound Sine Wave',13)
print([printpath 'verif_function2.png'], '-dpng', '-S800,800')


data_trig = csvread(['RUN_triggers/' logname], 1, 0);
fig++; %15
figure(fig)
set(fig, "Position", [0 0 [1 5].*plotsize])
subplot(6,1,1)
plot_subset(data_trig, leg, 'RUN\_trigger Results',[14 3])
subplot(6,1,2)
plot_subset(data_trig, leg, '',2)
subplot(6,1,3)
plot_subset(data_trig, leg, '',4:6)
subplot(6,1,4)
plot_subset(data_trig, leg, '',7:11)
subplot(6,1,5)
plot_subset(data_trig, leg, '',12)
axis([0 1.3*max(data_trig(:,1)) 0.09997 0.10006])
subplot(6,1,6)
plot_subset(data_trig, leg, '',13)
print([printpath 'verif_triggers.png'], '-dpng', '-S800,800')

cd ../Octave
