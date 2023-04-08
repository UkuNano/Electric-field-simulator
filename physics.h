#ifndef PHYSICS_H
#define PHYSICS_H

#define STRENGTH_COLOR_A  (Color){  30,  30,  70, 255 }
#define STRENGTH_COLOR_B  (Color){ 200, 200, 200, 255 }
#define STRENGTH_VALUE_B   10.0f
#define STRENGTH_COLOR_C  (Color){ 255, 255, 30, 255 }
#define STRENGTH_VALUE_C   30.0f
#define STRENGTH_COLOR_D  (Color){ 255, 120, 15, 255 }
#define STRENGTH_VALUE_D   50.0f
#define STRENGTH_COLOR_E  (Color){ 220, 20, 10, 255 }
#define STRENGTH_VALUE_E   75.0f

#define FIELD_LINE_MAX_STRENGTH 500.0f
#define FIELD_LINE_MAX_DISTANCE 40.0f
#define FIELD_LINE_STEP_PX      10.0f

#define COULOMB_CONSTANT  8987551792.0f
#define NANO_COULOMB      0.000000001f

Vector2 getStrengthVectorFromPointCharge(Vector2 samplePosition, Vector2 chargePos, float charge)
{
  Vector2 displacement = Vector2Subtract(samplePosition, chargePos);
  Vector2 dir = Vector2Normalize(displacement);
  float sqrDist = Vector2LengthSqr(displacement);
  float scalar = COULOMB_CONSTANT * charge / sqrDist;
  
  return Vector2Scale(dir, scalar);
}

Vector2 getFieldStrength(Vector2 samplePosition)
{
  Vector2 strengthVector = VEC2_ZERO;
  
  for (unsigned int i = 0; i < pointChargeCount; i++) {
    Vector2 vec = getStrengthVectorFromPointCharge(samplePosition, pointCharges[i].position, pointCharges[i].charge);
    strengthVector = Vector2Add(strengthVector, vec);
  }
  
  for (unsigned int i = 0; i < chargeMeshCount; i++) {
    for (unsigned int j = 0; j < chargeMeshes[i].assetHandle->chargeCount; j++) {
      Vector3 transformed = multiplyMatrixVector(&chargeMeshes[i].transform, toVec3(chargeMeshes[i].assetHandle->charges[j].position));
      
      Vector2 vec = getStrengthVectorFromPointCharge(samplePosition, toVec2(transformed), chargeMeshes[i].assetHandle->charges[j].charge);
      strengthVector = Vector2Add(strengthVector, vec);
    }
  }
  
  return strengthVector;
}

Color getColorFromStrength(float strength)
{
  if (strength < STRENGTH_VALUE_B) {
    return lerpColor(STRENGTH_COLOR_A, STRENGTH_COLOR_B, strength / STRENGTH_VALUE_B);
    
  } else if (strength < STRENGTH_VALUE_C) {
    return lerpColor(STRENGTH_COLOR_B, STRENGTH_COLOR_C, (strength - STRENGTH_VALUE_B) / (STRENGTH_VALUE_C - STRENGTH_VALUE_B));
    
  } else if (strength < STRENGTH_VALUE_D) {
    return lerpColor(STRENGTH_COLOR_C, STRENGTH_COLOR_D, (strength - STRENGTH_VALUE_C) / (STRENGTH_VALUE_D - STRENGTH_VALUE_C));
    
  } else if (strength < STRENGTH_VALUE_E) {
    return lerpColor(STRENGTH_COLOR_D, STRENGTH_COLOR_E, (strength - STRENGTH_VALUE_D) / (STRENGTH_VALUE_E - STRENGTH_VALUE_D));
  }
  
  return STRENGTH_COLOR_E;
}

