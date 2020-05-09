#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
	
int score=0;
int times_2_turns = 0;
bool nextRandom = false;
bool nextBonus = false;
volatile int pixel_buffer_start; // global variable

void clear_screen();
void wait_for_vsync();

void plot_pixel(int x, int y, short int line_color);
void swap(int *x,int *y);
void draw_line(int x0,int y0, int x1, int y1, short int colour);
void plot_box(int x_box, int y_box, int short bg_color);

void draw_triangle1(short int color);
void draw_triangle2(short int color);
void draw_triangle3(short int color);
void draw_triangle4(short int color);
void draw_block(int short colourTop,int short colourRight, int short colourBottom, int short colourLeft);
void drawCircle(int xc, int yc, int x, int y, int short colour);
void circleBres(int xc, int yc, int r, int short colour);
void draw_circle_filled(int xc, int yc, int r, int short colour);

void draw_score_digit(int x, int y,int num, int short color);	//x, y is the left top coordinate of the digit that you wanna draw
void draw_score(int short color);								//draw score
void draw_question_mark(int xc, int yc, int short bg_color);
void draw_times_2(int xc, int yc, int short color);				//draw_times2

void shifted_triangle_colours(short int *array);

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // initialize colours
	short int colors[4]={0xF800,0xFFE0,0x07E0,0x001F}; //getting the red, yellow,green and blue colours
	short int triangle_colours[4]={0xF800,0xFFE0,0x07E0,0x001F};
	//intialize random speeds
	int dy[6]={5,6,5,7,8,9};
	int dy_random_part=dy[rand()%6];
	int dy_total=0;
	//set up the hex display
	volatile int *hex_display3_0=0xFF200020;
	char seg7[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67,0x77,0x7C,0x39,0x5E,0x79,0x71};
	int score_100s=0;
	int score_10s=0;
	int score_1s=0;
	int temp_score=0;
	//just display initial score of 0
	
	//deciding which side of the screen the ball comes from 0 top,1 left,2 right
	int side_mode=0;

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
   wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	
	//get edge capture register
	volatile int *edge_capture_register = 0xFF20005C;
   
    while (1)
    {
    	nextRandom = false; //reset nextRandom first
	    nextBonus = false;  //reset nextBonus first
		if(score > 3){
			int temp = rand()%4;
			if(temp == 0)
				nextRandom = true;
			else if(temp == 1 && (times_2_turns == 0))
				nextBonus = true;
		}
				
 	  	clear_screen();
		//get a random ball colour
		int short random_colour=colors[rand()%4];
		//set random mode 0 top, 1 left, 2 right, where ball comes from 
		side_mode=rand()%3;
			//top mode
		if(side_mode==0){
		    while(dy_total<100){	
			// (POLLING) check if any push buttons were pressed
			if((*edge_capture_register)!=0){
				//shift the colours in the triangle
				shifted_triangle_colours(triangle_colours);
				
				//reset the edge capture register (any button pressed)
				*edge_capture_register=0b1111;
			}
			clear_screen();
			
			draw_score(0xf800);	//plot score on vga
			//check for bonus
    	    if(times_2_turns !=0)
       			draw_times_2(23, 20, 0xFFE0);
			
		 	//draw all shapes (maybe put score under here)
			draw_block(triangle_colours[0],triangle_colours[1],triangle_colours[2],triangle_colours[3]);
			
			draw_circle_filled(160,10+dy_total,10,random_colour);
			dy_total=dy_total+dy_random_part;
		  
        	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
      		}
		}
	 	//ball from left side of the screen
		else if(side_mode==1){
		    while(dy_total<90){	
			// (POLLING) check if any push buttons were pressed
			if((*edge_capture_register)!=0){
				//shift the colours in the triangle
				shifted_triangle_colours(triangle_colours);
				
				//reset the edge capture register (any button pressed)
				*edge_capture_register=0b1111;
			}
			clear_screen();
			draw_score(0xf800);	//plot score on vga
			if(times_2_turns !=0)
       			draw_times_2(23, 20, 0xFFE0);
		
			//draw all shapes (maybe put score under here)
			draw_block(triangle_colours[0],triangle_colours[1],triangle_colours[2],triangle_colours[3]);
				
			draw_circle_filled(10+dy_total,170,10,random_colour);
			dy_total=dy_total+dy_random_part;
			  
        	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    	    }
		}
		//right side mode
		else if(side_mode==2){
		    while(dy_total<90){	
			// (POLLING) check if any push buttons were pressed
			if((*edge_capture_register)!=0){
				//shift the colours in the triangle
				shifted_triangle_colours(triangle_colours);
				
				//reset the edge capture register (any button pressed)
				*edge_capture_register=0b1111;
			}
			clear_screen();
			draw_score(0xf800);	//plot score on vga
			if(times_2_turns !=0)
       			draw_times_2(23, 20, 0xFFE0);
		
			//draw all the shapes  (maybe put drawing under here)  
			draw_block(triangle_colours[0],triangle_colours[1],triangle_colours[2],triangle_colours[3]);
				
			draw_circle_filled(310-dy_total,170,10,random_colour);
			dy_total=dy_total+dy_random_part;
			  
        	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    	    }
		}		
		//check if score should increase or reset and display on HEX
		//top increase
		if((side_mode==0)&&(triangle_colours[0]==random_colour)){
			if(times_2_turns != 0){
				score+=2;
				times_2_turns--;
			}
			else score++;
		}
		//left increase
		else if((side_mode==1)&&(triangle_colours[3]==random_colour)){
			if(times_2_turns != 0){
				score+=2;
				times_2_turns--;
			}
			else score++;
		}
		//right increase
		else if((side_mode==2)&&(triangle_colours[1]==random_colour)){
			if(times_2_turns != 0){
				score+=2;
				times_2_turns--;
			}
			else score++;
		}
		//no increase
		else{
			score=0;
			times_2_turns = 0;
		}
		//if did not fail and next square is randomized
		if(score !=0 && nextRandom){
			int n=0;
			while(n < 3){
				int temp1 = rand()%4;
				int temp2 = rand()%4;
				if(temp1 != temp2){
					short int temp = triangle_colours[temp1];
					triangle_colours[temp1] = triangle_colours[temp2];
					triangle_colours[temp2] = temp;
					n++;
				}
			}
		}
		temp_score=score;
	    score_100s=0;
		score_10s=0;
		score_1s=0;
		
		
		
		//if did not fail and there should be a bonus next
		if(score!=0 && nextBonus){
			random_colour=colors[rand()%4];	//get a random color
			dy_random_part=dy[rand()%6];	//get a rondom speed
			dy_total = 40;	//start from a closer place
			if(side_mode==0){
		    	while(dy_total<100){	
					// (POLLING) check if any push buttons were pressed
					if((*edge_capture_register)!=0){
						//shift the colours in the triangle
						shifted_triangle_colours(triangle_colours);
						
						//reset the edge capture register (any button pressed)
						*edge_capture_register=0b1111;
					}
					clear_screen();
				
					draw_score(0xf800);	//plot score on vga
					
				
		 			//draw all shapes (maybe put score under here)
					draw_block(triangle_colours[0],triangle_colours[1],triangle_colours[2],triangle_colours[3]);
					draw_times_2(160, 10+dy_total, random_colour);
				
					dy_total=dy_total+dy_random_part;
			  
        			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        			pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
      			}
			}
		 	//ball from left side of the screen
			else if(side_mode==1){
			    while(dy_total<90){	
					// (POLLING) check if any push buttons were pressed
					if((*edge_capture_register)!=0){
					//shift the colours in the triangle
					shifted_triangle_colours(triangle_colours);
					
					//reset the edge capture register (any button pressed)
					*edge_capture_register=0b1111;
					}
					clear_screen();
					draw_score(0xf800);	//plot score on vga
					if(times_2_turns !=0)
       					draw_times_2(23, 20, 0xFFE0);
			
					//draw all shapes (maybe put score under here)
					draw_block(triangle_colours[0],triangle_colours[1],triangle_colours[2],triangle_colours[3]);
					draw_times_2(10 + dy_total, 170, random_colour);
					
					dy_total=dy_total+dy_random_part;
				  
        			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	        		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    	   		}
			}
			//right side mode
			else if(side_mode==2){
		    	while(dy_total<90){	
					// (POLLING) check if any push buttons were pressed
					if((*edge_capture_register)!=0){
						//shift the colours in the triangle
						shifted_triangle_colours(triangle_colours);
					
						//reset the edge capture register (any button pressed)
						*edge_capture_register=0b1111;
					}
				clear_screen();
				draw_score(0xf800);	//plot score on vga
				if(times_2_turns !=0)
       				draw_times_2(23, 20, 0xFFE0);
			
				//draw all the shapes  (maybe put drawing under here)  
				draw_block(triangle_colours[0],triangle_colours[1],triangle_colours[2],triangle_colours[3]);
				draw_times_2(310 - dy_total, 170, random_colour);
				
				dy_total=dy_total+dy_random_part;
			  
        		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	        	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    	    	}
			}
			//check if to get the the bonux
			if((side_mode==0)&&(triangle_colours[0]==random_colour)){
				times_2_turns = 3;
			}
			//left increase
			else if((side_mode==1)&&(triangle_colours[3]==random_colour)){
				times_2_turns = 3;
			}
			//right increase
			else if ((side_mode==2)&&(triangle_colours[1]==random_colour)){
				times_2_turns = 3;
			}
			//else does not have the bonux
		}
		
		//update dy speed
		dy_random_part=dy[rand()%6];
		dy_total=0;
	
		//provide support up to a 999 points
		while(temp_score>=100){
			score_100s++;
			temp_score=temp_score-100;
		
		}
		while(temp_score>=10){
			score_10s++;
			temp_score=temp_score-10;
		}
		score_1s=temp_score;
		*hex_display3_0=seg7[score_1s]|seg7[score_10s]<<8|seg7[score_100s]<<16;
		
		
		
	}
}

