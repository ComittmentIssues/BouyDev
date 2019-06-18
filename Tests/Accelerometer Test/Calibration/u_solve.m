function x = u_solve(U,b,n)

% SYNTAX:
%   x = u_solve(U,b,n);
%
% DESCRIPTION:
%   Returns U*x = b
%----------------------------------------------------------------------------------------------
% Copyright (C) 2018 Do Yeou Ku
%----------------------------------------------------------------------------------------------

x(n) = b(n)/U(n,n);
for k=n-1:-1:1
    x(k) = (b(k)-U(k,k+1:n)*x(k+1:n)')/U(k,k);
end
end
