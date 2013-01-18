#pragma once

#include <vector>
#include <glm/glm.hpp>

enum TAxis
{
    AXIS_X, AXIS_Y, AXIS_Z
};

class BBox
{

public:

    BBox (const std::vector<glm::vec3>& rPoints);
    BBox (const glm::vec3& rMin, const glm::vec3& rMax); 

    BBox cutLeft(TAxis nAxis, float fLength) const;
    BBox cutRight(TAxis nAxis, float fLength) const;

    void render() const;

    bool containsPoint(const glm::vec3& rPoint);

private:

    static glm::vec3 calcBBoxMin(const std::vector<glm::vec3>& rPoints);
    static glm::vec3 calcBBoxMax(const std::vector<glm::vec3>& rPoints);

    const glm::vec3 m_v3Min;
    const glm::vec3 m_v3Max; 
};

