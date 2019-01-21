/*
 * chessavr.cpp
 *
 * Created: 09/08/2016 02:31:28
 * Author : Muhammed Zia Dawood
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif



#define sbi(port,bit)  (port) |= (1 << (bit))
#define cbi(port,bit)  (port) &= ~(1 << (bit))

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "Stepper.h"
#include "pins.h"
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <util/crc16.h>
#include <string.h>
#include "definitions.h"



extern "C" {

	#include "uart.h"
}


double onestep = 0.7275; //mm
double stepspermm= 1.37931;
int Speed = 70;



Stepper X_Stepper(stepsPerRevolution,X_A,X_A_bar,X_B,X_B_bar,X_Enable );
Stepper Y_Stepper(stepsPerRevolution,Y_A,Y_A_bar,Y_B,Y_B_bar,Y_Enable);

void move_to_origin();
void move_away_origin();
void move_to_last_pos();
void move_to_freeway();
void move_off_freeway();
void move_to_first_pos();
void go_to(int , int );
void move_piece(int start_x,int start_y,int end_x,int end_y);
void go_to_grave();
void servo_up();
void servo_down();
void next_grave_pos(int *x,int *y);
void kill(int x, int y);

void testpiece();
void wait_for_player(int * ,int * , int * , int*);


int cur_cord[2];

void getPostions(int (*a)[8]);
int arrayCompare(int (*a)[8],int (*b)[8], int *Xchange,int *Ychange);
void arrayCopy(int (*orig)[8],int (*final)[8]);
void char_arrayCopy(char (*orig)[8],char (*final)[8]);
 

char in_coms[in_coms_size]="";

void wait_for_coms();
int assesscoms();
 void  convert_to_chess_string(int old_x,int old_y,int new_x,int new_y, char arr[]);
 void convert_to_cord(int * ,int * , int * , int*);
 void convert_error_to_cord(int *,int *,int *,int *);
 void convert_help_to_cord(int *old_x,int *old_y,int *new_x,int *new_y) ;


char b_w[8][8] = {
	{'b','b','b','b','b','b','b','b'},
	{'b','b','b','b','b','b','b','b'},
	{'n','n','n','n','n','n','n','n'},
	{'n','n','n','n','n','n','n','n'},
	{'n','n','n','n','n','n','n','n'},
	{'n','n','n','n','n','n','n','n'},
	{'w','w','w','w','w','w','w','w'},
	{'w','w','w','w','w','w','w','w'}
} ;

int currentBoardState[8][8] = {   // for reed switches
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0}
	} ;
int oldBoardState[8][8];
int grave_state[8][2] ={
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0}	   
	   };

uint8_t tempArr[8]= {0b00000001,0b00000010,0b00000100,0b00001000,0b00010000,0b00100000,0b01000000,0b10000000};

const int stepsperblock=209;
int current_x_step = 0;
int curent_y_step = 0;   // position at "move_to_last_pos"

int y_steps_to_position[8][8] = {
		{7*stepsperblock,6*stepsperblock,5*stepsperblock,4*stepsperblock,3*stepsperblock,2*stepsperblock,1*stepsperblock,0},
		{7*stepsperblock,6*stepsperblock,5*stepsperblock,4*stepsperblock,3*stepsperblock,2*stepsperblock,1*stepsperblock,0},
		{7*stepsperblock,6*stepsperblock,5*stepsperblock,4*stepsperblock,3*stepsperblock,2*stepsperblock,1*stepsperblock,0},
		{7*stepsperblock,6*stepsperblock,5*stepsperblock,4*stepsperblock,3*stepsperblock,2*stepsperblock,1*stepsperblock,0},
		{7*stepsperblock,6*stepsperblock,5*stepsperblock,4*stepsperblock,3*stepsperblock,2*stepsperblock,1*stepsperblock,0},
		{7*stepsperblock,6*stepsperblock,5*stepsperblock,4*stepsperblock,3*stepsperblock,2*stepsperblock,1*stepsperblock,0},
		{7*stepsperblock,6*stepsperblock,5*stepsperblock,4*stepsperblock,3*stepsperblock,2*stepsperblock,1*stepsperblock,0},
		{7*stepsperblock,6*stepsperblock,5*stepsperblock,4*stepsperblock,3*stepsperblock,2*stepsperblock,1*stepsperblock,0}
	} ;
int x_steps_to_position[8][8] = {
			{7*stepsperblock,7*stepsperblock,7*stepsperblock,7*stepsperblock,7*stepsperblock,7*stepsperblock,7*stepsperblock,7*stepsperblock},
			{6*stepsperblock,6*stepsperblock,6*stepsperblock,6*stepsperblock,6*stepsperblock,6*stepsperblock,6*stepsperblock,6*stepsperblock},
			{5*stepsperblock,5*stepsperblock,5*stepsperblock,5*stepsperblock,5*stepsperblock,5*stepsperblock,5*stepsperblock,5*stepsperblock},
			{4*stepsperblock,4*stepsperblock,4*stepsperblock,4*stepsperblock,4*stepsperblock,4*stepsperblock,4*stepsperblock,4*stepsperblock},
			{3*stepsperblock,3*stepsperblock,3*stepsperblock,3*stepsperblock,3*stepsperblock,3*stepsperblock,3*stepsperblock,3*stepsperblock},
			{2*stepsperblock,2*stepsperblock,2*stepsperblock,2*stepsperblock,2*stepsperblock,2*stepsperblock,2*stepsperblock,2*stepsperblock},
			{1*stepsperblock,1*stepsperblock,1*stepsperblock,1*stepsperblock,1*stepsperblock,1*stepsperblock,1*stepsperblock,1*stepsperblock},
			{0,0,0,0,0,0,0,0}
		} ;	
int y_steps_to_grave[8][2] = {
		{9*stepsperblock,8*stepsperblock},
		{9*stepsperblock,8*stepsperblock},
		{9*stepsperblock,8*stepsperblock},
		{9*stepsperblock,8*stepsperblock},
		{9*stepsperblock,8*stepsperblock},
		{9*stepsperblock,8*stepsperblock},
		{9*stepsperblock,8*stepsperblock},
		{9*stepsperblock,8*stepsperblock}	
	} ;	
int x_steps_to_grave[8][2] = {
			{7*stepsperblock,7*stepsperblock},
			{6*stepsperblock,6*stepsperblock},
			{5*stepsperblock,5*stepsperblock},
			{4*stepsperblock,4*stepsperblock},
			{3*stepsperblock,3*stepsperblock},
			{2*stepsperblock,2*stepsperblock},
			{1*stepsperblock,1*stepsperblock},
			{0*stepsperblock,0*stepsperblock}
		} ;
	
	int Xchange1, Ychange1, XchangeOld1, YchangeOld1;// player piece movement detection
	int Xchange2, Ychange2, XchangeOld2, YchangeOld2;	
	
	
void init() {
	
	pinMode(Y_Max_Pin,INPUT);
	pinMode(Y_Min_Pin,INPUT);
	pinMode(X_Max_Pin,INPUT);
	pinMode(X_Min_Pin,INPUT);
	
	pinMode(0,INPUT);// set Row 1-8 as input
	pinMode(1,INPUT);
	pinMode(2,INPUT);
	pinMode(3,INPUT);
	pinMode(4,INPUT);
	pinMode(5,INPUT);
	pinMode(6,INPUT);
	pinMode(7,INPUT);
	
	pinMode(10,OUTPUT);  // set columns as input to read
	pinMode(11,OUTPUT);
	//pinMode(12,OUTPUT);
	pinMode(22,OUTPUT);
	pinMode(14,OUTPUT);
	pinMode(26,OUTPUT);
	pinMode(25,OUTPUT);
	pinMode(24,OUTPUT);
	pinMode(30,OUTPUT); //special case , multi plexing pin for motor out and scaning !
	
	//_delay_ms(100);
	
	//void initTimer0(void);
	TCCR1A|=(1<<COM1A1)|(1<<WGM11);//|(1<<COM1B1);        //NON Inverted PWM
	TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); //PRESCALER=64 MODE 14(FAST PWM)

	ICR1=4999;  //fPWM=50Hz (Period = 20ms Standard).

	DDRD|=(1<<PD5);//(1<<PD4)|(1<<PD5);
   
    servo_down();

	sbi(DDRD, 4); // buzzer output on beep at startup
	sbi(PORTD,4);
	_delay_ms(300);
	cbi(PORTD,4);
	
	
	UARTinit();
    sei();	
	_delay_ms(800);
	//UARTsendString("coms up");
	_delay_ms(800);// VITAL!!
	_delay_ms(1000);
	
	move_to_first_pos();//A8  always at startup!

	
}





char turn = 'W';
int black_castle=1;// Gives ability to castle at 1st till king is moved
int count=10;
int main(void)
{ 
	
	init();

char temp_b_w[8][8];

while(1)
{
	
    //_delay_ms(100);
    char enginestring[]="";
    turn='W';
		
		int old__x =0,old__y=0,new__x=0,new__y=0;  // for player move
	    int pc_old_x=0,pc_old_y=0,pc_new_x=0,pc_new_y=0;// for pc move
	    char_arrayCopy(b_w,temp_b_w);// store copy 
	
	
	wait_for_player(&old__x,&old__y,&new__x,&new__y);
     
	convert_to_chess_string(old__x,old__y,new__x,new__y,enginestring);
	UARTsendString(enginestring);//send move
	 
	  _delay_ms(100);
	  
	  
	 
	  wait_for_coms();


	   _delay_ms(100);
	   
	   
	  int check_state = assesscoms();  // checks for error/gamestates returned by engine returns 0 if no error
	 
	 
	 
	  if (check_state==1)  // error 1 reported , invalid move
	  {
		 
		  
		  sbi(PORTD,4);  // sound buzzer
		  
		  int old_x_e,old_y_e,new_x_e,new_y_e;  // for player move
		  int pc_old_x_e,pc_old_y_e,pc_new_x_e,pc_new_y_e;// for pc error move
		  
		  convert_error_to_cord(&pc_old_x_e,&pc_old_y_e,&pc_new_x_e,&pc_new_y_e); // convert pc response to chess cord and place in variables
		  
		  wait_for_player(&old_x_e,&old_y_e,&new_x_e,&new_y_e);  // waits for player to undo move
		  
		  while (old_x_e!=pc_new_x_e && old_y_e!=pc_new_y_e && new_x_e!=pc_old_x_e && new_y_e!=pc_old_y_e) // verify that the correction of move is legitimate
		  {  
			  	  
		  }
		  
		  cbi(PORTD,4);
		  char_arrayCopy(temp_b_w,b_w); // restore board
		   turn='B';
		  //break;
	  } 
	  
	  
	  else if(check_state==0)  // no error reported extract pc move from coms and make it
	  { 
		  _delay_ms(100); 
		 turn='B'; 
		 
		 convert_to_cord(&pc_old_x,&pc_old_y,&pc_new_x,&pc_new_y);
		 
		 if (pc_old_x==0 && pc_old_y==0 && pc_new_x==0 && pc_new_y==0)
		 {
           wait_for_coms();
		   convert_to_cord(&pc_old_x,&pc_old_y,&pc_new_x,&pc_new_y);
		   		sbi(PORTD,4);
		   		_delay_ms(300);
		   		cbi(PORTD,4);
		   		_delay_ms(300);
		   		sbi(PORTD,4);
		   		_delay_ms(300);
		   		cbi(PORTD,4);
				sbi(PORTD,4);
				_delay_ms(300);
				cbi(PORTD,4);
				_delay_ms(300);
				sbi(PORTD,4);
				_delay_ms(300);
				cbi(PORTD,4);
}
		 
		 
         move_piece(pc_old_x,pc_old_y,pc_new_x,pc_new_y); 
	 
	       if (in_coms[8]=='c' && in_coms[9]=='h')  //if white is checked alert with two beeps
	       {
			   	sbi(PORTD,4);
			   	_delay_ms(300);
			   	cbi(PORTD,4);
				_delay_ms(300);
				sbi(PORTD,4);
				_delay_ms(300);
				cbi(PORTD,4);   
	       }
	 
	  }


	  else if(check_state==4)  // black won
	  {
		  _delay_ms(100);
		  turn='B';
		  convert_to_cord(&pc_old_x,&pc_old_y,&pc_new_x,&pc_new_y);
		  move_piece(pc_old_x,pc_old_y,pc_new_x,pc_new_y);
		  
		  _delay_ms(100);
		  
		  while (1)
		  {
			  sbi(PORTD,4);
			  _delay_ms(500);
			   cbi(PORTD,4);
			  _delay_ms(500);
		  }
		   
		  
		  
	  }
	 
}

}

  
  






// bellow are for move routines
void servo_up(void)
{
         OCR1A= servo_up_value;
		 _delay_ms(500);
}
void servo_down(void)
{
	OCR1A= servo_down_value;
			 _delay_ms(500);

}

void move_to_origin()
{
	while(!bit_is_set(X_Min_Pin_Avr,X_Min_Pin_No))
	{
		X_Stepper.step(-1);
	}

	while(!bit_is_set(Y_Min_Pin_Avr,Y_Min_Pin_No))
	{
		Y_Stepper.step(-1);
	}
	
}

void move_away_origin()
{
	while(!bit_is_set(X_Max_Pin_Avr,X_Max_Pin_No))
	{
		X_Stepper.step(1);
	}
_delay_ms(100);
	while(!bit_is_set(Y_Max_Pin_Avr,Y_Max_Pin_No))
	{
		Y_Stepper.step(1);
	}
	
}
 
 
 void move_to_last_pos()
 {
   move_to_origin();
   cur_cord[0]=7; //row
   cur_cord[1]=7;//column
   
    X_Stepper.step(170);
    Y_Stepper.step(110);
	
	 current_x_step = 0;
	 curent_y_step = 0;
 
 }
 
  void move_to_first_pos() //A8
  {
	  move_away_origin();
	  cur_cord[0]=0; //row
	  cur_cord[1]=0;//column
	  
	  X_Stepper.step(-105);
	  Y_Stepper.step(-500);
	  
	  current_x_step = x_steps_to_position[0][0] ;
	  curent_y_step = y_steps_to_position[0][0];
	  
	  digitalWrite(22,LOW);// coz of the multiplexing, using motor as reed switch inputs also
	  digitalWrite(30,LOW);
	  
  }
 
 
 
 void move_to_freeway()
 {
  _delay_ms(100);
  X_Stepper.step(stepsperblock/2);
    _delay_ms(100);
  Y_Stepper.step(stepsperblock/2);
    _delay_ms(100);
	
		  digitalWrite(22,LOW);// coz of the multiplexing, using motor as reed switch inputs also
		  digitalWrite(30,LOW);
 }
 
 void move_off_freeway()
  {
	   _delay_ms(100);
	  X_Stepper.step(-stepsperblock/2);
	   _delay_ms(100);
	  Y_Stepper.step(-stepsperblock/2);
	   _delay_ms(100);
	   
	   	  digitalWrite(22,LOW);// coz of the multiplexing, using motor as reed switch inputs also
	   	  digitalWrite(30,LOW);
  }
  
  
  void move_piece(int start_x,int start_y,int end_x,int end_y)
  {
	  
	  if (turn=='W')
	  {	 
		if (b_w[end_x][end_y]=='b')
		{
			kill(end_x,end_y);
		}
		
		b_w[start_x][start_y]='n';
	    b_w[end_x][end_y]='w';
	  }
	  
	 else if (turn=='B')
	 {

		 if (b_w[end_x][end_y]=='w')
		 {
			 kill(end_x,end_y);
		 }
		  b_w[start_x][start_y]='n';
		  b_w[end_x][end_y]='b';		 
	 } 
	 
	 
	 
	 
  go_to(start_x,start_y);
  servo_up();
  _delay_ms(500);
  go_to(end_x,end_y);
  servo_down();
  
 ////////////////////////////This only takes care of Black side castling/////////
		 if (start_x==0 && start_y==4&&end_x==0&&end_y==6&& black_castle==1)// black king side castling
		{
		  move_piece(0,7,0,5);   //moves h8 castle
		  
		}
  
		if (start_x==0 && start_y==4&&end_x==0&&end_y==2&&black_castle==1)// black Queen side castling
		{
			move_piece(0,0,0,3);   //moves h8 castle
		}
		
		if (start_x==0 && start_y==4&&black_castle==1)
		{
			black_castle=0;
		}
////////////////////////////////////////////////////////////////
		
 
  
  //go_to(0,0);
  //digitalWrite(22,LOW);
//  digitalWrite(30,LOW);  
  _delay_ms(20);
 move_to_first_pos();
 
  
  }
 


void next_grave_pos(int *x,int *y)
{
	int i =0,j=0;
	int flag = 0;
	for(i=0;i<8;i++)
	{
		for(j=0;j<2;j++)
			if(grave_state[i][j]==0)
			{
				*x = i;
				*y = j;
				grave_state[i][j]=1;
				flag = 1;
				break;
			}
			if(flag == 1)
				break;
	}
}

 
 void go_to(int x, int y)
 {
	 
	          move_to_freeway();
			  
       		  X_Stepper.step(x_steps_to_position[x][y]-current_x_step);
       		  current_x_step= x_steps_to_position[x][y];
       		  
       		  Y_Stepper.step(y_steps_to_position[x][y]-curent_y_step);
       		  curent_y_step=y_steps_to_position[x][y];
				 
			  move_off_freeway();
 }
 
  void go_to_grave()
  {
	  int x,y;
	  next_grave_pos(&x,&y);
	  
	  move_to_freeway();
	  
	  X_Stepper.step(x_steps_to_grave[x][y]-current_x_step);
	  current_x_step= x_steps_to_grave[x][y];
	  
	  Y_Stepper.step(y_steps_to_grave[x][y]-curent_y_step);
	  curent_y_step=y_steps_to_grave[x][y];
	  
	  move_off_freeway();
	  
  }
  
  void kill(int x, int y)
  {
    go_to(x,y);
	servo_up();
	go_to_grave();
	servo_down();
	//go_to(0,0);
	b_w[x][y]='n';
	  
  }
 
 // Bellow are sense routines
 
 void getPostions(int (*a)[8])
 {
	 
	 
	 digitalWrite(10,HIGH);

	 for(int i=0;i<=7;i++)
	 {
		 a[7][i] = (PINB&tempArr[i])?1:0;
	 }

	 digitalWrite(10,LOW);
	 
	 digitalWrite(11,HIGH);

	 for(int i=0;i<=7;i++)
	 {
		 a[6][i] = (PINB&tempArr[i])?1:0;
		 
	 }
	 
	 digitalWrite(11,LOW);
	 
	 
	 digitalWrite(22,HIGH);

	 for(int i=0;i<=7;i++)
	 {
		 a[5][i] = (PINB&tempArr[i])?1:0;
		 
	 }
	 digitalWrite(22,LOW);
	 
	 digitalWrite(14,HIGH);

	 for(int i=0;i<=7;i++)
	 {
		 a[4][i] = (PINB&tempArr[i])?1:0;
		 
	 }
	 digitalWrite(14,LOW);
	 
	 
	 digitalWrite(26,HIGH);

	 for(int i=0;i<=7;i++)
	 {
		 a[3][i] = (PINB&tempArr[i])?1:0;
		 
	 }
	 digitalWrite(26,LOW);
	 
	 digitalWrite(25,HIGH);

	 for(int i=0;i<=7;i++)
	 {
		 a[2][i] = (PINB&tempArr[i])?1:0;
		 
	 }
	 digitalWrite(25,LOW);
	 
	 digitalWrite(24,HIGH);

	 for(int i=0;i<=7;i++)
	 {
		 a[1][i] = (PINB&tempArr[i])?1:0;
		 
	 }
	 digitalWrite(24,LOW);

	 digitalWrite(30,HIGH);

	 for(int i=0;i<=7;i++)
	 {
		 a[0][i] = (PINB&tempArr[i])?1:0;
		 
	 }
	 digitalWrite(30,LOW);

 }

 int arrayCompare(int (*a)[8],int (*b)[8], int *Xchange,int *Ychange)
 {
	 int i,j;
	 for(i=0;i<8;i++)
	 {
		 for(j=0;j<8;j++) /* Cycle through 8 rows and 8 cols */
		 {
			 if(a[i][j]!=b[i][j]) /* Compare Each element */
			 {

				 *Xchange = i;
				 *Ychange = j;
				 return 1; 		/* Change detected */
			 }
		 }
	 }
	 return 0; 		/* No Change detected */
 }

 void arrayCopy(int (*orig)[8],int (*final)[8])
 {
	 unsigned char i,j;
	 for(i=0;i<8;i++)
	 {
		 for(j=0;j<8;j++)
		 {
			 final[i][j]=orig[i][j];
		 }
	 }
 }
  void char_arrayCopy(char (*orig)[8],char (*final)[8])
  {
	  unsigned char i,j;
	  for(i=0;i<8;i++)
	  {
		  for(j=0;j<8;j++)
		  {
			  final[i][j]=orig[i][j];
		  }
	  }
  }
 

	
