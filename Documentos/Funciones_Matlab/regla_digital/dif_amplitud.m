function [  ] = dif_amplitud(  )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
t=1:1024;
T=1024;
r_seno=(2.32/2)*sin(t*2*pi/T);
r_coseno=((2.32+0.16)/2)*cos(t*2*pi/T);
r_arctan=atan2(r_seno,r_coseno);

for i=1:1024
    if(r_arctan(i)<0)
        r_arctan(i)= r_arctan(i)+2*pi;
    end
end


figure();
plot(r_seno)
hold on;
plot(r_coseno);
plot(r_arctan);
end

