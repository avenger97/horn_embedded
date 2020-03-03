#include <avr/io.h>
#include<stdint.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include"header.h"

struct{
volatile unsigned int FLAG_ISR1:1;
volatile unsigned int FLAG_ISR0:1;
}FLAG_BIT;
 uint8_t PIN_READ=0x00;
 uint16_t ADC_value;
 uint8_t PWM=128;
 uint8_t pwm_flag=0;


int main(void)
{

 Bit_func();
 ADMUX&=0x00;
 ADMUX|=(1<<REFS0);
 ADCSRA|=(1<<ADEN);

 EICRA|=(1<<ISC11);
 EICRA&=~(1<<ISC10);// interrupt
  EICRA|=(1<<ISC01);
 EICRA&=~(1<<ISC00); // interrupt

 EIMSK|=(1<<INT1);
 EIMSK|=(1<<INT0);

 TCCR0A|=((1<<WGM01) | (1<<WGM00));
    TCCR0A|=(1<<COM0A1);
    TCCR0A|=(1<<COM0A0);

    TCNT0=0x00;
    OCR0A=PWM;
        TCCR0B|=((1<<CS00)|(1<<CS02));
        TCCR0B&=~(1<<CS01);
        sei();



    while(1){
        PIN_READ=PIND;
        if(PIN_READ & (1<<PD4)){// sw1 high - horn mode
                ADCSRA|=(1<<ADSC);
                while(ADCSRA & (1<<ADSC));
                ADC_value=ADC;
                PIN_READ=PIND;
                if(ADC_value>=750){
                    if(PIN_READ & (1<<PD5)){
                        OCR0A=255;

                    }
                    else {
                        OCR0A=205;
                    }}
                else if(ADC_value <750){
                        PIN_READ=PIND;
                        if(PIN_READ & (1<<PD5)){
                            OCR0A=255;
                        }
                        else{
                               if(pwm_flag==1) {
                                    OCR0A=PWM;
                               }
                               else{
                                OCR0A=128;
                               }

                        }
                }
                }




        else {//sw1 low - calibration mode
                pwm_flag=1;

                if(FLAG_BIT.FLAG_ISR0==1){// pwm -5%

                OCR0A=OCR0A-13;
                FLAG_BIT.FLAG_ISR0=0;
                PWM=OCR0A;
            }
                else if (FLAG_BIT.FLAG_ISR1==1){// pwm +5%

                OCR0A=OCR0A+13;
                FLAG_BIT.FLAG_ISR1=0;
                PWM=OCR0A;


    }

        }

    }


    return 0;
}
ISR(INT1_vect){
                FLAG_BIT.FLAG_ISR1=1;
    }
ISR(INT0_vect){
                FLAG_BIT.FLAG_ISR0=1;
    }



