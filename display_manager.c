#include "display_manager.h"
#include "LCD.h"
#include "intrinsics.h"
#include "msp430fg4617.h"
#include "clock.h"
#include "stopwatch.h"

static char sw_changed = 0;
static char clk_changed = 0;
static char display_mode = CLOCK;
static unsigned lcd_timer = 0;
static const unsigned lcd_delay = 5;   //in 1/100 of a second

static unsigned greeting_delay = 0;

static unsigned message_delay = 0;
static unsigned char state_before_message = CLOCK;

void dm_init()
{
  P2DIR = 0xF;
  P2OUT = 0x0;
  LCD_init();
  LCD_print("BOUJOUR!");
  dm_setDisplayMode(CLOCK);
}

void dm_clkChanged()
{
  clk_changed = 1;
}

void dm_setDisplayMode(char dmode)
{
  display_mode = dmode;
  switch(display_mode)
  {
  case CLOCK:
    P2OUT = 1;
    clk_changed = 1;
    break;
  case STOPWATCH:    
    sw_changed = 1;
    break;
  }
}

void dm_swChanged()
{
  sw_changed = 1;
}

void dm_tick()
{
  sw_changed = 1;
  clk_changed = 1;
  if(greeting_delay < 100)
  {
    greeting_delay++;
    return;
  }
  message_delay--;
  if(message_delay == 0)
  {
    display_mode = state_before_message;
    sw_changed = 1;
    clk_changed = 1;
  }
  lcd_timer++;
  if(lcd_timer >= lcd_delay)
  {
    switch(display_mode)
    {
    case CLOCK:
      if(clk_changed)
      {
        LCD_print(clk_tostring());
        clk_changed = 0;
        lcd_timer = 0;
        /*
        P2OUT = P2OUT << 1;
        if((P2OUT & 0xF) == 0)
          P2OUT = 1;*/
      }
      break;
    case STOPWATCH:
      if(sw_changed)
      {
        LCD_print(sw_tostring());
        sw_changed = 0;
        lcd_timer = 0;
      }
      break;
      
    case MESSAGE:
     
      break;
  }
  }
}

void dm_displayMessage(char* message, unsigned delay)
{
  message_delay = delay;
  if(display_mode != MESSAGE)
    state_before_message = display_mode;
  display_mode = MESSAGE;
  LCD_clear();
  LCD_print(message);
}