function [ senal_adc ] = cuantizacion( seno, coseno , bits)
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here

%Cuantizacion de los valores de coseno, para seno
%Es decir, cual es el nivel que mide el ADC para coseno, cuando estoy
% midiendo seno. Debe ser discreto.

res_a=2^bits; %Niveles segun resolucion del ADC (bits)
periodo=5; %En milimetros

%Cuanto vale Coseno, para las soluciones que cuantizan de seno.
coseno_de_seno=(res_a-1)/2*cos(seno(:,2)*2*pi()/periodo)+(res_a-1)/2;
coseno_de_seno=round(coseno_de_seno,0);
seno(:,3)=coseno_de_seno;

%Cuanto vale Seno, para las soluciones que cuantizan de coseno.
seno_de_coseno=(res_a-1)/2*sin(coseno(:,2)*2*pi()/periodo)+(res_a-1)/2;
seno_de_coseno=round(seno_de_coseno,0);
coseno(:,3)=seno_de_coseno;

%Armado de una sola matriz con todos los resultados
temp_seno=seno(:,2); %Posicion en mm
temp_seno(:,2)=seno(:,1); %Nivel de Seno
temp_seno(:,3)=seno(:,3); %Nivel de Coseno
temp_coseno=coseno(:,2); %Posicion en mm
temp_coseno(:,2)=coseno(:,3); %Nivel de Seno
temp_coseno(:,3)=coseno(:,1); %Nivel de Seno

senal_adc=[temp_seno;temp_coseno]; %Matriz con todos los resultados
senal_adc=sortrows(senal_adc,1); %Ordeno por posicion dentro de periodo

arcotan=atan2(senal_adc(:,2)-res_a/2,senal_adc(:,3)-res_a/2);
arcotan=[arcotan(arcotan>=0);arcotan(arcotan<0)+2*pi()];
arcotan=5/(2*pi)*arcotan;
resolucion=arcotan(2:end)-arcotan(1:(end-1));

figure;
subplot(2,1,1);
plot(senal_adc(:,1),arcotan,'.')
title('Procesamiento del Arco-tangente de las senales muestreadas');
ylabel('Arco-tangente convertido de radianes a posicion lineal [mm]'),xlabel('Posicion lineal del sensor [mm]');

subplot(2,1,2);
plot(senal_adc(2:end,1),resolucion, '.')
title('Distancia entre puntos consecutivos');
ylabel('Diferencia entre cada punto y su proximo anterior'),xlabel('Posicion lineal [mm]');

min_res=max(resolucion)*1000

if(1)
    figure;
    scatter(temp_coseno(:,1),temp_coseno(:,2))
    hold on;
    scatter(temp_coseno(:,1),temp_coseno(:,3))

    figure;
    scatter(temp_seno(:,1),temp_seno(:,2))
    hold on;
    scatter(temp_seno(:,1),temp_seno(:,3))

    figure;
    scatter(senal_adc(:,1),senal_adc(:,2))
    hold on;
    scatter(senal_adc(:,1),senal_adc(:,3))
    
end



if (0)

    fromto=1
    upto=20;
    figure();
    hax=axes;
    scatter(seno(fromto:upto,2),coseno_de_seno(fromto:upto))
    hold on;
    scatter(coseno(fromto:upto,2),coseno(fromto:upto,1))
    scatter(seno(fromto:upto,2),seno(fromto:upto,1))
    
    for i=fromto:upto
        line([seno(i,2) seno(i,2)],get(hax,'YLim'),'Color','red')
    end
    title('Cuantificacion discreta de senales continuas');
    ylabel('Niveles discretos'),xlabel('Posicion lineal [mm]');
    
end






end

