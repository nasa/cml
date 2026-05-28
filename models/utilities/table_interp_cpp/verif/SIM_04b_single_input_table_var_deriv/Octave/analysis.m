data1a = csvread("../RUN_01a_default/log_test_data.csv",1,0);
data1f = csvread("../RUN_01f_1elem/log_test_data.csv",1,0);
data2a = csvread("../RUN_02a_default_no_xdot/log_test_data.csv",1,0);
data2b = csvread("../RUN_02b_default_linear/log_test_data.csv",1,0);

%Use the same functions from the verification sim to generate analytical data
%for comparisons.
x(21)= 0.0;
x_analytical(5,21) = 0.0;
xdot_analytical(5,21) = 0.0;
delta_sin(2,21)=0.0;
for ii = 1:21
  x4 = (ii-1) / 4.0;
  x(ii) = x4;
  x_analytical(1,ii) = 1.0;
  xdot_analytical(1,ii) = 0.0;

  x_analytical(2,ii) = x4+1.0;
  xdot_analytical(2,ii) = 1.0;

  x_analytical(3,ii) = (x4 - 1.5) * (x4 -4);
  xdot_analytical(3,ii) = 2*x4 - 5.5;

  x_analytical(4,ii) = (x4-1) * (x4-3) * (x4-5);
  xdot_analytical(4,ii) = (x4-3)*(x4-5) + (x4-1)*(x4-5) + (x4-1)*(x4-3);

  x_analytical(5,ii)    = sin(x4);
  xdot_analytical(5,ii) = cos(x4);

  delta_cube(1,ii) = data1a(ii,6) - x_analytical(4,ii);
  delta_cube(2,ii) = data2b(ii,6) - x_analytical(4,ii);
  delta_sin(1,ii) = data1a(ii,7) - x_analytical(5,ii);
  delta_sin(2,ii) = data2b(ii,7) - x_analytical(5,ii);
end


figure(1)
clf
hold on
plot(data1a(:,2),data1a(:,3),'x-')
plot(data2b(:,2),data1a(:,8),'x-')
plot(data1a(:,2),data1a(:,4),'o-')
plot(data2b(:,2),data1a(:,9),'o-')
grid on
legend(["const-x";"const-x-dot";"linear-x";"linear-x-dot"],'Location','NorthWest')
title("Var-Deriv Algorithm on Simple Functions")
saveas(1, "simple.png")


figure(2)
clf
hold on
plot(data1a(:,2),data1a(:,5),'x-')
plot(data2b(:,2),data2b(:,5),'x-')
plot(x(:),x_analytical(3,:),'+')
plot(data1a(:,2),data1a(:,10),'o-')
plot(data2b(:,2),data2b(:,10),'o-')
plot(x(:),xdot_analytical(3,:),'+')
grid on
legend(["x:var-deriv";"x:linear";"x:analytical";"x-dot:var-deriv";"x-dot:linear";"x-dot:analytical"],'Location','SouthEast')
title("Comparison for a Quadratic Function")
saveas(2, "quadratic.png")

figure(3)
clf
hold on
plot(data1a(:,2),data1a(:,6),'x-')
plot(data2b(:,2),data2b(:,6),'x-')
plot(x(:),x_analytical(4,:),'+')
plot(data1a(:,2),data1a(:,11),'o-')
plot(data2b(:,2),data2b(:,11),'o-')
plot(x(:),xdot_analytical(4,:),'+')
grid on
legend(["x:var-deriv";"x:linear";"x:analytical";"x-dot:var-deriv";"x-dot:linear";"x-dot:analytical"],'Location','NorthEast')
title("Comparison for a Cubic Function")
saveas(3, "cubic.png")

figure(4)
clf
hold on
plot(data1a(:,2),data1a(:,7),'x-')
plot(data2b(:,2),data2b(:,7),'x-')
plot(x(:),x_analytical(5,:),'+')
plot(data1a(:,2),data1a(:,12),'o-')
plot(data2b(:,2),data2b(:,12),'o-')
plot(x(:),xdot_analytical(5,:),'+')
grid on
legend(["x:var-deriv";"x:linear";"x:analytical";"x-dot:var-deriv";"x-dot:linear";"x-dot:analytical"],'Location','NorthEast')
title("Comparison for a Sine Function")
saveas(4, "sine.png")

figure(5)
clf
hold on
plot(x, delta_cube(1,:))
plot(x, delta_cube(2,:))
grid on
title('Variation Between Interpolation and Analytical Values')
legend(["delta:var-deriv";"delta:linear"],'Location','SouthEast')
saveas(5, "cubic_delta.png")


figure(6)
clf
hold on
plot(x, delta_sin(1,:))
plot(x, delta_sin(2,:))
grid on
title('Variation Between Interpolation and Analytical Values')
legend(["delta:var-deriv";"delta:linear"],'Location','SouthEast')
saveas(6, "sine_delta.png")