// code for subroutines (not shown)
void wait_for_vsync(){
	
	volatile int *pixel_ctrl_ptr=0xFF203020; //contains the pointer to the buffer
	*pixel_ctrl_ptr=1;//writes a 1 to the buffer to set S to 1 to start sync process
	register int status;
	status =*(pixel_ctrl_ptr+3); 
	
	//wait for S to become 0
	while((status&0x01)!=0){
		status=*(pixel_ctrl_ptr+3);
	}
}

void clear_screen(){
	//interate through x and y and plot all black
	for (int x=0; x<=319; x++){
		for(int y=0; y<=239; y++){
			plot_pixel(x,y, 0x0000);   // this line is blue
		} 
	}
}

void plot_pixel(int x, int y, short int line_color)
{
	if(x >= 0 && (x<320) && (y>=0) && (y<240))
    	*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void draw_triangle1(short int color){
	
	for(int i=0; i<60; i++){
		draw_line(100+i,110+i,220-i,110+i,color);
		
	}

}
void draw_triangle2(short int color){
	
	for(int i=0; i<60; i++){
		draw_line(220-i,110+i,220-i,230-i,color);
		
	}

}
void draw_triangle3(short int color){
	
	for(int i=0; i<60; i++){
		draw_line(100+i,230-i,220-i,230-i,color);
		
	}

}
void draw_triangle4(short int color){
	
	for(int i=0; i<60; i++){
		draw_line(100+i,110+i,100+i,230-i,color);
		
	}

}

void draw_block(int short colourTop,int short colourRight, int short colourBottom, int short colourLeft){
	//colour each of the triangles with the appropriate colours
	draw_triangle1(colourTop);
	draw_triangle2(colourRight);
	draw_triangle3(colourBottom);
	draw_triangle4(colourLeft);
}

//utilize the circle drawing algorithim bresenham
void drawCircle(int xc, int yc, int x, int y,int short colour) 
{ 
    plot_pixel(xc+x, yc+y, colour); 
    plot_pixel(xc-x, yc+y, colour); 
    plot_pixel(xc+x, yc-y, colour); 
    plot_pixel(xc-x, yc-y, colour); 
    plot_pixel(xc+y, yc+x, colour); 
    plot_pixel(xc-y, yc+x, colour); 
    plot_pixel(xc+y, yc-x, colour); 
    plot_pixel(xc-y, yc-x, colour); 
} 
void circleBres(int xc, int yc, int r, int short colour) 
{ 
    int x = 0, y = r; 
    int d = 3 - 2 * r; 
    drawCircle(xc, yc, x, y,colour); 
    while (y >= x) 
    { 
        // for each pixel we will 
        // draw all eight pixels 
          
        x++; 
  
        // check for decision parameter 
        // and correspondingly  
        // update d, x, y 
        if (d > 0) 
        { 
            y--;  
            d = d + 4 * (x - y) + 10; 
        } 
        else
            d = d + 4 * x + 6; 
        drawCircle(xc, yc, x, y, colour); 
         
    } 
}

void draw_circle_filled(int xc, int yc, int r, int short colour){
	
	//draw a filled circle using the bresham algorithim
	for(int i=0;i<=r;i++){
		circleBres(xc,yc,i,colour);
	}
	if(nextRandom)	//if next one is ramdon
		draw_question_mark(xc, yc, colour);
}

void shifted_triangle_colours(short int *array){
	//get in the current order of the colours in the array
	short int temp0=array[0];
	short int temp1=array[1];
	short int temp2=array[2];
	short int temp3=array[3];
	
	//shift the colours
	array[0]=temp3;
	array[1]=temp0;
	array[2]=temp1;
	array[3]=temp2;

}


//plots a 3x3 box
void plot_box(int x_box, int y_box, int short color){
	for(int x=0; x<3;x++){
		for(int y=0; y<3;y++){
			plot_pixel(x_box+x,y_box+y,color);
		}
	}
}


void draw_line(int x0,int y0, int x1, int y1, short int colour){
	bool is_steep = abs(y1-y0) > abs(x1-x0); //check if magnitude greater than one of slope
	
	//if magnitude of slope greater than 1, switch x and y so we can iterrate through y
	if(is_steep){
		swap(&x0,&y0);
		swap(&x1,&y1);
	}
	
	//make it easier to draw (make it so the direction of the slope doesnt matter)
	if(x0>x1){
		swap(&x0,&x1);
		swap(&y0,&y1);
	}
	
	//process to tell when to iterate y
	int deltax=x1-x0;
	int deltay=abs(y1-y0);
	int error=-(deltax/2);
	int y=y0; //start iteration from the first y value;
	
	int y_step=0;
	
	//if line should be going up or down, y_step is different for each
	if(y0<y1){
		y_step=1;
	}
	else{
		y_step=-1;
	}
	//interate through x (which could also represent the y value)
	for(int x=x0; x<=x1; x++){
		if(is_steep)
			plot_pixel(y,x,colour);
		else
			plot_pixel(x,y,colour);
		
		//only increase the y value once we reach error is greater than 0.
		error=error+deltay;
		if(error>=0){
			y=y+y_step;
			error=error-deltax;	
		}
	}
}

void swap(int *x,int *y){
	int temp=*x;
	*x=*y;
	*y=temp;
}

void draw_score_digit(int x, int y, int num, int short color){
	if(num == 1){
		plot_box(x, y+6, color);
		plot_box(x+3, y+3, color);
		plot_box(x+6, y, color);
		plot_box(x+6, y+3, color);
		plot_box(x+6, y+6, color);
		plot_box(x+6, y+9, color);
		plot_box(x+6, y+12, color);
		plot_box(x+6, y+15, color);
		plot_box(x+6, y+18, color);
	}
	else if(num == 2){
		plot_box(x, y+3, color);
		plot_box(x, y+15, color);
		plot_box(x, y+18, color);
		plot_box(x+3, y, color);
		plot_box(x+3, y+12, color);
		plot_box(x+3, y+18, color);
		plot_box(x+6, y, color);
		plot_box(x+6, y+9, color);
		plot_box(x+6, y+18, color);
		plot_box(x+9, y+3, color);
		plot_box(x+9, y+6, color);
		plot_box(x+9, y+18, color);
	}
	else if(num == 3){
		plot_box(x, y+3, color);
		plot_box(x, y+15, color);
		plot_box(x+3, y, color);
		plot_box(x+3, y+9, color);
		plot_box(x+3, y+18, color);
		plot_box(x+6, y, color);
		plot_box(x+6, y+9, color);
		plot_box(x+6, y+18, color);
		plot_box(x+9, y+3, color);
		plot_box(x+9, y+6, color);
		plot_box(x+9, y+12, color);
		plot_box(x+9, y+15, color);
	}
	else if(num == 4){
		plot_box(x, y+9, color);
		plot_box(x, y+12, color);
		plot_box(x+3, y+6, color);
		plot_box(x+3, y+12, color);
		plot_box(x+6, y+3, color);
		plot_box(x+6, y+12, color);
		plot_box(x+9, y, color);
		plot_box(x+9, y+3, color);
		plot_box(x+9, y+6, color);
		plot_box(x+9, y+9, color);
		plot_box(x+9, y+12, color);
		plot_box(x+9, y+15, color);
		plot_box(x+9, y+18, color);
	}
	else if(num == 5){
		plot_box(x, y, color);
		plot_box(x, y+3, color);
		plot_box(x, y+6, color);
		plot_box(x, y+15, color);
		plot_box(x+3, y, color);
		plot_box(x+3, y+6, color);
		plot_box(x+3, y+18, color);
		plot_box(x+6, y, color);
		plot_box(x+6, y+6, color);
		plot_box(x+6, y+18, color);
		plot_box(x+9, y, color);
		plot_box(x+9, y+9, color);
		plot_box(x+9, y+12, color);
		plot_box(x+9, y+15, color);
	}
	else if(num == 6){
		plot_box(x, y+3, color);
		plot_box(x, y+6, color);
		plot_box(x, y+9, color);
		plot_box(x, y+12, color);
		plot_box(x, y+15, color);
		plot_box(x+3, y, color);
		plot_box(x+3, y+9, color);
		plot_box(x+3, y+18, color);
		plot_box(x+6, y, color);
		plot_box(x+6, y+9, color);
		plot_box(x+6, y+18, color);
		plot_box(x+9, y+3, color);
		plot_box(x+9, y+12, color);
		plot_box(x+9, y+15, color);
	}
	else if(num == 7){
		plot_box(x, y, color);
		plot_box(x+3, y, color);
		plot_box(x+3, y+12, color);
		plot_box(x+3, y+15, color);
		plot_box(x+3, y+18, color);
		plot_box(x+6, y, color);
		plot_box(x+6, y+9, color);
		plot_box(x+9, y, color);
		plot_box(x+9, y+3, color);
		plot_box(x+9, y+6, color);	
	}
	else if(num == 8){
		plot_box(x, y+3, color);
		plot_box(x, y+6, color);
		plot_box(x, y+12, color);
		plot_box(x, y+15, color);
		plot_box(x+3, y, color);
		plot_box(x+3, y+9, color);
		plot_box(x+3, y+18, color);
		plot_box(x+6, y, color);
		plot_box(x+6, y+9, color);
		plot_box(x+6, y+18, color);
		plot_box(x+9, y+3, color);
		plot_box(x+9, y+6, color);
		plot_box(x+9, y+12, color);
		plot_box(x+9, y+15, color);
	}
	else if(num == 9){
		plot_box(x, y+3, color);
		plot_box(x, y+6, color);
		plot_box(x, y+15, color);
		plot_box(x+3, y, color);
		plot_box(x+3, y+9, color);
		plot_box(x+3, y+18, color);
		plot_box(x+6, y, color);
		plot_box(x+6, y+9, color);
		plot_box(x+6, y+18, color);
		plot_box(x+9, y+3, color);
		plot_box(x+9, y+6, color);
		plot_box(x+9, y+9, color);
		plot_box(x+9, y+12, color);
		plot_box(x+9, y+15, color);
	}
	else if(num == 0){
		plot_box(x, y+3, color);
		plot_box(x, y+6, color);
		plot_box(x, y+9, color);
		plot_box(x, y+12, color);
		plot_box(x, y+15, color);
		plot_box(x+3, y, color);
		plot_box(x+3, y+18, color);
		plot_box(x+6, y, color);
		plot_box(x+6, y+18, color);
		plot_box(x+9, y+3, color);
		plot_box(x+9, y+6, color);
		plot_box(x+9, y+9, color);
		plot_box(x+9, y+12, color);
		plot_box(x+9, y+15, color);
	}
}


void draw_score(int short color){
	draw_score_digit(240, 10, (score%1000 - score%100)/100, color);
	draw_score_digit(255, 10, (score%100 - score%10)/10, color);
	draw_score_digit(270, 10, score%10, color);
}

void draw_question_mark(int xc, int yc, int short bg_color){
	int short color = 0xFFFF - bg_color;
	plot_pixel(xc - 3, yc - 4, color);
	plot_pixel(xc - 3, yc - 3, color);
	plot_pixel(xc - 3, yc - 2, color);
	plot_pixel(xc - 2, yc - 4, color);
	plot_pixel(xc - 2, yc - 3, color);
	plot_pixel(xc - 2, yc - 2, color);
	plot_pixel(xc - 1, yc - 5, color);
	plot_pixel(xc, yc - 5, color);
	plot_pixel(xc, yc, color);
	plot_pixel(xc, yc + 1, color);
	plot_pixel(xc, yc + 2, color);
	plot_pixel(xc, yc + 4, color);
	plot_pixel(xc, yc + 5, color);
	plot_pixel(xc + 1, yc - 5, color);
	plot_pixel(xc + 1, yc - 1, color);
	plot_pixel(xc + 1, yc, color);
	plot_pixel(xc + 1, yc + 1, color);
	plot_pixel(xc + 1, yc + 2, color);
	plot_pixel(xc + 1, yc + 4, color);
	plot_pixel(xc + 1, yc + 5, color);
	plot_pixel(xc + 2, yc - 5, color);
	plot_pixel(xc + 2, yc - 4, color);
	plot_pixel(xc + 2, yc - 3, color);
	plot_pixel(xc + 2, yc - 2, color);
	plot_pixel(xc + 2, yc - 1, color);
	plot_pixel(xc + 3, yc - 4, color);
	plot_pixel(xc + 3, yc - 3, color);
	plot_pixel(xc + 3, yc - 2, color);
	plot_pixel(xc + 3, yc - 1, color);
}


void draw_times_2(int xc, int yc, int short color){
	int x = xc - 10;
	int y = yc - 14;
	draw_score_digit(x + 17, y, 2, color);
	plot_box(x, y+3, color);
	plot_box(x, y+15, color);
	plot_box(x+3, y+6, color);
	plot_box(x+3, y+12, color);
	plot_box(x+6, y+9, color);
	plot_box(x+9, y+6, color);
	plot_box(x+9, y+12, color);
	plot_box(x+12, y+3, color);
	plot_box(x+12, y+15, color);
}






