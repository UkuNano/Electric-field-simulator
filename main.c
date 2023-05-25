#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

// #define RAYMATH_IMPLEMENTATION
#include "raymath.h"

#include "global.h"

#ifndef RELEASE
#include "debug.h"
#endif

#include "camera.h"
#include "pointcharge.h"
#include "chargemesh.h"
#include "ui.h"
#include "select.h"
#include "physics.h"
#include "vectorfield.h"

#define ACTION_ADD_POINTPA         1
#define ACTION_ADD_POINTNA         2
#define ACTION_ADD_POINTPB         3
#define ACTION_ADD_POINTNB         4
#define ACTION_ADD_POINTPC         5
#define ACTION_ADD_POINTNC         6
#define ACTION_ADD_POINTPD         7
#define ACTION_ADD_POINTND         8
// #define ACTION_ADD_LINEMESHP       5
// #define ACTION_ADD_LINEMESHN       6
#define ACTION_ADD_CIRCLEMESHP     9
#define ACTION_ADD_CIRCLEMESHN    10
// #define ACTION_ADD_SQUAREMESHP     9
// #define ACTION_ADD_SQUAREMESHN    10
#define ACTION_ADD_TRIANGLEMESHP  11
#define ACTION_ADD_TRIANGLEMESHN  12
#define ACTION_TOGGLE_VEC_FIELD   13
#define ACTION_TOGGLE_FIELD_LINES 14
#define ACTION_DELETE             15

// #define ACTION_TOGGLE_FIELD_IMAGE 16

bool isConsoleEnabled = false;
bool isMouseVectorEnabled = false;
bool isVectorFieldEnabled = true;
bool isFieldLinesEnabled = true;

// bool isFieldImageEnabled = false;

// MeshAsset lineMeshP;
// MeshAsset lineMeshN;
MeshAsset circleMeshP;
MeshAsset circleMeshN;
// MeshAsset squareMeshP;
// MeshAsset squareMeshN;
MeshAsset triangleMeshP;
MeshAsset triangleMeshN;

unsigned int getActionFromInput()
{
  if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) return ACTION_DELETE;
  return ACTION_NONE;
}

void handleAction(unsigned int actionId)
{
  switch (actionId)
  {
  case ACTION_ADD_POINTPA:
    addPointCharge(NANO_COULOMB * 10, screenToWorld(rightClickPosition));
    break;
  case ACTION_ADD_POINTNA:
    addPointCharge(-NANO_COULOMB * 10, screenToWorld(rightClickPosition));
    break;
  case ACTION_ADD_POINTPB:
    addPointCharge(NANO_COULOMB * 50, screenToWorld(rightClickPosition));
    break;
  case ACTION_ADD_POINTNB:
    addPointCharge(-NANO_COULOMB * 50, screenToWorld(rightClickPosition));
    break;
  case ACTION_ADD_POINTPC:
    addPointCharge(NANO_COULOMB * 100, screenToWorld(rightClickPosition));
    break;
  case ACTION_ADD_POINTNC:
    addPointCharge(-NANO_COULOMB * 100, screenToWorld(rightClickPosition));
    break;
  case ACTION_ADD_POINTPD:
    addPointCharge(NANO_COULOMB * 200, screenToWorld(rightClickPosition));
    break;
  case ACTION_ADD_POINTND:
    addPointCharge(-NANO_COULOMB * 200, screenToWorld(rightClickPosition));
    break;
//  case ACTION_ADD_LINEMESHP:
//    addChargeMesh(&lineMeshP, screenToWorld(rightClickPosition), -60.0f);
//    break;
//  case ACTION_ADD_LINEMESHN:
//    addChargeMesh(&lineMeshN, screenToWorld(rightClickPosition), -60.0f);
//    break;
  case ACTION_ADD_CIRCLEMESHP:
    addChargeMesh(&circleMeshP, screenToWorld(rightClickPosition), 0.0f);
    break;
  case ACTION_ADD_CIRCLEMESHN:
    addChargeMesh(&circleMeshN, screenToWorld(rightClickPosition), 0.0f);
    break;
//  case ACTION_ADD_SQUAREMESHP:
//    addChargeMesh(&squareMeshP, screenToWorld(rightClickPosition), 45.0f);
//    break;
//  case ACTION_ADD_SQUAREMESHN:
//    addChargeMesh(&squareMeshN, screenToWorld(rightClickPosition), 45.0f);
//    break;
  case ACTION_ADD_TRIANGLEMESHP:
    addChargeMesh(&triangleMeshP, screenToWorld(rightClickPosition), 0.0f);
    break;
  case ACTION_ADD_TRIANGLEMESHN:
    addChargeMesh(&triangleMeshN, screenToWorld(rightClickPosition), 0.0f);
    break;
  case ACTION_TOGGLE_VEC_FIELD:
    isVectorFieldEnabled = !isVectorFieldEnabled;
    break;
//  case ACTION_TOGGLE_FIELD_IMAGE:
//    isFieldImageEnabled = !isFieldImageEnabled;
//    break;
  case ACTION_TOGGLE_FIELD_LINES:
    isFieldLinesEnabled = !isFieldLinesEnabled;
    break;
  case ACTION_DELETE:
    if (selectionType == SELECTION_POINT) {
      removePointCharge((unsigned int)((PointCharge*)selection - pointCharges)); // This suspicious code calculates the index
      selection = NULL;
      selectionType = SELECTION_NONE;
    } else if (selectionType == SELECTION_MESH) {
      removeChargeMesh((unsigned int)((ChargeMesh*)selection - chargeMeshes));
      selection = NULL;
      selectionType = SELECTION_NONE;
    }
    break;
  }
}