int undo_attack_x=100;
int undo_attack_y=100;	
			  
 void wait_for_player(int *oldx,int *oldy,int *newx, int *newy)
 {  
	int exit_flag=1;
	int exit_flag_2=1;// for user help request interupt
	
 	while(exit_flag) // Only leave loop when we are certain of the movement
 	{
		 
		 
		 
		 
		 
	 		Xchange1=10, Ychange1=10,XchangeOld1=10, YchangeOld1=10;
	 		Xchange2=10, Ychange2=10,XchangeOld2=10, YchangeOld2=10;
           
	 		count = 10;

	 		// Step 1 - Detect Piece That Moved
	 		//_delay_ms(100);
	 		getPostions(currentBoardState);
	 		_delay_ms(100); 			 // Dumps current state into currentBoardState
	 		arrayCopy(currentBoardState, oldBoardState); // Make a copy so we can compare later


	 		while(1)
			  {
				  
				  if (bit_is_set(Y_Min_Pin_Avr,Y_Min_Pin_No)) // allows for suggested move add check for suggested move here
				  {
					  exit_flag_2=0;
					  exit_flag=0;
					  int pc_help_old_x ,pc_help_old_y,pc_help_new_x,pc_help_new_y;
					  
					  convert_help_to_cord(&pc_help_old_x,&pc_help_old_y,&pc_help_new_x,&pc_help_new_y);
					  move_piece(pc_help_old_x,pc_help_old_y,pc_help_new_x,pc_help_new_y);
					  *oldx=pc_help_old_x;
					  *oldy=pc_help_old_y;
					  *newx=pc_help_new_x;
					  *newy=pc_help_new_y;
					  
					  
					  break;
					  
					  
				  }
				  
				  
		 			_delay_ms(10);
		 			getPostions(currentBoardState);		// Dumps current state into currentBoardState
					 
					 						if (currentBoardState[undo_attack_x][undo_attack_y]==1)
					 						{
						 						b_w[undo_attack_x][undo_attack_y]='b';
												 undo_attack_x=100;
												 undo_attack_y=100;

					 						}
					 
		 			if(arrayCompare(currentBoardState, oldBoardState,&Xchange1,&Ychange1)) // See proto for full explanation
		 			{
			 			// Move has been detected.
			 			count--; // Reduce counter - de-bouncing


			 			if((Xchange1!=XchangeOld1) ||  (Ychange1!=YchangeOld1)  ) // If a different piece is now detected, restart
			 			count = 10;

			 			//If the change has been the same for the last few compares then trigger
			 			if((count==0) && (Xchange1==XchangeOld1) && (Ychange1==YchangeOld1)&&currentBoardState[XchangeOld1][YchangeOld1]==0)
			 			{    

							 _delay_ms(100);
							count = 10;  
							
								  if (b_w[XchangeOld1][YchangeOld1]=='b') // a black piece ;lifted on white turn ie being taken
								  {

									  b_w[XchangeOld1][YchangeOld1]='n';
								  
									  undo_attack_x=XchangeOld1;
									  undo_attack_y=YchangeOld1;
								  
								  
									  return(wait_for_player(&*oldx,&*oldy,&*newx,&*newy));
								  
								  
								  
								  }
							  else if (b_w[XchangeOld1][YchangeOld1]=='w')
							  {
								  //UARTsendString("White piece being moved");
								  //_delay_ms(100);
								  b_w[XchangeOld1][YchangeOld1]='n';
								  
							  }				 	
				 			
				 			break;
			 			}

			 			XchangeOld1 = Xchange1;
			 			YchangeOld1 = Ychange1;
						 *oldx = XchangeOld1;
						 *oldy = Ychange1;
		 			}
	 		}

	 		arrayCopy(currentBoardState, oldBoardState); //make a copy so we can compare later
	 		count =10;
	 		//_delay_ms(100);
	 		//Step 2 - Detect Where That Piece Moved To  ********* ALMOST A COMPLETE-COPY FROM ABOVE
	 	
	 	
	 		while(exit_flag_2==1) {
		 		_delay_ms(10);
		 		getPostions(currentBoardState);
		 		if(arrayCompare(currentBoardState, oldBoardState,&Xchange2,&Ychange2))
		 		{
			 		//Move has been detected.
			 		count--; //Reduce counter - de-bouncing

			 		if((Xchange2!=XchangeOld2) ||  (Ychange2!=YchangeOld2)  )
			 		count = 10; //If an error has been detected then start the counter over again

			 		//If the change has been the same for the last few compares then trigger
			 		if((count==0) && (Xchange2==XchangeOld2) && (Ychange2==YchangeOld2)&&currentBoardState[XchangeOld2][YchangeOld2]==1&&currentBoardState[XchangeOld1][YchangeOld1]==0)
			 		{
				 		//char x = (XchangeOld2)+ '0';
				 		//char y = (YchangeOld2)+ '0';
						 b_w[XchangeOld2][YchangeOld2]='w';
				 		//UARTsendString("\n piece placed at:(");
				 		//UARTputChar(x);
				 		//UARTsendString(",");
				 		//UARTputChar(y);
				 		//UARTsendString(")");
						 _delay_ms(100);
				 		count = 10;
						exit_flag = 0;
						//return;
				 		break;
			 		}
			 	

			 		XchangeOld2 = Xchange2;
			 		YchangeOld2 = Ychange2;
					*newx=XchangeOld2;
					*newy=YchangeOld2; 
			 	

			 	
		 		}
	 		}
			 

 	}
 }	  
 
 
 
 //COMS BELLOW 
 void wait_for_coms()
 {
	 
	 for (int i=0; i<in_coms_size ;i++)
	 {
		 in_coms[i]=NULL;
	 }
	 
	 int new_or_old =0; //means old
	 
	 while (new_or_old==0)
	 {
		 new_or_old = getString(in_coms); // returns 1 if new string inserted and this is done to arr
	 }

	 if(new_or_old)
	 {   
		//UARTsendString("\n micro recieved new string:");
		//UARTsendString(in_coms);
		//UARTsendString("\n");
	 }

 }
 
