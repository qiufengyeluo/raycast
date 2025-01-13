#ifndef MAGIC_ENGINE_NAVIGATION_DETOUR_NODE_H_H
#define MAGIC_ENGINE_NAVIGATION_DETOUR_NODE_H_H

#define DETOUR_NODE_PARENT_BITS 24
#define DETOUR_NODE_STATE_BITS  2
#define DETOUR_NODE_FLAGS_BITS  3

#include "navmesh.h"

namespace Detour {

    enum NodeFlags {
        NODE_OPEN            = 0x01,
        NODE_CLOSED          = 0x02,
        NODE_PARENT_DETACHED = 0x04, //节点的父节点不相邻。使用raycast找到的
    };

    typedef unsigned short NodeIndex;
    static const NodeIndex NULL_IDX = (NodeIndex) ~ 0;

    struct Node {
        float pos[3];
        float cost;
        float total;
        //========================================================
        unsigned int pidx  : DETOUR_NODE_PARENT_BITS;  //占用 24位
        unsigned int state : DETOUR_NODE_STATE_BITS;   //占用 2位
        unsigned int flags : DETOUR_NODE_FLAGS_BITS;   //占用 3位
        //========================共29位==========================
        PolyRef id;
    };

    static const int MAX_STATES_PER_NODE = 1 << DETOUR_NODE_STATE_BITS;	// numb

    class NodePool {
        public:
            NodePool(int maxNodes, int hashSize);
            ~NodePool();

            void Clear();

            Node *GetNode(PolyRef id, unsigned char state = 0);
            Node *FindNode(PolyRef id, unsigned char state);
            unsigned int FindNodes(PolyRef id, Node **ptrNodes, const int maxNodes);

            inline unsigned int GetNodeIdx(const Node *ptrNode) {
                if (!ptrNode) return 0;
                return (unsigned int)(ptrNode - m_nodes) + 1;
            }

            inline Node* GetNodeAtIdx(unsigned int idx)   {
		        if (!idx) return 0;
		        return &m_nodes[idx - 1];
	        }

            inline const Node* GetNodeAtIdx(unsigned int idx) const {
                if (!idx) return 0;
                return &m_nodes[idx - 1];
            }
            
            inline int GetMemUsed() const
            {
                return sizeof(*this) +
                    sizeof(Node)*m_maxNodes +
                    sizeof(NodeIndex)*m_maxNodes +
                    sizeof(NodeIndex)*m_hashSize;
            }
            
            inline int GetMaxNodes() const { return m_maxNodes; }
            
            inline int GetHashSize() const { return m_hashSize; }
            inline NodeIndex GetFirst(int bucket) const { return m_first[bucket]; }
            inline NodeIndex GetNext(int i) const { return m_next[i]; }
            inline int GetNodeCount() const { return m_nodeCount; }
        private:
            NodePool(const NodePool&);
            NodePool& operator=(const NodePool&);
        private:
            Node*       m_nodes;
            NodeIndex*  m_first;
            NodeIndex*  m_next;
            const int   m_maxNodes;
            const int   m_hashSize;
            int         m_nodeCount;
    };


    class NodeQueue {
    public:
        NodeQueue(int n);
        ~NodeQueue();
        
        inline void Clear() { m_size = 0; }
        
        inline Node* Top() { return m_heap[0]; }
        
        inline Node* Pop() {
            Node* result = m_heap[0];
            m_size--;
            trickleDown(0, m_heap[m_size]);
            return result;
        }
        
        inline void Push(Node* node) {
            m_size++;
            bubbleUp(m_size-1, node);
        }
        
        inline void Modify(Node* node) {
            for (int i = 0; i < m_size; ++i)
            {
                if (m_heap[i] == node)
                {
                    bubbleUp(i, node);
                    return;
                }
            }
        }
        
        inline bool Empty() const { return m_size == 0; }
        
        inline int GetMemUsed() const {
            return sizeof(*this) +
            sizeof(Node*) * (m_capacity + 1);
        }
        
        inline int GetCapacity() const { return m_capacity; }
        
    private:
        // Explicitly disabled copy constructor and copy assignment operator.
        NodeQueue(const NodeQueue&);
        NodeQueue& operator=(const NodeQueue&);

        void bubbleUp(int i,    Node* node);
        void trickleDown(int i, Node* node);
        
        Node**    m_heap;
        const int m_capacity;
        int       m_size;
    };		

}

#endif