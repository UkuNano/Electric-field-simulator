#ifndef VECTORFIELD_H
#define VECTORFIELD_H

typedef struct VectorFieldChunk
{
  Vector2 vectors[64]; // Row 1, row 2, row 3, ...
  Vector2int position;
  bool enabled;
} VectorFieldChunk;

VectorFieldChunk* chunks;
size_t chunkBufferSize = 0;
unsigned int chunkCount = 0;

void createChunkBuffer(unsigned int maxChunkCount)
{
  chunkBufferSize = sizeof(VectorFieldChunk) * maxChunkCount;
  chunks = (VectorFieldChunk*)malloc(chunkBufferSize);
}

void resizeChunkBuffer(unsigned int maxChunkCount)
{
  VectorFieldChunk* newBuffer = (VectorFieldChunk*)malloc(sizeof(VectorFieldChunk) * maxChunkCount);
  // memcpy(newBuffer, chunks, chunkBufferSize); // void *memcpy(void* dest, const void* src, size_t n)
  
  unsigned int newBufferIndex = 0;
  
  // Only copy the chunks that will be used
  for (unsigned int i = 0; i < chunkCount; i++) {
    if (chunks[i].enabled) {
      memcpy(&newBuffer[newBufferIndex], &chunks[i], sizeof(VectorFieldChunk));
      newBufferIndex++;
    }
  }
  
  free(chunks);
  chunks = newBuffer;
  chunkBufferSize = sizeof(VectorFieldChunk) * maxChunkCount;
  chunkCount = newBufferIndex;
}

void destroyChunkBuffer()
{
  free(chunks);
}

void computeChunk(unsigned int index)
{
  // Keep this unsigned
  for (int i = 0; i < 64; i++)
    chunks[index].vectors[i] = getFieldStrength( (Vector2){ chunks[index].position.x + i % 8, chunks[index].position.y + i / 8 } );
}

void createChunk(Vector2int pos)
{ 
  unsigned int newIndex = chunkCount;
  bool isReplacingDisabledChunk = false;
  
  // Find a chunk marked for removal
  for (unsigned int i = 0; i < chunkCount; i++) {
    if (!chunks[i].enabled) {
      newIndex = i;
      isReplacingDisabledChunk = true;
      break;
    }
  }
  
  if (newIndex >= chunkCount && sizeof(VectorFieldChunk) * (chunkCount + 1) > chunkBufferSize)
    return;
  
  chunks[newIndex].position = pos;
  chunks[newIndex].enabled = true;
  computeChunk(newIndex);
  
  if (!isReplacingDisabledChunk) chunkCount++;
}

void updateVectorFieldChunks(bool forceRefresh)
{
  Vector2int firstChunkPos = (Vector2int){ ( (int)floorf(camera.position.x / 8.0f) - ((int)floorf(camera.view.x / 8.0f) + 1) ) * 8,
                                           ( (int)floorf(camera.position.y / 8.0f) - ((int)floorf(camera.view.y / 8.0f) + 1) ) * 8};
  
  Vector2int lastChunkPos = (Vector2int){ ( (int)floorf(camera.position.x / 8.0f) + (int)floorf(camera.view.x / 8.0f) + 1 ) * 8,
                                          ( (int)floorf(camera.position.y / 8.0f) + (int)floorf(camera.view.y / 8.0f) + 1 ) * 8};
  
  unsigned int newChunkCount = (lastChunkPos.x - firstChunkPos.x + 8) * (lastChunkPos.y - firstChunkPos.y + 8) / 64;
  
  bool addNewChunks = false;
  
  // Remove old unneccesary chunks
  for (unsigned int i = 0; i < chunkCount; i++) {
    Vector2int pos = chunks[i].position;
    
    if (pos.x < firstChunkPos.x || pos.x > lastChunkPos.x || pos.y < firstChunkPos.y || pos.y > lastChunkPos.y) {
      chunks[i].enabled = false;
      addNewChunks = true;
    }
  }
  
  if (forceRefresh) {
    addNewChunks = true;
    
    // Disable all chunks so they will be replaced later on
    for (unsigned int i = 0; i < chunkCount; i++)
      chunks[i].enabled = false;
  }
  
  if (sizeof(VectorFieldChunk) * newChunkCount != chunkBufferSize) {
    addNewChunks = addNewChunks || ( sizeof(VectorFieldChunk) * newChunkCount > chunkBufferSize );
    
    resizeChunkBuffer(newChunkCount);
  }
  
  
  if (addNewChunks) {
    Vector2int samplePoint = firstChunkPos;
    
    while (samplePoint.x <= lastChunkPos.x) {
      while (samplePoint.y <= lastChunkPos.y) {
        bool found = false;
        
        // Check if a chunk at that position already exists
        for (unsigned int i = 0; i < chunkCount; i++) {
          if (chunks[i].position.x == samplePoint.x && chunks[i].position.y == samplePoint.y && chunks[i].enabled) {
            found = true;
            break;
          }
        }
        
        if (!found)
          createChunk(samplePoint);
        
        samplePoint.y += 8;
      }
      
      samplePoint.y = firstChunkPos.y;
      samplePoint.x += 8;
    }
  }
}

void drawVector(Vector2 vec, Vector2 pos, Color color)
{
  Vector2 normal = worldToScreenDir(Vector2Scale( (Vector2){ vec.y, -vec.x }, 0.15f ));
  Vector2 arrowPos = worldToScreen(Vector2Add(pos, Vector2Scale(vec, 0.75f)));
  Vector2 tip = worldToScreen(Vector2Add(pos, vec));
  
  DrawLineV(worldToScreen(pos), tip, color);
  DrawLineV(Vector2Add(arrowPos, normal), tip, color);
  DrawLineV(Vector2Add(arrowPos, Vector2Negate(normal)), tip, color);
}

void drawVectorFieldChunks()
{
  for (unsigned int i = 0; i < chunkCount; i++) {
    if (chunks[i].enabled) {
      // Keep this unsigned
      for (int j = 0; j < 64; j++) {
        Vector2 pos = (Vector2){ chunks[i].position.x + j % 8, chunks[i].position.y + j / 8 };
        Vector2 vec = chunks[i].vectors[j];
        float strength = Vector2Length(vec);
        
        if (strength > 0.05f)
          drawVector(Vector2Scale(vec, 1.0f / strength), pos, getColorFromStrength(strength));
      }
    }
  }
}

#endif // VECTORFIELD_H