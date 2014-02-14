#include <Pager.h>
#include <Ribbon.h>
#include "icon.h"

char VersionPtr[] = "BBVT";
int AppStackSize = 1000;

static LcdConfig LCD; // Height & Width of LCD

void parseESC(vector* in_cmd)
{
  string temp;
  for(int i=0; i < in_cmd.length; i++){
    string temp += in_cmd[i];
  }
  if(temp == "[H" || temp == "[;H" || temp == "[f" || temp == "[;f") //move cursour to the top left of the screen
  {
    //do that
  }
  else if(temp == "D") //scroll screen up
  {
    //do that
  }
  else if(temp == "M") //scroll screen down
  {
    //do that
  }
  else if(temp == "E") //next line
  {
    //do that
  }
  else if(temp == "6n") //cursour position request
  {
    int cursorx, cursory;
    LcdGetCursorPoint(&cursorx,&cursory);
    //todo: convert to text position
    CommSendBuffer({27,'[',cursory,';',cursorx,'R'},6);
  }
}

void putchar(char c)
{
  if(LcdPutStringXY(-1,-1,&c,1,LCD_MULTILINE)==0||c=='\n')
  {
    LcdScroll(LcdGetFontHeight(-1));
    LcdSetCursorPoint(0,LCD.height-LcdGetFontHeight(-1));
    LcdPutStringXY(-1,-1,&c,1,0);
  }
}

void TermThread(void)
{
  int c;
  vector<char> cmd;
  MESSAGE msg;
  PID pid;
  pid.Name = VersionPtr;
  pid.EnableForeground = TRUE;
  pid.Icon = NULL;
  RimSetPID(&pid);
  LcdGetConfig(&LCD); // Get LCD configuration
  RimRequestForeground(RimGetCurrentTaskID());
  LcdCopyBitmapToDisplay(&IconBMP, 0, 0);
  LcdPutStringXY(20,8,"BBVT v0.01\n\n",-1,LCD_MULTILINE);
  CommOpenPort(9600,8,COMM_NO_PARITY,1,2048,2048);

  for (;;)              // Start main loop
  {
    RimGetMessage(&msg);
    switch (msg.Device) // Msg loop
    {
      case DEVICE_KEYPAD:
        switch (msg.Event)
        {
          case KEY_DOWN:
          {
            c=msg.SubMsg;
            switch (c)
            {
              case KEY_ESCAPE: // ESCAPE key exits
                CommClosePort();
                RibbonShowRibbon();
                RimTerminateThread();
                break;
              default:
                CommSendChar(c);
                #putchar(c); // Echo char
            }
            break;
          }
        }
      case DEVICE_COM1:
        switch (msg.Event)
        {
          case COMM_RX_AVAILABLE:
	    char in = CommReadChar();
	    if(esc)
	    {
	      cmd.push_back(in);
	      if(cmd.length > 3)
		esc = false;
	    }
	    if(in == 27)
	      esc = true;
	    else
	      putchar(in);
        }
    } // Msg
  }   // Main
}

void PagerMain(void)
{
  RibbonRegisterFunction(VersionPtr, &IconBMP, 0, 40, TermThread, AppStackSize);
  return;
}

