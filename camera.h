#ifndef CAMERA_H
#define CAMERA_H

#define DEFAULT_ZOOM  (Vector2){ 10.0f, 7.5f }
#define ZOOM_STEP     (Vector2){  2.0f, 1.5f }

typedef struct CameraView
{
  Vector2 position, view;
} CameraView;

CameraView camera;

Vector2 worldToScreen(Vector2 p)
{
  float screenX = (p.x - camera.position.x) / camera.view.x * (float)(WINDOW_WIDTH/2) + (float)WINDOW_WIDTH/2;
  float screenY = (p.y - camera.position.y) / -camera.view.y * (float)(WINDOW_HEIGHT/2) + (float)WINDOW_HEIGHT/2;
  
  return (Vector2) { screenX, screenY };
}

Vector2 worldToScreenDir(Vector2 v)
{
  float screenX = v.x / camera.view.x * (float)(WINDOW_WIDTH/2);
  float screenY = v.y / -camera.view.y * (float)(WINDOW_HEIGHT/2);
  
  return (Vector2) { screenX, screenY };
}

Vector2 screenToWorld(Vector2 p)
{
  float worldX = (p.x - (float)(WINDOW_WIDTH/2)) / (float)(WINDOW_WIDTH/2) * camera.view.x + camera.position.x;
  float worldY = (p.y - (float)(WINDOW_HEIGHT/2)) / (float)(WINDOW_HEIGHT/2) * -camera.view.y + camera.position.y;
  
  return (Vector2) { worldX, worldY };
}

Vector2 screenToWorldDir(Vector2 v)
{
  float worldX = v.x / (float)(WINDOW_WIDTH/2) * camera.view.x;
  float worldY = v.y / (float)(WINDOW_HEIGHT/2) * -camera.view.y;
  
  return (Vector2) { worldX, worldY };
}

void updateCamera()
{
  if (IsMouseButtonDown(2)) {
    Vector2 delta = screenToWorldDir(GetMouseDelta());
    camera.position = Vector2Subtract(camera.position, delta);
  }
  
  camera.view = Vector2Add(camera.view, Vector2Scale(ZOOM_STEP, -GetMouseWheelMove()));
  
  if (camera.view.x < ZOOM_STEP.x)
    camera.view = ZOOM_STEP;
}

void drawGrid()
{
  float camX = (int)camera.position.x;
  float camY = (int)camera.position.y;
  int limit = (int)camera.view.x + 1;
  
  for (int i = -limit; i < limit + 1; i++) {
    Vector2 point = worldToScreen( (Vector2){ camX + i, camY + i } );
    DrawLine(point.x, 0, point.x, WINDOW_HEIGHT, (Color){ 30, 30, 30, 255 } );
    DrawLine(0, point.y, WINDOW_WIDTH, point.y, (Color){ 30, 30, 30, 255 } );
  }
  
  Vector2 zeroPoint = worldToScreen( (Vector2){ 0.0f, 0.0f } );
  DrawLine(zeroPoint.x, 0, zeroPoint.x, WINDOW_HEIGHT, (Color){ 50, 50, 50, 255 } );
  DrawLine(0, zeroPoint.y, WINDOW_WIDTH, zeroPoint.y, (Color){ 50, 50, 50, 255 } );
}

#endif // CAMERA_H