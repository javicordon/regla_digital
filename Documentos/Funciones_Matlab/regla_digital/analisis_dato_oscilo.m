function [  ] = analisis_dato_oscilo(  )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

maximo=[];
minimo=[];

for i=1:12

a=csvread((sprintf('ReglaDigital_Oscilo/NewFile%d.csv',i)),2,1);

if (0)
    figure()
    plot(a(:,1))
    hold on
    plot(a(:,2))
    ylabel('Tension en volt')
    xlabel('puntos muestreados')
end

maximo(i,1)=max(a(:,1));
minimo(i,1)=min(a(:,1));
maximo(i,2)=max(a(:,2));
minimo(i,2)=min(a(:,2));
end
maximo
minimo
promedio=(maximo+minimo)/2
ampltud_pp=maximo-minimo