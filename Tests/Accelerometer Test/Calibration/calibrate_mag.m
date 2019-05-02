


function [ U, c ] = calibrate_mag( rawMagSet )
% performs magnetometer calibration from a set of data 
% using Merayo technique with a non iterative algoritm
% J.Merayo et al. "Scalar calibration of vector magnemoters"
% Meas. Sci. Technol. 11 (2000) 120-132.
%
%   X      : a Nx3 (or 3xN) data matrix
%              each row (columns) contains x, y, z measurements
%              N must be such that the data set describes
%              as completely as possible the 3D space
%              In any case N > 10
%              
%    The calibration tries to find the best 3D ellipsoid that fits the data set
%    and returns the parameters of this ellipsoid
%
%    U     :  shape ellipsoid parameter, (3x3) upper triangular matrix
%    c      : ellipsoid center, (3x1) vector
%
%    Ellipsoid equation : (v-c)'*(U'*U)(v-c) = 1 
%    with v a rough triaxes magnetometer  measurement
%
%    calibrated measurement w = U*(v-c)
%
%   author : Alain Barraud, Suzanne Lesecq 2008
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[N,m] = size(rawMagSet);
% check that X is not transposed
if m>3&&N==3
    rawMagSet = rawMagSet';
    N = m;
    m = 3;
end
% calibration fails due to not enough data
if N<=10,U = [];c = [];
    disp('magnetometer calibration fails due to not enough data');
    return;
end

% write  the ellipsoid equation as D*p=0
% the best parameter is the solution of min||D*p|| with ||p||=1;
% form D matrix from X measurements
x = rawMagSet(:,1); y = rawMagSet(:,2); z = rawMagSet(:,3); 
D = [x.^2, y.^2, z.^2, x.*y, x.*z, y.*z, x, y, z, ones(N,1)];
% avoids to compute the svd of a large matrix
D=triu(qr(D));
% because usually N may be very large
[U,S,V] = svd(D);
p = V(:,end);if p(1)<0,p =-p;end;

% the following matrix A(p) must be positive definite
% The optimization done by svd does not include such a constraint
% With "good" data the constraint is allways satisfied
% With too poor data A may fail to be positive definite
% In this case the calibration fails
A = [p(1) p(4)/2 p(5)/2;
     p(4)/2 p(2) p(6)/2; 
     p(5)/2 p(6)/2 p(3)];
[U,ok] = f_cholesky(m,A);
% calibration fails due to poor data
if ~ok,U = [];c = [];return;end 
b = [p(7);p(8);p(9)];
v = ut_solve(U,b/2,m);
d = p(10);
s = 1/sqrt(v*v'-d);
% ellipsoid center
c =-u_solve(U,v,m)';
% shape ellipsoid parameter
U = s*U;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
end