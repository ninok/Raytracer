#pragma once

#include "BBox.h"

class Octree
{


public:
    

    Octree (const BBox& rBBox, size_t nMaxPointsPerCell);
    ~Octree();


    size_t getDepth() const {return m_nDepth;}

    
    void insertTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
    
//    void insertQuad(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& v4);

    
private:

    class Node
    {

    public:
        Node(float fSize, const BBox& rBBox): m_fSize(fSize), m_aBBox(rBBox), m_pParent(NULL), m_aObjects(NULL)
        {

        }
        ~Node()
        {
        }

//        bool isLeaf() {return m_pLeft == NULL && m_pRight == NULL;}

        float                   m_fSize;
        const BBox              m_aBBox;
        Node*                   m_pParent;
        boost::array<Node*, 8>  m_aChildren;
    
        std::vector<size_t> m_aObjects;
        
    private:
        Node(Node&):m_aBBox(BBox(glm::vec3(0.0f), glm::vec3(0.0f))) {}
        Node& operator& (Node&){return *this;}
    };


    Node*                   m_pRoot;
    const size_t            m_nMaxPointsPerCell;
    size_t                  m_nDepth;

    const BBox              m_aBBox;
    
    friend class OctreeDebugger;
};