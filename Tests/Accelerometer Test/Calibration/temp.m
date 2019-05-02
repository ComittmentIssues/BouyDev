data = [x y z];
n = 1:length(z);
[U,c] = calibrate_mag(data);

figure(1);
subplot(3,1,1);plot(n,x);
subplot(3,1,2);plot(n,y);
subplot(3,1,3);plot(n,z);


data_cal = data*U; 
x_cal = data_cal(:,1)-c(1);
y_cal = data_cal(:,2)-c(2);
z_cal = data_cal(:,3)-c(3);
figure(2);
subplot(3,1,1);plot(n,x_cal,n,max(x_cal),n,min(x_cal));
subplot(3,1,2);plot(n,y_cal);
subplot(3,1,3);plot(n,z_cal);
