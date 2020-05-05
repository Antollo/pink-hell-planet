
#include "terrain.h"


Terrain::Terrain()
{
    //TODO reading from vector
    for (int x = 0; x < 4 * TerrainChunk::chunkSize; x += TerrainChunk::chunkSize)
        for (int z = 0; z < 2 * TerrainChunk::chunkSize; z += TerrainChunk::chunkSize)
            chunks.emplace(getVecInt3(x, 0, z), new TerrainChunk({x, 0, z}, this, true));
    chunks.emplace(getVecInt3(0, TerrainChunk::chunkSize, 0), new TerrainChunk({0, TerrainChunk::chunkSize, 0}, this, true));
}

VecInt3 Terrain::getChunkFromPoint(VecInt3 pos)
{
    for (auto& i : pos)
    {
        if (i < 0)
            i -= TerrainChunk::chunkSize - 1;
        i /= TerrainChunk::chunkSize;
        i *= TerrainChunk::chunkSize;
    }
    return pos;
}

void Terrain::draw(Window *window) const
{
    for (auto& i : chunks)
        i.second->draw(window);
}

void Terrain::updateBuffers()
{
    for (auto i : toRegen)
    {
        auto it = chunks.find(i);
        if (it == chunks.end())
            it = chunks.emplace(i, new TerrainChunk(i, (Terrain*)this, false)).first;
        it->second->updateBuffers();
    }
    toRegen.clear();
}

void Terrain::TerrainCube::addPoints(int d)
{
    if (size == 1)
        chunk->terrain->marchingPointsAdd(intPos, d);
    else
    {
        for (int x = 0; x < size; x++)
            for (int y = 0; y < size; y++)
            {
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(x, y, 0), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(x, y, size - 1), d);
            }
        for (int z = 1; z < size - 1; z++)
        {
            for (int other = 0; other < size - 1; other++)
            {
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(other + 1, 0, z), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(other, size - 1, z), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(0, other, z), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(size - 1, other + 1, z), d);
            }
        }
    }
}

void Terrain::TerrainCube::genBuffers()
{
    if (whole)
        chunk->drawAtPos(size, intPos);
    else
    {
        for (auto& i : childs)
        {
            if (i)
            {
                if (i->childCount == 0)
                    i.reset();
                else
                    i->genBuffers();
            }
        }
    }
}
