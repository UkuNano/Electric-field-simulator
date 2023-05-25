#ifndef GLOBAL_H
#define GLOBAL_H

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MIDGRAY     (Color){ 120, 120, 120, 255 }
#define ALPHA_BLACK (Color){  10,  10,  10, 200 }
#define ALPHA_WHITE (Color){ 240, 240, 240, 120 }

#define VEC2_ZERO (Vector2){ 0.0f, 0.0f }
#define VEC2_ONE  (Vector2){ 1.0f, 1.0f }

#define ACTION_NONE 0

typedef struct Vector2int
{
  int x, y;
} Vector2int;

Vector2int vec2ToVec2int(Vector2 vec)
{
  return (Vector2int){ (int)vec.x, (int)vec.y };
}

Vector2 vec2intToVec2(Vector2int vec)
{
  return (Vector2){ (float)vec.x, (float)vec.y };
}

void printFloat(float value, const char* unit, int x, int y)
{
  char buf[32];
  sprintf(buf, "%.3f %s", value, unit);
  buf[31] = 0;
  DrawText(buf, x, y, 20, RAYWHITE);
}

void printInt(int value, const char* unit, int x, int y)
{
  char buf[32];
  sprintf(buf, "%i %s", value, unit);
  buf[31] = 0;
  DrawText(buf, x, y, 20, RAYWHITE);
}

// For some stupid reason the fabsf() function returns an uninitsialised float.
// I commented that function from math.h header.
float fabsf(float x)
{
	if (x < 0) return -x;
	return x;
}

bool isPointInRect(Vector2 pos, float x, float y, float w, float h)
{
  return ( pos.x > x && pos.x < x + w &&
           pos.y > y && pos.y < y + h );
}

bool isPointInRectRec(Vector2 pos, Rectangle rec)
{
  return ( pos.x > rec.x && pos.x < rec.x + rec.width &&
           pos.y > rec.y && pos.y < rec.y + rec.height );
}

bool isPointInRectV(Vector2 pos, Vector2 a, Vector2 b)
{
  return ( pos.x > a.x && pos.x < b.x &&
           pos.y > a.y && pos.y < b.y );
}

Color lerpColor(Color a, Color b, float t)
{
  return (Color){ (unsigned char)Lerp((float)a.r, (float)b.r, t),
                  (unsigned char)Lerp((float)a.g, (float)b.g, t),
                  (unsigned char)Lerp((float)a.b, (float)b.b, t), 255};
}

Vector3 toVec3(Vector2 v)
{
  return (Vector3){ v.x, v.y, 1.0f };
}

Vector2 toVec2(Vector3 v)
{
  return (Vector2){ v.x, v.y };
}

#endif // GLOBAL_H