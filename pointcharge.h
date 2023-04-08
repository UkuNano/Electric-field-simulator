#ifndef POINTCHARGE_H
#define POINTCHARGE_H

#define PCHARGE_COLOR (Color){ 255, 60, 30, 255 }
#define NCHARGE_COLOR (Color){ 50, 90, 255, 255 }
#define POINT_CHARGE_RADIUS 0.25f

typedef struct PointCharge
{
  float charge;
  Vector2 position;
} PointCharge;

PointCharge* pointCharges = NULL;
size_t pointChargeBufferSize = 0;
unsigned int pointChargeCount = 0;

void createPointChargeBuffer(unsigned int chargeCount)
{
  pointChargeBufferSize = chargeCount * sizeof(PointCharge);
  pointCharges = (PointCharge*)malloc(pointChargeBufferSize);
}

void destroyPointChargeBuffer()
{
  free(pointCharges);
}

void resizePointChargeBuffer()
{
  PointCharge* newBuffer = (PointCharge*)malloc(pointChargeBufferSize * 2);
  memcpy(newBuffer, pointCharges, pointChargeBufferSize);
  free(pointCharges);
  pointCharges = newBuffer;
  pointChargeBufferSize *= 2;
}

void addPointCharge(float _charge, Vector2 _position)
{
  if ((pointChargeCount + 1) * sizeof(PointCharge) > pointChargeBufferSize)
    resizePointChargeBuffer();
  
  pointCharges[pointChargeCount].charge = _charge;
  pointCharges[pointChargeCount].position = _position;
  
  pointChargeCount++;
}

void removePointCharge(unsigned int index)
{
  for (unsigned int i = index + 1; i < pointChargeCount; i++) {
    pointCharges[i - 1].charge = pointCharges[i].charge;
    pointCharges[i - 1].position = pointCharges[i].position;
  }
  
  pointChargeCount--;
}

void drawPointCharges()
{
  // printInt(pointChargeBufferSize, "bytes", 10, 100);
  // printInt(pointChargeCount * sizeof(PointCharge), "bytes", 10, 130);
  
  for (unsigned int i = 0; i < pointChargeCount; i++) {
    if (pointCharges[i].charge > 0) {
      DrawCircleV(worldToScreen(pointCharges[i].position), POINT_CHARGE_RADIUS / camera.view.x * (float)(WINDOW_WIDTH/2), PCHARGE_COLOR);
      
      Vector2 p1 = worldToScreen(Vector2Add(pointCharges[i].position, (Vector2){  0.03f,  0.20f } ));
      Vector2 p2 = worldToScreen(Vector2Add(pointCharges[i].position, (Vector2){ -0.03f, -0.20f } ));
      Vector2 p3 = worldToScreen(Vector2Add(pointCharges[i].position, (Vector2){  0.20f,  0.03f } ));
      Vector2 p4 = worldToScreen(Vector2Add(pointCharges[i].position, (Vector2){ -0.20f, -0.03f } ));
      
      DrawRectangleV(p1, Vector2Subtract(p2, p1), RAYWHITE);
      DrawRectangleV(p3, Vector2Subtract(p4, p3), RAYWHITE);
    } else {
      DrawCircleV(worldToScreen(pointCharges[i].position), POINT_CHARGE_RADIUS / camera.view.x * (float)(WINDOW_WIDTH/2), NCHARGE_COLOR);
      
      Vector2 p1 = worldToScreen(Vector2Add(pointCharges[i].position, (Vector2){  0.20f,  0.03f } ));
      Vector2 p2 = worldToScreen(Vector2Add(pointCharges[i].position, (Vector2){ -0.20f, -0.03f } ));
      
      DrawRectangleV(p1, Vector2Subtract(p2, p1), RAYWHITE);
    }
  }
}

#endif // POINTCHARGE_H