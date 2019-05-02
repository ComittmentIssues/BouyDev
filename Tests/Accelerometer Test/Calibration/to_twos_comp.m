function [ temp ] = to_twos_comp( val, bits )

v=val;
temp=single(0);

for i = 0:(bits-2)
    if bitand(v,2^i) == 2^i
        temp = double(temp+2^i);
    end
end

if bitand(v,2^(bits-1)) == 2^(bits-1)
    temp = double(temp-2^(bits-1));
end 
end