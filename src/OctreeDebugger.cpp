#include "PCH.h"

#include "OctreeDebugger.h"
#include "BBox.h"


void OctreeDebugger::render(const Octree& rTree, size_t nDebugLevel, bool bRenderPoints)
{
    renderNode(rTree.m_pRoot, rTree.m_aBBox, 0, nDebugLevel, bRenderPoints);
}

void OctreeDebugger::renderNode(Octree::Node* pNode, const BBox& rBBox, size_t nLevel, size_t nDebugLevel, bool bRenderPoints)
{
    if (nLevel == nDebugLevel)
    {
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        rBBox.render();
    }
//
//    if (pNode)
//    {
//        if (pNode->isLeaf() && bRenderPoints)
//        {
//            glColor3f(1.0f,0.0f,0.0f);
//            glEnableClientState(GL_VERTEX_ARRAY);
//            glVertexPointer(3, GL_FLOAT, 0, &pNode->m_aPoints[0]);
//            glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(pNode->m_aPoints.size()));
//            glDisableClientState(GL_VERTEX_ARRAY); 
//        }
//        else
//        {
//            const BBox aBBoxLeft = rBBox.cutLeft(pNode->m_nAxis, pNode->m_fMedian);
//            const BBox aBBoxRight = rBBox.cutRight(pNode->m_nAxis, pNode->m_fMedian);
//
//            renderNode(pNode->m_pLeft, aBBoxLeft, nLevel+1, nDebugLevel, bRenderPoints);
//            renderNode(pNode->m_pRight, aBBoxRight, nLevel+1, nDebugLevel, bRenderPoints);
//        }
//    }
}

