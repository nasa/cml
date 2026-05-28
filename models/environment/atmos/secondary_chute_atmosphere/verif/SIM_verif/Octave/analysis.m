data = csvread('../verif_data/RUN_verif/log_test_data.csv' ,1,0);


figure(1)
clf
hold on
plot(data(:,1),data(:,3),'Linewidth',2)
for i=9:6:21
  plot(data(:,1),data(:,i),'Linewidth',1)
end
h = legend(['Payload';'dchute';'mchute';'schute'], 'Location','NorthEastOutside');
set( h,'fontsize',20)
title('Variation with Time of Altitude','fontsize',20)
xlabel('Time / s','fontsize',20)
ylabel('Altitude / m','fontsize',20)
grid minor
set(gca, "linewidth", 2, "fontsize", 16)

figure(2)
clf
hold on
plot(data(:,1),data(:,4),'Linewidth',2)
for i=10:6:22
  plot(data(:,1),data(:,i),'Linewidth',1)
end
h = legend(['Payload';'dchute';'mchute';'schute'], 'Location','NorthEastOutside');
set( h,'fontsize',20)
title('Variation with Time of Atmospheric Density','fontsize',20)
xlabel('Time / s','fontsize',20)
ylabel('Density / (kg m^{-3})','fontsize',20)
grid minor
set(gca, "linewidth", 2, "fontsize", 16)


figure(3)
clf
hold on
plot(data(:,3),data(:,4),'Linewidth',2)
for i=10:6:22
  plot(data(:,i-1),data(:,i),'Linewidth',1)
end
h = legend(['Payload';'dchute';'mchute';'schute'], 'Location','NorthEastOutside');
set( h,'fontsize',20)
title('Variation with Altitude of Atmospheric Density','fontsize',20)
xlabel('Altitude / m','fontsize',20)
ylabel('Density / (kg m^{-3})','fontsize',20)
grid minor
set(gca, "linewidth", 2, "fontsize", 16)

% check wind-vmag

for j = 5:8
  figure(j)
  clf
  hold on
  plot(data(:,1),data(:,j),'Linewidth',2)
  for i=j+6:6:j+18
    plot(data(:,1),data(:,i),'Linewidth',1)
  end
  h = legend(['Payload';'dchute';'mchute';'schute'], 'Location','NorthEastOutside');
  set( h,'fontsize',20)
  %title('Variation with Altitude of Atmospheric Density','fontsize',20)
  xlabel('Time / s','fontsize',20)
  %ylabel('Density / (kg m^{-3})','fontsize',20)
  grid minor
  set(gca, "linewidth", 2, "fontsize", 16)
end

figure(8)
title('Variation with Time of Wind Speed','fontsize',20)
ylabel('Wind Speed / (m s^{-1})','fontsize',20)

