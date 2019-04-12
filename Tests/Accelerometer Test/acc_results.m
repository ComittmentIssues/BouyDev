Nsamples = 11352;
Tsample = 1200023;
t = linspace(0,Tsample,Nsamples);

%Statistics

MeanAcc = [sum(Acc_X),sum(Acc_Y),sum(Acc_Z)]/Nsamples;
Acc_Std = [std(Acc_X),std(Acc_Y),std(Acc_Z)];

MeanX = repmat(MeanAcc(1),1,Nsamples);
MeanY = repmat(MeanAcc(2),1,Nsamples);
MeanZ = repmat(MeanAcc(3),1,Nsamples);

UppX = repmat((MeanAcc(1)+Acc_Std(1)),1,Nsamples);
UppY = repmat((MeanAcc(2)+Acc_Std(2)),1,Nsamples);
UppZ = repmat((MeanAcc(3)+Acc_Std(3)),1,Nsamples);

LowX = repmat((MeanAcc(1)-Acc_Std(1)),1,Nsamples);
LowY = repmat((MeanAcc(2)-Acc_Std(2)),1,Nsamples);
LowZ = repmat((MeanAcc(3)-Acc_Std(3)),1,Nsamples);
% plot

%Accelerometer
figure (1)
subplot(3,1,1);
plot(t,transpose(Acc_X),t,MeanX,t,UppX,t,LowX);title("X");xlabel("Time (s)");ylabel("accelartion (m/s^2)");
subplot(3,1,2);
plot(t,transpose(Acc_Y),t,MeanY,t,UppY,t,LowY);title("Y");xlabel("Time (s)");ylabel("accelartion (m/s^2)");
subplot(3,1,3);
plot(t,transpose(Acc_Z),t,MeanZ,t,UppZ,t,LowZ);title("Z");xlabel("Time (s)");ylabel("accelartion (m/s^2)");
sgtitle("3 axis Acceleromter data sampled at 10Hz for "+Nsamples+" samples");


MeanAcc = [sum(Mag_X),sum(Mag_Y),sum(Mag_Z)]/Nsamples
Acc_Std = [std(Mag_X),std(Mag_Y),std(Mag_Z)]

MeanX = repmat(MeanAcc(1),1,Nsamples);
MeanY = repmat(MeanAcc(2),1,Nsamples);
MeanZ = repmat(MeanAcc(3),1,Nsamples);

UppX = repmat((MeanAcc(1)+Acc_Std(1)),1,Nsamples);
UppY = repmat((MeanAcc(2)+Acc_Std(2)),1,Nsamples);
UppZ = repmat((MeanAcc(3)+Acc_Std(3)),1,Nsamples);

LowX = repmat((MeanAcc(1)-Acc_Std(1)),1,Nsamples);
LowY = repmat((MeanAcc(2)-Acc_Std(2)),1,Nsamples);
LowZ = repmat((MeanAcc(3)-Acc_Std(3)),1,Nsamples);
%Magnetometer
figure(2)
subplot(3,1,1);
plot(t,transpose(Mag_X),t,MeanX,t,UppX,t,LowX);title("X");xlabel("Time (s)");ylabel("\mu T");
subplot(3,1,2);
plot(t,transpose(Mag_Y),t,MeanY,t,UppY,t,LowY);title("Y");xlabel("Time (s)");ylabel("\mu T");
subplot(3,1,3);
plot(t,transpose(Mag_Z),t,MeanZ,t,UppZ,t,LowZ);title("Z");xlabel("Time (s)");ylabel("\mu T");
sgtitle("3 axis Magnetometer data sampled at 10Hz for "+Nsamples+" samples");
