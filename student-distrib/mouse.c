/********************************************/
/*                                          */
/********************************************/

//refered to these links
// http://forum.osdev.org/viewtopic.php?t=10247
// http://houbysoft.com/download/ps2mouse.html

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "mouse.h"

#include "rtc.h"
#include "int_handler.h"
#include "paging.h"
#include "terminal.h"
#include "filesys.h"
#include "systemcalls.h"
#include "bootscreen.h" 


#define MOUSE_PORT        0x64                                                              //
#define MOUSE_IO_PORT     0x60                                                              //  
#define MOUSE_ENABLE      0xF4                                                              //
#define MOUSE_COMMAND     0xD4                                                              //
#define MOUSE_COMPAQ      0x20                                                              //
#define MOUSE_AUX_ENABLE  0xA8                                                              //
#define BIT_SHIFT_BY_3    3                                                                 //bit shift a number by 3
#define BIT_SHIFT_BY_4    4                                                                 //bit shift a number by 4
#define BIT_SHIFT_BY_5    5                                                                 //bit shift a number by 5
coord_t mouse_coords;                                                                       //struct for mouse coordinates
int32_t read_flag = 0;                                                                      //a flag to check if we should read the first packet or not              
uint8_t read_byte();                                                                        //this is to read the 2nd and 3rd byte from the mouse
uint8_t attempt_read();                                                                     //this is to read the 1st byte from the mouse
void move_mouse(int32_t x_change, int32_t y_change);                                        //this calculates the new position of the mouse pointer and takes care of coloring  
void attrib_changed(int32_t x, int32_t y);                                                  //this is for the screen color change on clicking 
int8_t input_packet;                                                                        //this is the 1st packet we get  

void init_mouse()
{
  mouse_coords.x = 79;                                                                      //initial mouse x coordinate
  mouse_coords.y = 24;                                                                      //initial mouse y coordinate 
  enable_irq(12);                                        

  // We need to enable the interrupt, for which we need the compaq status byte
  // Get the status byte 
  uint8_t status = inb(MOUSE_IO_PORT);

  // Bit 2 is responsible for enabling interrupt IRQ12.
  status = status | 0x2;

        //mouse initialization 
  outb(MOUSE_AUX_ENABLE, MOUSE_PORT);                                                                   //MOUSE_PORT 0x64
        //enable the interrupt
  outb(MOUSE_COMPAQ, MOUSE_PORT);                                                           //MOUSE_COMPAQ  0x20      MOUSE_PORT 0x64
  
  // Send the status back to the mouse
  outb(MOUSE_IO_PORT, MOUSE_PORT);                                                          //MOUSE_PORT 0x64         MOUSE_IO_PORT 0x60
  outb(status, MOUSE_IO_PORT);                                                              //MOUSE_IO_PORT 0x60
    
  //send modify status byte 
  outb(MOUSE_IO_PORT, MOUSE_PORT);                                                          //MOUSE_IO_PORT 0x60      MOUSE_PORT 0x64
  outb(0x47, MOUSE_IO_PORT);                                                                //MOUSE_IO_PORT 0x60

  // Tell the mouse we are sending a command
  outb(MOUSE_COMMAND, MOUSE_PORT);                                                          //MOUSE_PORT 0x64         MOUSE_COMMAND 0xD4
  outb(MOUSE_ENABLE, MOUSE_IO_PORT);                                                        //MOUSE_IO_PORT 0x60      MOUSE_ENABLE  0xF4
}


