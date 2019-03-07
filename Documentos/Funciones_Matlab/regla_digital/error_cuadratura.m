function [  ] = error_cuadratura(  )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
t=0:0.0001:2*pi();
error_fase=2*pi/2000;
A=2.32/2;
%error_ampltiud=0.16/2;
error_ampltiud=0;
arcotan=atan2(A*sin(t),A*cos(t));
arcotan_error=atan2(A*sin(t),(A+error_ampltiud)*cos(t-error_fase));

subplot(3,1,1);
plot(t,arcotan,t,arcotan_error);title('ArcTan2 Sin/Cos');
ylabel('Amplitud en [rad]'),xlabel('Resolucion por periodo [rad]');

arcotan=[arcotan(arcotan>=0),arcotan(arcotan<0)+2*pi()];
arcotan=5/(2*pi)*arcotan;

arcotan_error=[arcotan_error(arcotan_error>=0),arcotan_error(arcotan_error<0)+2*pi()];
arcotan_error=5/(2*pi)*arcotan_error;

subplot(3,1,2)
plot(t,arcotan,t,arcotan_error); title('Funcion lineal de posicion');
ylabel('Amplitud en [mm]'),xlabel('Resolucion por periodo [rad]');

subplot(3,1,3)
plot(t,arcotan-arcotan_error);title('Error por desfase en medicion');
ylabel('Error [mm]'),xlabel('Resolucion por periodo [rad]');



end

