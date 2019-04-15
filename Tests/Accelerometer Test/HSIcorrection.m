n= 1:568;
%Mean Values
M_ = [mean(M_x),mean(M_y),mean(M_z)];
Mmin = [min(M_x),min(M_y),min(M_z)];
Mmax = [max(M_x),max(M_y),max(M_z)];

%remove Hard Iron

Mx_corr = M_x - M_(1);
My_corr = M_y - M_(2);
Mz_corr = M_z - M_(3);

figure(1)
plot(M_x,M_y)

figure(2)
subplot(1,2,1);
plot(n,M_x)
subplot(1,2,2)
plot(n,Mx_corr)



