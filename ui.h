#ifndef UI_H
#define UI_H

#define MENU_ELEM_TYPE_BUTTON 0
#define MENU_ELEM_TYPE_BREAK  1
#define MENU_ELEM_TYPE_STATIC 2

#define MIN_MENU_WIDTH        100
#define MENU_FONT_SIZE        20
#define MENU_TEXT_ELEM_HEIGHT 25
#define MENU_BREAK_HEIGHT     10

#define MENU_BACKGROUND_COLOR (Color){ 20, 20, 20, 255 }
#define MENU_HIGHLIGHT_COLOR  (Color){ 60, 60, 60, 255 }

typedef struct MenuElement
{
  unsigned int actionId;
  unsigned char type;
  char* text;
} MenuElement;

typedef struct Menu
{
  MenuElement* elements;
  unsigned int elemCount;
  size_t bufferSize;
  unsigned int width, height;
} Menu;

bool isRightClickMenuEnabled = false;
Vector2 rightClickPosition = VEC2_ZERO;
Vector2 rightClickMenuPosition = VEC2_ZERO;
Menu rightClickMenu;

void createMenu(Menu* menu, unsigned int allocElemCount)
{
  menu->bufferSize = allocElemCount * sizeof(MenuElement);
  menu->elements = (MenuElement*)malloc(menu->bufferSize);
  menu->elemCount = 0;
  menu->height = 0;
  menu->width = MIN_MENU_WIDTH;
}

void destroyMenu(Menu* menu)
{
  for (size_t i = 0; i < menu->elemCount; i++)
    free(menu->elements[i].text);
  
  free(menu->elements);
}

void addMenuElement(Menu* menu, unsigned char _type, char* _text, unsigned int _actionId)
{
  if (menu->elemCount * sizeof(MenuElement) > menu->bufferSize) {
#ifndef RELEASE
    dumpToConsole("Menu buffer too small!");
#endif
    return;
  }
  
  MenuElement* newElem = &menu->elements[menu->elemCount];
  
  newElem->actionId = _actionId;
  newElem->type = _type;
  
  size_t len = strlen(_text) + 1; // strlen() does not count the final null character
  newElem->text = (char*)malloc(len * sizeof(char));
  strncpy(newElem->text, _text, len);
  
  unsigned int textWidth = MeasureText(_text, MENU_FONT_SIZE);
  if (textWidth + 10 > menu->width)
    menu->width = textWidth + 10;
  
  if (_type == MENU_ELEM_TYPE_BUTTON || _type == MENU_ELEM_TYPE_STATIC) {
    menu->height += MENU_TEXT_ELEM_HEIGHT;
  } else if (_type == MENU_ELEM_TYPE_BREAK) {
    menu->height += MENU_BREAK_HEIGHT;
  }
  
  menu->elemCount++;
}

void drawMenu(Menu* menu, Vector2 position, Vector2 mousePos)
{ 
  DrawRectangle(position.x, position.y, menu->width, menu->height, MENU_BACKGROUND_COLOR);
  DrawLine(position.x, position.y, position.x + menu->width, position.y, MIDGRAY);
  DrawLine(position.x, position.y + menu->height, position.x + menu->width, position.y + menu->height, MIDGRAY);
  DrawLine(position.x, position.y, position.x, position.y + menu->height, MIDGRAY);
  DrawLine(position.x + menu->width, position.y, position.x + menu->width, position.y + menu->height, MIDGRAY);
  
  unsigned int currentY = position.y;
  
  for (size_t i = 0; i < menu->elemCount; i++) {
    unsigned char type = menu->elements[i].type;
    
    switch (type)
    {
    case MENU_ELEM_TYPE_BUTTON:
      if (isPointInRect(mousePos, position.x, currentY, menu->width, MENU_TEXT_ELEM_HEIGHT))
        DrawRectangle(position.x + 1, currentY, menu->width - 1, MENU_TEXT_ELEM_HEIGHT, MENU_HIGHLIGHT_COLOR);
      
      DrawText(menu->elements[i].text, position.x + 5, currentY + 2, MENU_FONT_SIZE, RAYWHITE);
      currentY += MENU_TEXT_ELEM_HEIGHT;
      break;
    case MENU_ELEM_TYPE_STATIC:
      DrawText(menu->elements[i].text, position.x + 5, currentY + 2, MENU_FONT_SIZE, MIDGRAY);
      currentY += MENU_TEXT_ELEM_HEIGHT;
      break;
    case MENU_ELEM_TYPE_BREAK:
      DrawLine(position.x, currentY + 5, position.x + menu->width - 1, currentY + 5, MIDGRAY);
      currentY += MENU_BREAK_HEIGHT;
      break;
    }
  }
}

unsigned int getActionFromButtons(Vector2 mousePos)
{
  if (isRightClickMenuEnabled) {
    unsigned int currentY = rightClickMenuPosition.y;
    
    for (size_t i = 0; i < rightClickMenu.elemCount; i++) {
      unsigned int type = rightClickMenu.elements[i].type;
      
      switch (type)
      {
      case MENU_ELEM_TYPE_BUTTON:
        if (isPointInRect(mousePos, rightClickMenuPosition.x, currentY, rightClickMenu.width, MENU_TEXT_ELEM_HEIGHT)) {
          isRightClickMenuEnabled = false;
          return rightClickMenu.elements[i].actionId;
        }
      case MENU_ELEM_TYPE_STATIC:
        currentY += MENU_TEXT_ELEM_HEIGHT;
        break;
      case MENU_ELEM_TYPE_BREAK:
        currentY += MENU_BREAK_HEIGHT;
        break;
      }
    }
  }
  
  return ACTION_NONE;
}

bool isMouseOnUI()
{
  return ( isRightClickMenuEnabled && isPointInRect(GetMousePosition(),
           rightClickMenuPosition.x, rightClickMenuPosition.y, rightClickMenu.width, rightClickMenu.height) );
}

#endif // UI_H