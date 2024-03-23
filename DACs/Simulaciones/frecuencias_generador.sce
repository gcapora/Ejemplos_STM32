/*******************************************************************************
* ISPEL: Simulación del error en la selección de frecuencia
********************************************************************************
* Se simula la diferencia entre la frecuencia objetivo (o configurada o deseada) 
* y la frecuencia que efectivamente logra el generador del proyecto ISPEL.
*
* UNDAV, marzo de 2024.
*******************************************************************************/

//------------------------------------------------------------------------------
// Configuración establecida en placa STM32 Nucleo-429ZI

DivisorA = 9;
DivisorMinimoB = 2;
FrecBase = 90e6 / DivisorA;
MuestrasSenialMax = 5000;

//------------------------------------------------------------------------------
// Variables de simulación

ef = -1:0.00125:5;
Pruebas = length(ef);

FrecSenialObjetivo    = zeros (1,Pruebas);
DivisorB              = zeros (1,Pruebas);
FrecMuestreo          = zeros (1,Pruebas);

MuestrasSenial_1      = zeros (1,Pruebas);
FrecSenialResultado_1 = zeros (1,Pruebas);
ErrorEnFrec_1         = zeros (1,Pruebas);

MuestrasSenial_N      = zeros (1,Pruebas);
Ciclos_N              = zeros (1,Pruebas);
MuestrasPorCiclo_N    = zeros (1,Pruebas);
FrecSenialResultado_N = zeros (1,Pruebas);
ErrorEnFrec_N         = zeros (1,Pruebas);
ErrorDeFase_N         = zeros (1,Pruebas);

//------------------------------------------------------------------------------
// Funciones privadas

function resultado = calcular_mcd(a, b)
    while b ~= 0
        temp = b;
        b = modulo(a, b);
        a = temp;
    end
    resultado = a;
endfunction

//------------------------------------------------------------------------------
// Simulación muestra a muestra

disp('Frecuencia mínima posible (Hz):', FrecBase / (2^16) / MuestrasSenialMax)

for i=1:1:Pruebas
    
    // -------------------------------------------------------------------------
    // Primero determinamos frecuencia en escala logaritmica y muestreo ideal
    
    FrecSenialObjetivo (i) = 1*10^ef(i);
    FrecMuestreoDeseada    = FrecSenialObjetivo (i) * MuestrasSenialMax;
    
    // -------------------------------------------------------------------------
    // Calculamos el Divisor B y frec. de muestreo para esa frecuencia de senial
    
    DivisorB (i) = ceil( FrecBase / FrecMuestreoDeseada ); 
        // Divisor entero, busco el más grande porque la frecuencia puede subir
        // si bajamos la cantidad de muestras por período.
    DivisorB (i) = max ( DivisorB (i), DivisorMinimoB);
    DivisorB (i) = min ( DivisorB (i), 2^16);  
        // DivisorB tiene límites superior e inferior.
    FrecMuestreo (i) = FrecBase / DivisorB (i);

    // -------------------------------------------------------------------------
    // Comenzamos con calculos suponiendo un sólo período en la señal muestra
    
    MuestrasSenial_1 (i) = round( FrecMuestreo (i) / FrecSenialObjetivo (i) );
        // Busco el entero más próximo a una cantidad no entera de muestras.
    MuestrasSenial_1 (i) = min ( MuestrasSenial_1 (i), MuestrasSenialMax );
    FrecSenialResultado_1(i) = FrecMuestreo (i) /  MuestrasSenial_1 (i);
    ErrorEnFrec_1 (i) = abs (FrecSenialObjetivo (i) - FrecSenialResultado_1 (i) ) / FrecSenialObjetivo (i) *1e6;
    
    // -------------------------------------------------------------------------
    // Ahora permitimos cantidad fracional de muestras por período
    
    Muestras_F =  FrecMuestreo (i) / FrecSenialObjetivo (i);
    // disp(Muestras_F);
    Ciclos_N (i) = floor ( MuestrasSenialMax / Muestras_F );
    Ciclos_N (i) = max ( 1, Ciclos_N (i) );
    MuestrasSenial_N (i) = round ( Ciclos_N (i) * Muestras_F );
    MuestrasSenial_N (i) = min (MuestrasSenial_N (i), MuestrasSenialMax);
    FrecSenialResultado_N (i) = Ciclos_N (i) * FrecMuestreo (i) /  MuestrasSenial_N (i);
    
    MCD = calcular_mcd ( Ciclos_N (i), MuestrasSenial_N (i) );
    if (MCD > 1) then
        //disp(MCD);
        //Ciclos_N (i)         = Ciclos_N (i) / MCD;
        //MuestrasSenial_N (i) = MuestrasSenial_N (i) / MCD;
    end;
    MuestrasPorCiclo_N (i) = MuestrasSenial_N (i) /  Ciclos_N (i);
    ErrorEnFrec_N (i) = abs (FrecSenialObjetivo (i) - FrecSenialResultado_N (i) ) / FrecSenialObjetivo (i) *1e6;
    ErrorDeFase_N (i) = 180 / MuestrasPorCiclo_N (i);
    
end;

//------------------------------------------------------------------------------
// Graficamos

close();
close();
close();
figure();
semilogx (FrecSenialObjetivo, ErrorEnFrec_1, "b");
semilogx (FrecSenialObjetivo, ErrorEnFrec_N, "r");
title('Error por discretización en la selección de frecuencia (ppm)')

F0 = FrecSenialObjetivo(1);
F8 = FrecSenialObjetivo(Pruebas);
ha=gca()
ha.data_bounds = [F0, 0; F8, 1000];
//xgrid();
ErrorEnFrecMax = max(ErrorEnFrec_N);
plot ( [F0, F8], [ErrorEnFrecMax, ErrorEnFrecMax], "r-." )

figure();
semilogx (FrecSenialObjetivo, MuestrasSenial_1 );
semilogx (FrecSenialObjetivo, MuestrasSenial_N, "r" );
title('Muestras utilizadas para la señal')

figure ();
semilogx (FrecSenialObjetivo, ErrorDeFase_N );
//semilogx (FrecSenialObjetivo, MuestrasPorCiclo_N, "k" );
title('Ruido de fase debido a la discretización de muestras (º)')

disp('Error máximo con período entero (ppm):', max(ErrorEnFrec_1));
disp('Error máximo con período fraccional (ppm):', max(ErrorEnFrec_N));
