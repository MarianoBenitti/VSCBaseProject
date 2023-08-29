/*! \mainpage Ejercicio Titulo
 * \date 01/01/2023
 * \author Nombre
 * \section genDesc Descripcion general
 * [Complete aqui con su descripcion]
 *
 * \section desarrollos Observaciones generales
 * [Complete aqui con sus observaciones]
 *
 * \section changelog Registro de cambios
 *
 * |   Fecha    | Descripcion                                    |
 * |:----------:|:-----------------------------------------------|
 * | 01/01/2023 | Creacion del documento                         |
 *
 */



/* Includes ------------------------------------------------------------------*/
#include "mbed.h"
/* END Includes --------------------------------------------------------------*/


/* typedef -------------------------------------------------------------------*/
typedef enum {UP=0,FALLING,DOWN,RISSING}e_estadoBoton;
/* END typedef ---------------------------------------------------------------*/

/* define --------------------------------------------------------------------*/
#define INTERVALOLED 100
#define INTERVALOBOTON 40
#define INTERVALOHB 200
#define LIMITEAUX 0x0F 
#define LIMITE 0x07
/* END define ----------------------------------------------------------------*/

/* hardware configuration ----------------------------------------------------*/

DigitalOut LED(PC_13);
DigitalIn BOTON(PA_4);
DigitalOut LEDROJO(PA_5);
DigitalOut LEDAMARILLO(PA_6);
/* END hardware configuration ------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/

/* END Function prototypes ---------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/
Timer myTimer;
e_estadoBoton e_estadob1;
uint8_t cont=0;
uint8_t contAux=0;
uint32_t mask=0;
uint32_t tAntHB=0;//tiempo anterior del HEARTBEAT
uint32_t tAntB1=0;//tiempo anterior del boton
uint32_t tAntL1=0;//tiempo anterior del led
uint32_t tAntPul=0;//tiempo anterior de la pulsacion
uint32_t tPulsado=0;//tiempo de duracion del pulso
uint8_t modoPul=0;//si vale 0 es intermitente si vale 1 constante
/* END Global variables ------------------------------------------------------*/


/* Function prototypes user code----------------------------------------------*/

/* END Function prototypes user code------------------------------------------*/

int main()
{
/* Local variables -----------------------------------------------------------*/

/* END Local variables -------------------------------------------------------*/


/* User code -----------------------------------------------------------------*/
    myTimer.start();
    LEDROJO=1;
    LEDAMARILLO=1;
    while(1){
    	
        if(myTimer.read_ms()-tAntHB >= INTERVALOHB){
            tAntHB=myTimer.read_ms();
            LED = !LED;
        } 
           //MAQUINA DE ESTADO BOTON
            if(myTimer.read_ms()-tAntB1 >= INTERVALOBOTON){
                tAntB1=myTimer.read_ms();
            switch(e_estadob1){
                case UP: 
                        if(BOTON.read()){//si el boton se encuentra presionado se pasa a bajando
                         e_estadob1 = FALLING;   
                        }
                    break;
                case FALLING:
                        if(BOTON.read()){//si el boton sigue presionado se pasa a abajo(o pulsado)
                         e_estadob1 = DOWN;
                         cont++;
                         cont=cont & LIMITE;
                         mask=mask | (1<<((cont+1)*2));//el mas 1 es para que parpadee desde el comienzo
                         if(cont==0){
                            mask=0;
                         }
                         LEDAMARILLO=0;
                         //leemos el tiempo de la pulsacion
                         tAntPul=myTimer.read_ms(); 
                        }
                    break;
                case DOWN:
                        if(BOTON.read()==0){//si el boton ya no esta presionado se pasa a subiendo
                            e_estadob1= RISSING;
                            LEDAMARILLO=1;
                            if(myTimer.read_ms()-tAntPul>=1000){
                                modoPul=1;
                            }
                        }
                    break;
                case RISSING:
                        if(BOTON.read()==0){//si el boton sigue sin estar presionado se pasa a arriba
                            e_estadob1= UP;
                        }
                        tPulsado=myTimer.read_ms()-tAntPul;
                    break;
                default: e_estadob1=UP;
                    break;
            }
        }
            
                if(modoPul){//modo para larga pulsacion
                    LEDROJO=0;
                    if(myTimer.read_ms()-tAntL1>=tPulsado){
                        LEDROJO=1;
                        modoPul=0;
                    }
                }else{//modo para clave morse
                    if(myTimer.read_ms()-tAntL1 >= INTERVALOLED){
                        tAntL1=myTimer.read_ms();
                        LEDROJO= (~mask) & 1<<contAux;//comprobamos con la mascara si esta encendido o apagado
                        contAux=contAux & LIMITEAUX;//reiniciamos el contador
                        contAux++;
                    }
                }

    }

/* END User code -------------------------------------------------------------*/
}