void onUpdate()
{
  unsigned int oldChargeCount = pointChargeCount + chargeMeshCount;

#ifndef RELEASE
  if (IsKeyPressed(KEY_Q))
    dumpToConsole("Hello!");
  
  if (IsKeyPressed(KEY_TAB))
    isConsoleEnabled = !isConsoleEnabled;
#endif

  if (IsMouseButtonPressed(1)) {
    rightClickPosition = GetMousePosition();
    rightClickMenuPosition = rightClickPosition;
    
    if (rightClickMenuPosition.x + rightClickMenu.width > WINDOW_WIDTH)
      rightClickMenuPosition.x -= rightClickMenuPosition.x + rightClickMenu.width - WINDOW_WIDTH + 1;
    
    if (rightClickMenuPosition.y + rightClickMenu.height > WINDOW_HEIGHT)
      rightClickMenuPosition.y -= rightClickMenuPosition.y + rightClickMenu.height - WINDOW_HEIGHT + 1;
    
    isRightClickMenuEnabled = true;
  }
  
  unsigned int actionId = ACTION_NONE;
  
  if (IsMouseButtonPressed(0)) {
    actionId = getActionFromButtons(GetMousePosition());
    handleAction(actionId);
    
    if (!isMouseOnUI()) {
      isRightClickMenuEnabled = false;
      getSelection(GetMousePosition());
    }
  }
  
  isMouseVectorEnabled = ( IsMouseButtonDown(0) && !isMouseOnUI() );
  
  actionId = getActionFromInput();
  handleAction(actionId);
  
  updateVectorFieldChunks(pointChargeCount + chargeMeshCount != oldChargeCount);
  updateCamera();
}

void onDraw()
{
  ClearBackground( (Color){ 20, 20, 20, 255 } );
  
//  if (isFieldImageEnabled)
//    drawFieldImage();
  
  drawGrid();
  
  if (isVectorFieldEnabled && (pointChargeCount > 0 || chargeMeshCount > 0))
    drawVectorFieldChunks();
  
  if (isFieldLinesEnabled)
    drawFieldLines();
  
  if (isMouseVectorEnabled) {
    Vector2 samplePoint = screenToWorld(GetMousePosition());
    Vector2 strengthVector = getFieldStrength(samplePoint);
    float strength = Vector2Length(strengthVector);
    
    drawVector(Vector2Scale(strengthVector, 1.0f / strength), samplePoint, RAYWHITE);
    DrawRectangle(0, 0, 150, 30, ALPHA_BLACK);
    printFloat(strength, "V/m", 5, 5);
  }
  
  drawChargeMeshes();
  drawPointCharges();
  drawSelection();
  drawStrengthGradient();
  
  if (isRightClickMenuEnabled)
    drawMenu(&rightClickMenu, rightClickMenuPosition, GetMousePosition());

#ifndef RELEASE
  if (isConsoleEnabled) {
    drawConsole();
    printInt(GetFPS(), "FPS", 5, 5);
  }
#endif
}

