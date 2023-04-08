#ifndef DEBUG_H
#define DEBUG_H

#define CONSOLE_COLOR         (Color){ 30, 30, 30, 200 }
#define CONSOLE_EDGE_COLOR    (Color){ 25, 25, 25, 255 }
#define CONSOLE_FONT_SIZE     20
#define CONSOLE_WIDTH         600
#define CONSOLE_BUFFER_SIZE   16384 // 16 kb

size_t consoleBufferUsage = 0;
char* consoleBuffer;

void initConsole()
{
  consoleBuffer = (char*)malloc(CONSOLE_BUFFER_SIZE * sizeof(char));
  memset(consoleBuffer, 0, CONSOLE_BUFFER_SIZE);
}

void destroyConsole()
{
  free(consoleBuffer);
}

void drawConsole()
{
  const int consoleX = (WINDOW_WIDTH - CONSOLE_WIDTH) / 2;
  
  DrawRectangle(consoleX, 40, CONSOLE_WIDTH, 24, CONSOLE_EDGE_COLOR);
  DrawRectangle(consoleX, 64, CONSOLE_WIDTH, 376, CONSOLE_COLOR);
  DrawText("CONSOLE", (WINDOW_WIDTH - MeasureText("CONSOLE", CONSOLE_FONT_SIZE)) / 2, 42, CONSOLE_FONT_SIZE, RAYWHITE);
  
  if (consoleBufferUsage == 0) return;
  
  bool canDrawLine = false;
  unsigned int line = 0;
  size_t lineStartIndex = 0;
  
  for (size_t i = 1; i < consoleBufferUsage - 1; i++) {
    char nextChar = consoleBuffer[i + 1];
    consoleBuffer[i + 1] = 0;
    
    if (MeasureText(consoleBuffer + lineStartIndex, CONSOLE_FONT_SIZE) > CONSOLE_WIDTH - 10) {
      char currentChar = consoleBuffer[i];
      consoleBuffer[i] = 0;
      DrawText(consoleBuffer + lineStartIndex, consoleX + 5, line * 20 + 70, CONSOLE_FONT_SIZE, RAYWHITE);
      consoleBuffer[i] = currentChar;
      
      lineStartIndex = i;
      line++;
      canDrawLine = false;
    } else {
      canDrawLine = true;
    }
    consoleBuffer[i + 1] = nextChar;
  }
  
  if (canDrawLine) DrawText(consoleBuffer + lineStartIndex, consoleX + 5, line * 20 + 70, CONSOLE_FONT_SIZE, RAYWHITE);
}

void dumpToConsole(const char* s)
{
  size_t length = strlen(s);
  
  if (consoleBufferUsage + length > CONSOLE_BUFFER_SIZE) return;
  
  strcpy(consoleBuffer + consoleBufferUsage, s);
  
  consoleBufferUsage += length;
}

#endif // DEBUG_H