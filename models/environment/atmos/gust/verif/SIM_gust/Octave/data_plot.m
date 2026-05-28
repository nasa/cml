data(1,:,:) = csvread('../RUN_0_verif/log_verif.csv',1,0);

for ix =1:1
  figure(ix)
  clf
  plot(data(ix,:,1),data(ix,:,2))
end
data(1,:,1)
data(1,:,2)
