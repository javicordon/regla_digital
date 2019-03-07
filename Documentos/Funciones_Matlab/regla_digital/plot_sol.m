function [ maxima_resolucion, minima_resolucion ] = plot_sol( senal_A, senal_B )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
periodo=5;
res_a=2^5;

hax=axes;
ylim([0,res_a-1])
xlim([0,periodo])


%Senal A
num_puntos=size(senal_A(:,1),1);
for div=1:1:num_puntos
    a=senal_A(div,2);
    line([a a],get(hax,'YLim'),'Color','blue')
end
hold on;
scatter(senal_A(:,2),senal_A(:,1),'filled','blue')

%Senal B
num_puntos=size(senal_B(:,1),1);
for div=1:1:num_puntos
    a=senal_B(div,2);
    line([a a],get(hax,'YLim'),'Color','red')
end
hold on;
scatter(senal_B(:,2),senal_B(:,1),'filled','red')

title('Nivel medido por el ADC en funcion de la posicion del sensor');
xlabel('Distancia desplazada del sensor (mm)');
ylabel('Cuantificaci?n de la se?al en niveles discretos');

figure;

posiciones=[];
posiciones=[senal_A(:,2);senal_B(:,2)];
posiciones=sort(posiciones);
posiciones(2:end,2)=posiciones(2:end,1)-posiciones(1:end-1,1);
plot(posiciones(2:end,1),posiciones(2:end,2),'*')
hold on
plot(posiciones(2:end,1),posiciones(2:end,2),'-')
title('Distancia entre cada nivel consecutivo');
xlabel('Distancia desplazada del sensor (mm)');
ylabel('Distancia entre el nivel actual y el anterior (mm)');

maxima_resolucion=max(posiciones(2:end,2));
minima_resolucion=min(posiciones(2:end,2));

end

