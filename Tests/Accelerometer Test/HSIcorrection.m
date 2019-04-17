n= 1:568;
%Mean Values
M_ = [mean(M_x),mean(M_y),mean(M_z)];
Mmin = [min(M_x),min(M_y),min(M_z)];
Mmax = [max(M_x),max(M_y),max(M_z)];
figure(1);
scatter(M_x,M_y)
%remove Hard Iron

Mx_corr = M_x - M_(1);
My_corr = M_y - M_(2);
Mz_corr = M_z - M_(3);
% soft iron

r = sqrt(max(Mx_corr)^2+ max(My_corr)^2);
theta = asin(max(My_corr)/r);

R = [cos(theta) sin(theta); -sin(theta) cos(theta)];
V1 = zeros(2,568);
for i = n
    V1(:,i) = R*[Mx_corr(i);My_corr(i)];
end

sigma = max(V1(2,:))/max(V1(1,:))
V1 = V1*sigma;



figure(2);
scatter(V1(1,:),V1(2,:))

figure(3)
subplot(1,2,1);
plot(n,Mx_corr)
subplot(1,2,2)
plot(n,V1(1,:))