void onStart()
{
#ifndef RELEASE
  initConsole();
#endif

  createPointChargeBuffer(10);
  createChargeMeshBuffer(3);
  createChunkBuffer(20);
  
  createMenu(&rightClickMenu, 17);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_STATIC, "Add", 0);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "+10 uC point", ACTION_ADD_POINTPA);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "-10 uC point", ACTION_ADD_POINTNA);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "+50 uC point", ACTION_ADD_POINTPB);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "-50 uC point", ACTION_ADD_POINTNB);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "+100 uC point", ACTION_ADD_POINTPC);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "-100 uC point", ACTION_ADD_POINTNC);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "+200 uC point", ACTION_ADD_POINTPD);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "-200 uC point", ACTION_ADD_POINTND);
//  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "+Line", ACTION_ADD_LINEMESHP);
//  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "-Line", ACTION_ADD_LINEMESHN);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "+1 nC * 72 Circle", ACTION_ADD_CIRCLEMESHP);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "-1 nC * 72 Circle", ACTION_ADD_CIRCLEMESHN);
//  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "+Square", ACTION_ADD_SQUAREMESHP);
//  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "-Square", ACTION_ADD_SQUAREMESHN);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "+1 nC * 60 Triangle", ACTION_ADD_TRIANGLEMESHP);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "-1 nC * 60 Triangle", ACTION_ADD_TRIANGLEMESHN);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BREAK, "", 0);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_STATIC, "View", 0);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "Vector field", ACTION_TOGGLE_VEC_FIELD);
  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "Field lines", ACTION_TOGGLE_FIELD_LINES);
  
//  addMenuElement(&rightClickMenu, MENU_ELEM_TYPE_BUTTON, "Field image", ACTION_TOGGLE_FIELD_IMAGE);

//  lineMeshP = generateMeshAsset(MESHASSET_LINE, NANO_COULOMB);
//  lineMeshN = generateMeshAsset(MESHASSET_LINE, -NANO_COULOMB);
  circleMeshP = generateMeshAsset(MESHASSET_CIRCLE, NANO_COULOMB);
  circleMeshN = generateMeshAsset(MESHASSET_CIRCLE, -NANO_COULOMB);
//  squareMeshP = generateMeshAsset(MESHASSET_SQUARE, NANO_COULOMB);
//  squareMeshN = generateMeshAsset(MESHASSET_SQUARE, -NANO_COULOMB);
  triangleMeshP = generateMeshAsset(MESHASSET_TRIANGLE, NANO_COULOMB);
  triangleMeshN = generateMeshAsset(MESHASSET_TRIANGLE, -NANO_COULOMB);
  
  camera.position = VEC2_ZERO;
  camera.view = DEFAULT_ZOOM;
}

void onEnd()
{
  CloseWindow();
  destroyPointChargeBuffer();
  destroyChargeMeshBuffer();
  destroyChunkBuffer();
//  free(lineMeshP.charges);
//  free(lineMeshN.charges);
  free(circleMeshP.charges);
  free(circleMeshN.charges);
//  free(squareMeshP.charges);
//  free(squareMeshN.charges);
  free(triangleMeshP.charges);
  free(triangleMeshN.charges);
  destroyMenu(&rightClickMenu);

#ifndef RELEASE
  destroyConsole();
#endif
}

int main(void)
{
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Electric field simulator");
  SetTargetFPS(60);
  onStart();
  
  while (!WindowShouldClose())
  {
    onUpdate();
    
    BeginDrawing();
    onDraw();
    EndDrawing();
  }
  
  onEnd();
  return 0;
}