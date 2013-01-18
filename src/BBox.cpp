#include "PCH.h"

#include "BBox.h"


BBox::BBox(const std::vector<glm::vec3>& rPoints) :
    m_v3Min(calcBBoxMin(rPoints)),
    m_v3Max(calcBBoxMax(rPoints))
{
}


BBox::BBox (const glm::vec3& rMin, const glm::vec3& rMax) :
    m_v3Min(rMin),
    m_v3Max(rMax)
{
}

BBox BBox::cutLeft(TAxis nAxis, float fLength) const
{
    glm::vec3 v3Min = m_v3Min;
    glm::vec3 v3Max = m_v3Max;

    v3Max[nAxis] = fLength;
    
    return BBox(v3Min, v3Max);
}

BBox BBox::cutRight(TAxis nAxis, float fLength) const
{
    glm::vec3 v3Min = m_v3Min;
    glm::vec3 v3Max = m_v3Max;

    v3Min[nAxis] = fLength;
    
    return BBox(v3Min, v3Max);
}


glm::vec3 BBox::calcBBoxMin(const std::vector<glm::vec3>& rPoints)
{
    glm::vec3 aMin(std::numeric_limits<float>::max());

    BOOST_FOREACH(const glm::vec3& rPoint, rPoints)
    {
        if (rPoint.x < aMin.x)
        {
            aMin.x = rPoint.x;
        } 
        if (rPoint.y < aMin.y)
        {
            aMin.y = rPoint.y;
        } 
        if (rPoint.z < aMin.z)
        {
            aMin.z = rPoint.z;
        } 
    }

    return aMin;

}

glm::vec3 BBox::calcBBoxMax(const std::vector<glm::vec3>& rPoints)
{
    glm::vec3 aMax(-std::numeric_limits<float>::max());

    BOOST_FOREACH(const glm::vec3& rPoint, rPoints)
    {
        if (rPoint.x > aMax.x)
        {
            aMax.x = rPoint.x;
        } 
        if (rPoint.y > aMax.y)
        {
            aMax.y = rPoint.y;
        } 
        if (rPoint.z > aMax.z)
        {
            aMax.z = rPoint.z;
        } 
    }
    return aMax;
}


void BBox::render() const
{
   
//       7--------6
//      /|        /
//     / |       /|
//    /  |      / |
//    3--------2  |
//    |  4-----|--5
//    | /      |  /
//    |/       | /
//    |        |/
//    0--------1
    
    using glm::vec3;
    vec3 pVertices[8] =
    {
        vec3(m_v3Min.x, m_v3Min.y, m_v3Min.z),
        vec3(m_v3Max.x, m_v3Min.y, m_v3Min.z),
        vec3(m_v3Max.x, m_v3Max.y, m_v3Min.z),
        vec3(m_v3Min.x, m_v3Max.y, m_v3Min.z),
        vec3(m_v3Min.x, m_v3Min.y, m_v3Max.z),
        vec3(m_v3Max.x, m_v3Min.y, m_v3Max.z),
        vec3(m_v3Max.x, m_v3Max.y, m_v3Max.z),
        vec3(m_v3Min.x, m_v3Max.y, m_v3Max.z),
    };
    
    GLushort pLineIndices[24] =
    {
        0,1, 1,2, 2,3, 3,0, //front
        0,4, 1,5, 2,6, 3,7, //front to back        
        4,5, 5,6, 6,7, 7,4, //back
    };
    
    
    glColor3f(1.0f,0.0f,0.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, pVertices);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, pLineIndices);
    glDisableClientState(GL_VERTEX_ARRAY);
}

bool BBox::containsPoint(const glm::vec3& rPoint)
{
    
    return glm::all(glm::greaterThanEqual(rPoint, m_v3Min)) && glm::all(glm::lessThanEqual(rPoint, m_v3Max));
}