void drawStrengthGradient()
{
  const float step = STRENGTH_VALUE_E / 200.0f;
  float strength = 0.0f;
 
  DrawRectangle(WINDOW_WIDTH - 45, WINDOW_HEIGHT - 225, 45, 225, ALPHA_BLACK);
 
  for (int i = 0; i < 200; i++) {
    Color c = getColorFromStrength(strength);
    DrawLine(WINDOW_WIDTH - 20, WINDOW_HEIGHT - 10 - i, WINDOW_WIDTH - 10, WINDOW_HEIGHT - 10 - i, c);
    strength += step;
  }
  
  DrawLine(WINDOW_WIDTH - 20, WINDOW_HEIGHT - 211, WINDOW_WIDTH - 20, WINDOW_HEIGHT - 10, MIDGRAY);
  DrawLine(WINDOW_WIDTH - 9, WINDOW_HEIGHT - 211, WINDOW_WIDTH - 9, WINDOW_HEIGHT - 10, MIDGRAY);
  DrawLine(WINDOW_WIDTH - 20, WINDOW_HEIGHT - 210, WINDOW_WIDTH - 10, WINDOW_HEIGHT - 210, MIDGRAY);
  DrawLine(WINDOW_WIDTH - 20, WINDOW_HEIGHT - 10, WINDOW_WIDTH - 10, WINDOW_HEIGHT - 10, MIDGRAY);
  
  DrawLine(WINDOW_WIDTH - 25, WINDOW_HEIGHT - 10, WINDOW_WIDTH - 21, WINDOW_HEIGHT - 10, MIDGRAY);
  DrawLine(WINDOW_WIDTH - 25, WINDOW_HEIGHT - 50, WINDOW_WIDTH - 21, WINDOW_HEIGHT - 50, MIDGRAY);
  DrawLine(WINDOW_WIDTH - 25, WINDOW_HEIGHT - 90, WINDOW_WIDTH - 21, WINDOW_HEIGHT - 90, MIDGRAY);
  DrawLine(WINDOW_WIDTH - 25, WINDOW_HEIGHT - 130, WINDOW_WIDTH - 21, WINDOW_HEIGHT - 130, MIDGRAY);
  DrawLine(WINDOW_WIDTH - 25, WINDOW_HEIGHT - 170, WINDOW_WIDTH - 21, WINDOW_HEIGHT - 170, MIDGRAY);
  DrawLine(WINDOW_WIDTH - 25, WINDOW_HEIGHT - 210, WINDOW_WIDTH - 21, WINDOW_HEIGHT - 210, MIDGRAY);
  
  DrawText("0", WINDOW_WIDTH - 32, WINDOW_HEIGHT - 15, 10, MIDGRAY);
  DrawText("15", WINDOW_WIDTH - 35, WINDOW_HEIGHT - 55, 10, MIDGRAY);
  DrawText("30", WINDOW_WIDTH - 38, WINDOW_HEIGHT - 95, 10, MIDGRAY);
  DrawText("45", WINDOW_WIDTH - 38, WINDOW_HEIGHT - 135, 10, MIDGRAY);
  DrawText("60", WINDOW_WIDTH - 38, WINDOW_HEIGHT - 175, 10, MIDGRAY);
  DrawText("75", WINDOW_WIDTH - 38, WINDOW_HEIGHT - 215, 10, MIDGRAY);
  DrawText("V/m", WINDOW_WIDTH - 25, WINDOW_HEIGHT - 223, 10, MIDGRAY);
}

void drawFieldLine(Vector2 startPos)
{
  Vector2 samplePoint = startPos;
  float step = FIELD_LINE_STEP_PX / (float)(WINDOW_WIDTH/2) * camera.view.x;
  
  for (int i = 0; i < 200; i++) {
    if (Vector2DistanceSqr(samplePoint, camera.position) > FIELD_LINE_MAX_DISTANCE * FIELD_LINE_MAX_DISTANCE) return;
    
    Vector2 strengthVector = getFieldStrength(samplePoint);
    float strength = Vector2Length(strengthVector);
    
    if (strength > FIELD_LINE_MAX_STRENGTH && i > 5) return;
    
    Vector2 nextPoint = Vector2Add(samplePoint, Vector2Scale(strengthVector, step / strength));
    
    DrawLineV(worldToScreen(samplePoint), worldToScreen(nextPoint), RAYWHITE);
    samplePoint = nextPoint;
  }
}

void drawFieldLines()
{
  for (unsigned int i = 0; i < pointChargeCount; i++) {
    if (pointCharges[i].charge > 0) {
      for (int j = 0; j < 12; j++) {
        float angle = PI / 6.0f * j;
        Vector2 point = Vector2Add(pointCharges[i].position,
                                  (Vector2){ cos(angle) * POINT_CHARGE_RADIUS, sin(angle) * POINT_CHARGE_RADIUS });
        drawFieldLine(point);
      }
    }
  }
}

/*
void drawFieldImage()
{
  for (int x = 0; x < WINDOW_WIDTH; x++) {
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
      Vector2 samplePoint = screenToWorld((Vector2){ x, y });
      float strength = Vector2Length(getFieldStrength(samplePoint));
      DrawPixel(x, y, getColorFromStrength(strength));
    }
  }
}
*/

#endif // PHYSICS_H