void mouse_int_handler()
{
  input_packet = attempt_read();                                                            // Packet 1 from mouse 
  
  if(((input_packet & 0x08) >> 3) == 1)                                                     //if a0/4th bit is 1-this to verify if it is a proper mouse packet
  {
    if((((input_packet & 0x40) >> 6) == 0) && (((input_packet & 0x80) >> 7) == 0))          //check for the overflow bit and do not allow it
    {
      uint8_t xm = inb(MOUSE_IO_PORT);                                                               //packet 2 is x_xhange in unsigned 8bit form
      uint8_t ym = inb(MOUSE_IO_PORT);                                                               //packet 3 is y_change in unsigned 8bit form

      int32_t x_change_packet = (int8_t)xm;                                                 //convert packet 2 to signed form
      int32_t y_change_packet = (int8_t)ym;                                                 //convert packet 3 to signed form

      x_change_packet = x_change_packet - ((input_packet << 0x04) & 0x100);                 //get the 9th bit to get proper sign bit for packet 2
      y_change_packet = y_change_packet - ((input_packet << 0x03) & 0x100);                 //get the 9th bit to get proper sign bit for packet 3

      //since mouse is too fast, we will reduce the amount by 1 del of speed
      x_change_packet = x_change_packet%3;                                                  //this is to control mouse speed in x direction
      y_change_packet = y_change_packet%2;                                                  //this is to control mouse speed in y direction

      move_mouse(x_change_packet, y_change_packet);                                         //this finds the new mouse coordinates

    // Left Click case
    if((input_packet & 0x01) == 1)
    {
      if((mouse_coords.x > -1 && mouse_coords.x <= 10)&&(mouse_coords.y == 24))
      {
        screen_num = 1;
        terminal_switch(0, screen_num);
        send_eoi(12);
      }
      if((mouse_coords.x > 10 && mouse_coords.x <= 20)&&(mouse_coords.y == 24))
      {
        screen_num = 2;
        terminal_switch(1, screen_num);
        send_eoi(12);
      }
      if((mouse_coords.x > 20 && mouse_coords.x <= 31)&&(mouse_coords.y == 24))
      {
        screen_num = 3;
        terminal_switch(2, screen_num);
        send_eoi(12);
      }                                           
    }

    // Right Click case
    if(((input_packet & 0x02) >> 1) == 1)                                                   //change color on right click             
    {
      color_pick = (color_pick+1)%5;
      set_palette();                                                                        //changes background color
      printb(screens[screen_num]);
    }

    // Middle button press
    if(((input_packet & 0x04) >> 2) == 1);
      //printf("%d, %d",mouse_coords.x, mouse_coords.y);                                      //this calls a function 
    }
   } 
  send_eoi(12);
}


//this tries to read the first packet
uint8_t attempt_read()
{
  if((inb(MOUSE_PORT) & 0x1) == 0)                                                          //this is not a mouse packet
  {
    read_flag = 0;                                                                          //dont read
    return 0;
  }
  else
  {
    read_flag = 1;                                                                          //read packet
    return inb(MOUSE_IO_PORT);                                                                       //get mouse packet
  }
}

//This function calculates the change in the x and y directions to get the new coordinates
void move_mouse(int32_t x_change, int32_t y_change)
{
  set_colour(mouse_coords.x, mouse_coords.y, colour_palette[color_pick]);                   //change the previous color on screen of mouse position  
  
  //taking care of x_change
  if(((input_packet && 0x10) >> BIT_SHIFT_BY_4) > 0)                                                     //if positive x change
    mouse_coords.x += x_change;

  else if(((input_packet && 0x10) >> BIT_SHIFT_BY_4) < 0)                                                //if negative x change 
    mouse_coords.x = mouse_coords.x + x_change;
    
  else if(((input_packet && 0x10) >> BIT_SHIFT_BY_4) == 0)                                               //if no change then add -> this is just for safety purpose 
    mouse_coords.x += x_change;

    
  if(mouse_coords.x < 0)                                                                    //in case x goes out of bounds
    mouse_coords.x = 0;
    
  else if(mouse_coords.x > 79)                                                              //in case x goes out of bounds
    mouse_coords.x = 79;

  //taking care of y_change
  if(((input_packet && 0x20) >> BIT_SHIFT_BY_5) > 0)                                                     //if y change is positive
    mouse_coords.y += y_change;
    
  else if(((input_packet && 0x20) >> BIT_SHIFT_BY_5) < 0)                                                //if y change is negative
    mouse_coords.y -= y_change;                                                             //we subtract because y axis has axis flipped

  else if(((input_packet && 0x20) >> BIT_SHIFT_BY_5) == 0)                                               //if no change then subtract -> this is just for safety purpose
    mouse_coords.y -= y_change;

  if(mouse_coords.y < 0)                                                                    //in case y goes out of bounds
    mouse_coords.y = 0;
  
  else if(mouse_coords.y > 24)                                                              //in case y goes out of bounds
    mouse_coords.y = 24;                               
    
  //color the curson on the screen
  show_cursor();                                                                            //show mouse cursor at new position on screen
}

//this simple changes the video memory of the current position to a new color which becomes the mouse pointer
void show_cursor()
{
  set_colour(mouse_coords.x, mouse_coords.y, MOUSE_PORT);                                         //here the mouse pointer color is 0x64
}