int assesscoms()
 {
	 if (in_coms[0]=='e'&&in_coms[1]=='r'&&in_coms[2]=='r'&&in_coms[5]=='1') // if error 1 , invalid move
	 {	 
		 return 1; // return error code 1
	 }
	 
	 
	 
	else if (in_coms[0]=='e'&&in_coms[1]=='r'&&in_coms[2]=='r'&& in_coms[5]=='3') // if error 3 , invalid move
	{ 
		return 3; // return error code 1
	} 
	 
	 
	 
	 
	else if (in_coms[4]=='B'&&in_coms[5]=='L') // black won
	{
		return 4;
	}	 
	 
	 
	 
	 
	 else // no issue, next move in string , then sugested move , then state checked/unchecked
	 {

		 return 0; // no error
	 }
 }
 
 
 
// conversion functions bellow 
void  convert_to_chess_string(int old_x,int old_y,int new_x,int new_y, char arr[])
 {
	arr[0]='m';
 // For position 1 , eg PA6B3  -----> a6 to b3 esssentially
		if (old_y==0)
		{
			arr[1]='a';
		}
 		else if (old_y==1)
 		{
	 		arr[1]='b';
 		}
   		else if (old_y==2)
   		{
	   		arr[1]='c';
   		}
 		else if (old_y==3)
 		{
	 		arr[1]='d';
 		}
 		else if (old_y==4)
 		{
	 		arr[1]='e';
 		}
 		else if (old_y==5)
 		{
	 		arr[1]='f';
 		}
 		else if (old_y==6)
 		{
	 		arr[1]='g';
 		}
 		else if (old_y==7)
 		{
	 		arr[1]='h';
 		}
		 
		 
	//position 3	 
		if (new_y==0)
		{
			arr[3]='a';
		}
		else if (new_y==1)
		{
			arr[3]='b';
		}
		else if (new_y==2)
		{
			arr[3]='c';
		}
		else if (new_y==3)
		{
			arr[3]='d';
		}
		else if (new_y==4)
		{
			arr[3]='e';
		}
		else if (new_y==5)
		{
			arr[3]='f';
		}
		else if (new_y==6)
		{
			arr[3]='g';
		}
		else if (new_y==7)
		{
			arr[3]='h';
		}		 
 // position 2
 
 		if (old_x==0)
 		{
	 		arr[2]='8';
 		}
 		else if (old_x==1)
 		{
	 		arr[2]='7';
 		}
 		else if (old_x==2)
 		{
	 		arr[2]='6';
 		}
 		else if (old_x==3)
 		{
	 		arr[2]='5';
 		}
 		else if (old_x==4)
 		{
	 		arr[2]='4';
 		}
 		else if (old_x==5)
 		{
	 		arr[2]='3';
 		}
 		else if (old_x==6)
 		{
	 		arr[2]='2';
 		}
 		else if (old_x==7)
 		{
	 		arr[2]='1';
 		}
 
 
  		if (new_x==0)
  		{
	  		arr[4]='8';
  		}
  		else if (new_x==1)
  		{
	  		arr[4]='7';
  		}
  		else if (new_x==2)
  		{
	  		arr[4]='6';
  		}
  		else if (new_x==3)
  		{
	  		arr[4]='5';
  		}
  		else if (new_x==4)
  		{
	  		arr[4]='4';
  		}
  		else if (new_x==5)
  		{
	  		arr[4]='3';
  		}
  		else if (new_x==6)
  		{
	  		arr[4]='2';
  		}
  		else if (new_x==7)
  		{
	  		arr[4]='1';
  		}
 
 arr[5]='\n';
 }
 


 
 
 void convert_to_cord(int *old_x,int *old_y,int *new_x,int *new_y) // must be called after wait_for_coms,places pc move into co_ords
 {
	 
	 // pos 0     'A'6B3
     if (in_coms[0]=='a')
	 {
	   *old_y=0;
	 }
	else if (in_coms[0]=='b')
	{
		*old_y=1;
	}
	else if (in_coms[0]=='c')
	{
		*old_y=2;
	}
	else if (in_coms[0]=='d')
	{
		*old_y=3;
	}
	else if (in_coms[0]=='e')
	{
		*old_y=4;
	}	 
	else if (in_coms[0]=='f')
	{
		*old_y=5;
	}
	else if (in_coms[0]=='g')
	{
		*old_y=6;
	}
	else if (in_coms[0]=='h')
	{
		*old_y=7;
	}
 
	
// pos 2

   if (in_coms[2]=='a')
   {
	   *new_y=0;
   }
   else if (in_coms[2]=='b')
   {
	   *new_y=1;
   }
   else if (in_coms[2]=='c')
   {
	   *new_y=2;
   }
   else if (in_coms[2]=='d')
   {
	   *new_y=3;
   }
   else if (in_coms[2]=='e')
   {
	   *new_y=4;
   }
   else if (in_coms[2]=='f')
   {
	   *new_y=5;
   }
   else if (in_coms[2]=='g')
   {
	   *new_y=6;
   }
   else if (in_coms[2]=='h')
   {
	   *new_y=7;
   }

// pos 1
     if (in_coms[1]=='1')
     {
	     *old_x=7;
     }
     else if (in_coms[1]=='2')
     {
	     *old_x=6;
     }
     else if (in_coms[1]=='3')
     {
	     *old_x=5;
     }
     else if (in_coms[1]=='4')
     {
	     *old_x=4;
     }
     else if (in_coms[1]=='5')
     {
	     *old_x=3;
     }
     else if (in_coms[1]=='6')
     {
	     *old_x=2;
     }
     else if (in_coms[1]=='7')
     {
	     *old_x=1;
     }
     else if (in_coms[1]=='8')
     {
	     *old_x=0;
     }

	// pos 3
	if (in_coms[3]=='1')
	{
		*new_x=7;
	}
	else if (in_coms[3]=='2')
	{
		*new_x=6;
	}
	else if (in_coms[3]=='3')
	{
		*new_x=5;
	}
	else if (in_coms[3]=='4')
	{
		*new_x=4;
	}
	else if (in_coms[3]=='5')
	{
		*new_x=3;
	}
	else if (in_coms[3]=='6')
	{
		*new_x=2;
	}
	else if (in_coms[3]=='7')
	{
		*new_x=1;
	}
	else if (in_coms[3]=='8')
	{
		*new_x=0;
	}

 }
 
 void convert_error_to_cord(int *old_x,int *old_y,int *new_x,int *new_y) // must be called after wait_for_coms,places pc move into co_ords
 {
	 
	 // pos 0     'A'6B3
	 if (in_coms[0+6]=='a')
	 {
		 *old_y=0;
	 }
	 else if (in_coms[0+6]=='b')
	 {
		 *old_y=1;
	 }
	 else if (in_coms[0+6]=='c')
	 {
		 *old_y=2;
	 }
	 else if (in_coms[0+6]=='d')
	 {
		 *old_y=3;
	 }
	 else if (in_coms[0+6]=='e')
	 {
		 *old_y=4;
	 }
	 else if (in_coms[0+6]=='f')
	 {
		 *old_y=5;
	 }
	 else if (in_coms[0+6]=='g')
	 {
		 *old_y=6;
	 }
	 else if (in_coms[0+6]=='h')
	 {
		 *old_y=7;
	 }
	 
	 
	 // pos 2

	 if (in_coms[2+6]=='a')
	 {
		 *new_y=0;
	 }
	 else if (in_coms[2+6]=='b')
	 {
		 *new_y=1;
	 }
	 else if (in_coms[2+6]=='c')
	 {
		 *new_y=2;
	 }
	 else if (in_coms[2+6]=='d')
	 {
		 *new_y=3;
	 }
	 else if (in_coms[2+6]=='e')
	 {
		 *new_y=4;
	 }
	 else if (in_coms[2+6]=='f')
	 {
		 *new_y=5;
	 }
	 else if (in_coms[2+6]=='g')
	 {
		 *new_y=6;
	 }
	 else if (in_coms[2+6]=='h')
	 {
		 *new_y=7;
	 }

	 // pos 1
	 if (in_coms[1+6]=='1')
	 {
		 *old_x=7;
	 }
	 else if (in_coms[1+6]=='2')
	 {
		 *old_x=6;
	 }
	 else if (in_coms[1+6]=='3')
	 {
		 *old_x=5;
	 }
	 else if (in_coms[1+6]=='4')
	 {
		 *old_x=4;
	 }
	 else if (in_coms[1+6]=='5')
	 {
		 *old_x=3;
	 }
	 else if (in_coms[1+6]=='6')
	 {
		 *old_x=2;
	 }
	 else if (in_coms[1+6]=='7')
	 {
		 *old_x=1;
	 }
	 else if (in_coms[1+6]=='8')
	 {
		 *old_x=0;
	 }

	 // pos 3
	 if (in_coms[3+6]=='1')
	 {
		 *new_x=7;
	 }
	 else if (in_coms[3+6]=='2')
	 {
		 *new_x=6;
	 }
	 else if (in_coms[3+6]=='3')
	 {
		 *new_x=5;
	 }
	 else if (in_coms[3+6]=='4')
	 {
		 *new_x=4;
	 }
	 else if (in_coms[3+6]=='5')
	 {
		 *new_x=3;
	 }
	 else if (in_coms[3+6]=='6')
	 {
		 *new_x=2;
	 }
	 else if (in_coms[3+6]=='7')
	 {
		 *new_x=1;
	 }
	 else if (in_coms[3+6]=='8')
	 {
		 *new_x=0;
	 }

 }
 
 void convert_help_to_cord(int *old_x,int *old_y,int *new_x,int *new_y) // must be called after wait_for_coms,places pc move into co_ords
 {
	 
	 // pos 0     'A'6B3
	 if (in_coms[4]=='a')
	 {
		 *old_y=0;
	 }
	 else if (in_coms[4]=='b')
	 {
		 *old_y=1;
	 }
	 else if (in_coms[4]=='c')
	 {
		 *old_y=2;
	 }
	 else if (in_coms[4]=='d')
	 {
		 *old_y=3;
	 }
	 else if (in_coms[4]=='e')
	 {
		 *old_y=4;
	 }
	 else if (in_coms[4]=='f')
	 {
		 *old_y=5;
	 }
	 else if (in_coms[4]=='g')
	 {
		 *old_y=6;
	 }
	 else if (in_coms[4]=='h')
	 {
		 *old_y=7;
	 }
	 
	 
	 // pos 2

	 if (in_coms[6]=='a')
	 {
		 *new_y=0;
	 }
	 else if (in_coms[6]=='b')
	 {
		 *new_y=1;
	 }
	 else if (in_coms[6]=='c')
	 {
		 *new_y=2;
	 }
	 else if (in_coms[6]=='d')
	 {
		 *new_y=3;
	 }
	 else if (in_coms[6]=='e')
	 {
		 *new_y=4;
	 }
	 else if (in_coms[6]=='f')
	 {
		 *new_y=5;
	 }
	 else if (in_coms[6]=='g')
	 {
		 *new_y=6;
	 }
	 else if (in_coms[6]=='h')
	 {
		 *new_y=7;
	 }

	 // pos 1
	 if (in_coms[5]=='1')
	 {
		 *old_x=7;
	 }
	 else if (in_coms[5]=='2')
	 {
		 *old_x=6;
	 }
	 else if (in_coms[5]=='3')
	 {
		 *old_x=5;
	 }
	 else if (in_coms[5]=='4')
	 {
		 *old_x=4;
	 }
	 else if (in_coms[5]=='5')
	 {
		 *old_x=3;
	 }
	 else if (in_coms[5]=='6')
	 {
		 *old_x=2;
	 }
	 else if (in_coms[5]=='7')
	 {
		 *old_x=1;
	 }
	 else if (in_coms[5]=='8')
	 {
		 *old_x=0;
	 }

	 // pos 3
	 if (in_coms[7]=='1')
	 {
		 *new_x=7;
	 }
	 else if (in_coms[7]=='2')
	 {
		 *new_x=6;
	 }
	 else if (in_coms[7]=='3')
	 {
		 *new_x=5;
	 }
	 else if (in_coms[7]=='4')
	 {
		 *new_x=4;
	 }
	 else if (in_coms[7]=='5')
	 {
		 *new_x=3;
	 }
	 else if (in_coms[7]=='6')
	 {
		 *new_x=2;
	 }
	 else if (in_coms[7]=='7')
	 {
		 *new_x=1;
	 }
	 else if (in_coms[7]=='8')
	 {
		 *new_x=0;
	 }

 }
 
 
 
 
	void testpiece()
	{
	   move_piece(0,3,7,7);
	   _delay_ms(80);
	   move_piece(7,7,3,3);
	}
