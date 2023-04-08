#ifndef MAT3_H
#define MAT3_H

#define degsin(x) sin((x) * DEG2RAD)
#define degcos(x) cos((x) * DEG2RAD)

#define MAT3_IDENTITY (Mat3){ 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f }

typedef struct Mat3
{
  float m11, m12, m13;
  float m21, m22, m23;
  float m31, m32, m33;
} Mat3;

Vector3 multiplyMatrixVector(Mat3* m, Vector3 v)
{
	return (Vector3){ m->m11 * v.x + m->m12 * v.y + m->m13 * v.z,
                    m->m21 * v.x + m->m22 * v.y + m->m23 * v.z,
                    m->m31 * v.x + m->m32 * v.y + m->m33 * v.z };
}

Mat3 multiplyMatrixMatrix(Mat3* a, Mat3* b)
{
  return (Mat3){ a->m11*b->m11 + a->m12*b->m21 + a->m13*b->m31, a->m11*b->m12 + a->m12*b->m22 + a->m13*b->m32, a->m11*b->m13 + a->m12*b->m23 + a->m13*b->m33,
                 a->m21*b->m11 + a->m22*b->m21 + a->m23*b->m31, a->m21*b->m22 + a->m22*b->m22 + a->m23*b->m32, a->m21*b->m13 + a->m22*b->m23 + a->m23*b->m33,
                 a->m31*b->m11 + a->m32*b->m21 + a->m33*b->m31, a->m31*b->m32 + a->m32*b->m22 + a->m33*b->m32, a->m31*b->m13 + a->m32*b->m23 + a->m33*b->m33 };
}

Mat3 translateMatrix(Mat3* m, Vector2 v)
{
  Mat3 translation = (Mat3){ 1.0f, 0.0f,  v.x,
                             0.0f, 1.0f,  v.y,
                             0.0f, 0.0f, 1.0f };
  
  return multiplyMatrixMatrix(m, &translation);
}

Mat3 scaleMatrix(Mat3* m, Vector2 v)
{
  Mat3 scale = (Mat3){  v.x, 0.0f, 0.0f,
                       0.0f,  v.y, 0.0f,
                       0.0f, 0.0f, 1.0f };
  
  return multiplyMatrixMatrix(m, &scale);
}

Mat3 rotateMatrix(Mat3* m, float x)
{
  Mat3 rotation = (Mat3){ degcos(x), -degsin(x), 0.0f,
                          degsin(x),  degcos(x), 0.0f,
                               0.0f,       0.0f, 1.0f };
  
  return multiplyMatrixMatrix(m, &rotation);
}

/*
Mat3 translateRotateMatrix(Mat3* m, Vector2 v, float x)
{
  Mat3 rotation = (Mat3){ degcos(x), degsin(x),  v.x,
                          degsin(x), degcos(x),  v.y,
                               0.0f,      0.0f, 1.0f };
  
  return multiplyMatrixMatrix(m, &rotation);
}
*/

#endif // MAT3_H