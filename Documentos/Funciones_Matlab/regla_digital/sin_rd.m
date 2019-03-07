function [ resolucion_S, maxima, minima, estandar, relacion_vs_rect ] = sin_rd( bits  )

fig=figure;
hax=axes;
hold on;

resolucion_S=[]; % Formato [nivel(div),posicion(x),dif(posicion(i)-posicion(i-1))]
res_a=2^bits;
periodo=5;
t=0:periodo/1024:periodo;
t2=0:1:res_a-1;
plot(t,(res_a-1)/2*sin(t*2*pi()/periodo)+(res_a-1)/2);

for i=t2
    plot([0,periodo],[i,i])
end

syms x;
n=1;

for div=0:1:res_a-1
    eqn = (res_a-1)/2*sin(x*2*pi()/periodo)+(res_a-1)/2 == div;
    x_div = double(solve(eqn,x));
    num_sol=size(x_div,1);
    for i=1:num_sol
        a=x_div(i);
        if(a<0)
            a=periodo+a;
        end
        resolucion_S(n,1)=div;
        resolucion_S(n,2)=a;
        line([a a],get(hax,'YLim'),'Color',[1 0 0])
        n=n+1;
    end
end
resolucion_S=sortrows(resolucion_S,2);
scatter(resolucion_S(:,2),resolucion_S(:,1),'filled','blue')
ylim([0,res_a-1])
xlim([0,periodo])
title('A*Sin(x)+A/2');
xlabel('Distancia desplazada del sensor (mm)');
ylabel('Cuantificaci?n de la se?al en niveles discretos');

for i=2:size(resolucion_S(:,1))
    resolucion_S(i,3)=resolucion_S(i,2)-resolucion_S(i-1,2);
end

figure();
scatter(resolucion_S(2:end,2),resolucion_S(2:end,3))
title('Distancia entre cada nivel consecutivo');
xlabel('Distancia desplazada del sensor (mm)');
ylabel('Distancia entre el nivel actual y el anterior (mm)');

maxima=min(resolucion_S(2:end,3))
minima=max(resolucion_S(2:end,3))
estandar=periodo/res_a;
hold on;
plot([0,periodo],[estandar,estandar])

relacion_vs_rect=sum(resolucion_S(:,3)<periodo/res_a)*100/size(resolucion_S(:,3),1)
end

