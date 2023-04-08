#ifndef SELECT_H
#define SELECT_H

#define SELECTION_COLOR (Color){ 230, 180, 20, 255 }

#define SELECTION_NONE  0
#define SELECTION_POINT 1
#define SELECTION_MESH  2

void* selection = NULL;
unsigned int selectionType = SELECTION_NONE;

void getSelection(Vector2 screenPos)
{
  Vector2 pos = screenToWorld(screenPos);
  
  for (unsigned int i = 0; i < pointChargeCount; i++) {
    if (Vector2DistanceSqr(pointCharges[i].position, pos) < POINT_CHARGE_RADIUS * POINT_CHARGE_RADIUS) {
      selection = (void*)&pointCharges[i];
      selectionType = SELECTION_POINT;
      return;
    }
  }
  
  for (unsigned int i = 0; i < chargeMeshCount; i++) {
    Rectangle rec = chargeMeshes[i].boundingBox;
    
    if (pos.x > rec.x && pos.x < rec.x + rec.width && pos.y < rec.y && pos.y > rec.y + rec.height) {
      selection = (void*)&chargeMeshes[i];
      selectionType = SELECTION_MESH;
      return;
    }
  }
  
  selection = NULL;
  selectionType = SELECTION_NONE;
}

void drawSelection()
{
  if (selectionType == SELECTION_POINT) {
    Vector2 p1 = worldToScreen(Vector2Add(((PointCharge*)selection)->position, (Vector2){ -POINT_CHARGE_RADIUS, POINT_CHARGE_RADIUS }));
    Vector2 p2 = worldToScreen(Vector2Add(((PointCharge*)selection)->position, (Vector2){ POINT_CHARGE_RADIUS, -POINT_CHARGE_RADIUS }));
    Vector2 deltap = Vector2Subtract(p2, p1);
    
    DrawRectangleLines(p1.x, p1.y, deltap.x, deltap.y, SELECTION_COLOR);
  } else if (selectionType == SELECTION_MESH) {
    Rectangle rec = ((ChargeMesh*)selection)->boundingBox;
    Vector2 p1 = worldToScreen((Vector2){ rec.x, rec.y });
    Vector2 deltap = worldToScreenDir((Vector2){ rec.width, rec.height });
    
    DrawRectangleLines(p1.x, p1.y, deltap.x, deltap.y, SELECTION_COLOR);
  }
}

#endif // SELECT_H