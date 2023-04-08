#ifndef MESH_H
#define MESH_H

#include "mat3.h"

#define MESHASSET_LINE      0
#define MESHASSET_CIRCLE    1
#define MESHASSET_SQUARE    2
#define MESHASSET_TRIANGLE  3

typedef struct MeshAsset
{
  PointCharge* charges;
  unsigned int chargeCount;
} MeshAsset;

typedef struct ChargeMesh
{
  Mat3 transform;
  MeshAsset* assetHandle;
  Rectangle boundingBox;
} ChargeMesh;

ChargeMesh* chargeMeshes = NULL;
size_t meshBufferSize = 0;
unsigned int chargeMeshCount = 0;

MeshAsset generateMeshAsset(unsigned int type, float charge)
{
  MeshAsset asset;
  
  switch (type)
  {
  case MESHASSET_LINE:
    asset.chargeCount = 100;
    asset.charges = (PointCharge*)malloc(asset.chargeCount * sizeof(PointCharge));
    
    for (int i = 0; i < 100; i++)
      asset.charges[i] = (PointCharge){ charge, (Vector2){ (float)i / 20.0f, 0.0f } };
    
    break;
  case MESHASSET_CIRCLE:
    asset.chargeCount = 72;
    asset.charges = (PointCharge*)malloc(asset.chargeCount * sizeof(PointCharge));
    
    for (int i = 0; i < 72; i++)
      asset.charges[i] = (PointCharge){ charge, (Vector2){ degcos((float)i * 5.0f) * 2.5f, degsin((float)i * 5.0f) * 2.5f } };
    
    break;
  case MESHASSET_SQUARE:
    asset.chargeCount = 200;
    asset.charges = (PointCharge*)malloc(asset.chargeCount * sizeof(PointCharge));
    
    for (int i = 0; i < 50; i++)
      asset.charges[i] = (PointCharge){ charge, (Vector2){ (float)i / 10.0f - 2.5f, -2.5f } };

    for (int i = 0; i < 50; i++)
      asset.charges[i + 50] = (PointCharge){ charge, (Vector2){ 2.5f, (float)i / 10.0f - 2.5f } };

    for (int i = 0; i < 50; i++)
      asset.charges[i + 100] = (PointCharge){ charge, (Vector2){ (float)i / -10.0f + 2.5f, 2.5f } };

    for (int i = 0; i < 50; i++)
      asset.charges[i + 150] = (PointCharge){ charge, (Vector2){ -2.5f, (float)i / -10.0f + 2.5f } };

    break;
  case MESHASSET_TRIANGLE:
    asset.chargeCount = 61;
    asset.charges = (PointCharge*)malloc(asset.chargeCount * sizeof(PointCharge));
    
    for (int i = 0; i < 20; i++)
      asset.charges[i] = (PointCharge){ charge, (Vector2){ (float)i / 4.0f, 0.0f } };

    for (int i = 0; i < 21; i++)
      asset.charges[i + 20] = (PointCharge){ charge, (Vector2){  5.0f - degcos(60.0f) * (float)i / 4.0f,
                                                                        degsin(60.0f) * (float)i / 4.0f } };

    for (int i = 0; i < 20; i++)
      asset.charges[i + 41] = (PointCharge){ charge, (Vector2){ degcos(60.0f) * (float)i / 4.0f,
                                                                degsin(60.0f) * (float)i / 4.0f } };
    break;
  }
  
  return asset;
}

void createChargeMeshBuffer(unsigned int chargeMeshCount)
{
  meshBufferSize = chargeMeshCount * sizeof(ChargeMesh);
  chargeMeshes = (ChargeMesh*)malloc(meshBufferSize);
}

void destroyChargeMeshBuffer()
{
  free(chargeMeshes);
}

void resizeChargeMeshBuffer()
{
  ChargeMesh* newBuffer = (ChargeMesh*)malloc(meshBufferSize * 2);
  memcpy(newBuffer, chargeMeshes, meshBufferSize);
  free(chargeMeshes);
  chargeMeshes = newBuffer;
  meshBufferSize *= 2;
}

void calculateChargeMeshBoundingBox(ChargeMesh* mesh)
{
  Vector2 min = (Vector2){ 10000000, 10000000 };
  Vector2 max = (Vector2){ -10000000, -10000000 };
  
  for (unsigned int i = 0; i < mesh->assetHandle->chargeCount; i++) {
    Vector2 transformed = toVec2(multiplyMatrixVector(&mesh->transform, toVec3(mesh->assetHandle->charges[i].position)));
    
    if (transformed.x < min.x)
      min.x = transformed.x;
    
    if (transformed.y < min.y)
      min.y = transformed.y;
    
    if (transformed.x > max.x)
      max.x = transformed.x;
    
    if (transformed.y > max.y)
      max.y = transformed.y;
  }
  
  mesh->boundingBox = (Rectangle){ min.x, max.y, (max.x - min.x), (min.y - max.y) };
}

void addChargeMesh(MeshAsset* _assetHandle, Vector2 _position, float _rotation)
{
  if ((chargeMeshCount + 1) * sizeof(ChargeMesh) > meshBufferSize)
    resizeChargeMeshBuffer();
  
  Mat3 _transform = { 1.0f, 0.0f, _position.x,
                      0.0f, 1.0f, _position.y,
                      0.0f, 0.0f, 1.0f };
  
  _transform = rotateMatrix(&_transform, _rotation);
  // _transform = scaleMatrix(&_transform, _scale);
  
  chargeMeshes[chargeMeshCount].transform = _transform;
  chargeMeshes[chargeMeshCount].assetHandle = _assetHandle;
  
  calculateChargeMeshBoundingBox(&chargeMeshes[chargeMeshCount]);
  
  chargeMeshCount++;
}

void removeChargeMesh(unsigned int index)
{
  for (unsigned int i = index + 1; i < chargeMeshCount; i++) {
    chargeMeshes[i - 1].transform = chargeMeshes[i].transform;
    chargeMeshes[i - 1].assetHandle = chargeMeshes[i].assetHandle;
    chargeMeshes[i - 1].boundingBox = chargeMeshes[i].boundingBox;
  }
  
  chargeMeshCount--;
}

void drawChargeMeshes()
{
  for (unsigned int i = 0; i < chargeMeshCount; i++) {
    for (unsigned int j = 0; j < chargeMeshes[i].assetHandle->chargeCount; j++) {
      Vector3 transformed = multiplyMatrixVector(&chargeMeshes[i].transform, toVec3(chargeMeshes[i].assetHandle->charges[j].position));
      
      if (chargeMeshes[i].assetHandle->charges[j].charge > 0) {
        DrawCircleV(worldToScreen(toVec2(transformed)), 3, PCHARGE_COLOR);
      } else {
        DrawCircleV(worldToScreen(toVec2(transformed)), 3, NCHARGE_COLOR);
      }
    }
  }
}

#endif // MESH_H