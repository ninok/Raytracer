#include "PCH.h"

#include "Octree.h"

#include <algorithm>

#include "BBox.h"

struct XAxisComparator
{
    inline bool operator() (const glm::vec3& a, const glm::vec3& b)
    {
        return a.x < b.x;
    }
};

struct YAxisComparator
{
    inline bool operator() (const glm::vec3& a, const glm::vec3& b)
    {
        return a.y < b.y;
    }
};

struct ZAxisComparator
{
    inline bool operator() (const glm::vec3& a, const glm::vec3& b)
    {
        return a.z < b.z;
    }
};

struct LengthComparator
{
    inline bool operator() (const glm::vec3& a, const glm::vec3& b)
    {
        return glm::length(a) < glm::length(b);
    }
};

Octree::Octree (const BBox& rBBox, size_t nMaxPointsPerCell) : m_aBBox(rBBox), m_nMaxPointsPerCell(nMaxPointsPerCell), m_nDepth(0)
{
    
}

Octree::~Octree()
{
    delete m_pRoot;
}
