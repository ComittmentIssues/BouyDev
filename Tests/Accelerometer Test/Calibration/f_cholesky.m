function [A,ok] = f_cholesky(n,A)

% SYNTAX:
%   [ temp ] = to_twos_comp( val, bits );
%
% DESCRIPTION:
%   Performs Cholesky factorisation
%----------------------------------------------------------------------------------------------
% Copyright (C) 2018 Do Yeou Ku
%----------------------------------------------------------------------------------------------

A(1,1:n) = A(1,1:n)/sqrt(A(1,1));
A(2:n,1) = 0;

    for j = 2:n
      A(j,j:n) = A(j,j:n) - A(1:j-1,j)'*A(1:j-1,j:n);
      if A(j,j)<=0
          ok = 0;
          break;
      end
      A(j,j:n) = A(j,j:n)/sqrt(A(j,j));
      A(j+1:n,j) = 0;
    end
ok=1;
end

