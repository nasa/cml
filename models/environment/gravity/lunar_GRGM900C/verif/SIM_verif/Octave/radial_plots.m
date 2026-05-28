% Pick a data set by index
data_choice = ["spher"; "002x002"; "004x004"; "032x032"; "100x100"; "150x150";"600x600"];
data_set = data_choice(2,:)

% the rest is automatic.
data_lo = csvread(strcat('../RUN_',data_set,'__low_alt/log_test_data.csv'), 1,1);
data_hi = csvread(strcat('../RUN_',data_set,'__high_alt/log_test_data.csv'), 1,1);

for rad = 1:7
  radius( rad)   = data_lo(rad,1);
  radius( rad+7) = data_hi(rad,1);
end
lon_lat = ['(-180,-45)';'( -90,-45)';'(  0,-45)';'( 90,-45)';
           '(-180,  0)';'( -90,  0)';'(  0,  0)';'( 90,  0)';
           '(-180, 45)';'( -90, 45)';'(  0, 45)';'( 90, 45)'];
line_sig=['-o';'-x';'-*'];

for lat = 1:3
  for lon = 1:4
    for rad = 1:7
      g_magnitude(rad,  lon,lat) = data_lo( (lat-1)*28 + (lon-1)*7 + rad, 4);
      g_magnitude(rad+7,lon,lat) = data_hi( (lat-1)*28 + (lon-1)*7 + rad, 4);

      g_align(rad,  lon,lat) = data_lo( (lat-1)*28 + (lon-1)*7 + rad, 5);
      g_align(rad+7,lon,lat) = data_hi( (lat-1)*28 + (lon-1)*7 + rad, 5);
    end
  end
end

for lat = 1:3
  for lon = 1:4
    delta_mag(:,lon,lat) = (g_magnitude(:,lon,lat) - g_magnitude(:,3,2))./g_magnitude(:,3,2);
    delta_align(:,lon,lat) = (g_align(:,lon,lat) +1);
    for rad = 1:14
      align_angle(rad,lon,lat) = rad2deg(acos(-g_align(rad,lon,lat)));
    end
  end
end


figure(1)
clf
hold on
for lat = 1:3
  for lon = 1:4
    plot( radius, g_magnitude(:,lon,lat),'-o','Linewidth',1)
  end
end


f2=figure(2,  'position',[0,0,2000,1000])
clf
hold on
for lat = 1:3
  for lon = 1:4
    plot( radius, delta_mag(:,lon,lat),line_sig(lat,:),'Linewidth',2,'markersize',10)
  end
end
h=legend(lon_lat);
set(h,'fontsize',18)
title(strcat('Variation with Radius of the Delta in g Relative to (lat,lon) = (0,0) for field :',data_set),
             'fontsize',20)
xlabel('Radius / m','fontsize',20)
ylabel('Delta-g / g(0,0)','fontsize',20)
grid minor
set(gca, "linewidth", 4, "fontsize", 12)
axis([1.5E6, 1.0E7, min(min(min(delta_mag))),max(max(max(delta_mag)))])
print (strcat('delta_g_',data_set,'.png'),"-S2000,1000")

f3=figure(3,  'position',[0,1100,2000,1000])
clf
hold on
for lat = 1:3
  for lon = 1:4
    plot( radius, align_angle(:,lon,lat),line_sig(lat,:),'Linewidth',2,'markersize',10)
  end
end
h=legend(lon_lat);
set(h,'fontsize',18)
title(strcat('Variation with Radius of the Misalignment of vector g with vector r for field  :',data_set),
             'fontsize',20)
xlabel('Radius / m','fontsize',20)
ylabel('Angular misalignment / deg','fontsize',20)
grid minor
set(gca, "linewidth", 4, "fontsize", 12)
axis([1.5E6, 1.0E7, min(min(min(align_angle))),max(max(max(align_angle)))])
print (strcat('misalign_',data_set,'.png'),"-S2000,1000")




%saveas(f2, strcat('delta_g_',data_set,'.png'));
%saveas(f3, strcat('misalign_',data_set,'.png'));
