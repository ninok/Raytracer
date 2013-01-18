#pragma once


#include "Octree.h"


class OctreeDebugger
{


public:

    static void render(const Octree& rTree, size_t nDebugLevel, bool bRenderPoints);


private:

    static void renderNode(Octree::Node* pNode, const BBox& rBBox, size_t nLevel, size_t nDebugLevel, bool bRenderPoints);

};

