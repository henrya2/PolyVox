/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*******************************************************************************/

#ifndef __PolyVox_MeshDecimator_H__
#define __PolyVox_MeshDecimator_H__

#include <bitset>

namespace PolyVox
{
	struct InitialVertexMetadata
	{
		list<uint32_t> trianglesUsingVertex;
		Vector3DFloat normal;
		bool isOnRegionEdge;
		bool isOnMaterialEdge;
	};

	struct CurrentVertexMetadata
	{
		list<uint32_t> trianglesUsingVertex;
	};

	struct Triangle
	{
		uint32_t v0;
		uint32_t v1;
		uint32_t v2;
		Vector3DFloat normal;
	};

	template <typename VertexType>
	class MeshDecimator
	{
	public:
		MeshDecimator(SurfaceMesh<VertexType>* pInputMesh/*, SurfaceMesh<PositionMaterial>* pMeshOutput*/);

		void execute();

	private:

		void fillInitialVertexMetadata(std::vector<InitialVertexMetadata>& vecInitialVertexMetadata);
		void fillCurrentVertexMetadata(std::vector<CurrentVertexMetadata>& vecCurrentVertexMetadata);

		void buildTriangles(void);

		bool attemptEdgeCollapse(uint32_t uSrc, uint32_t uDest);

		SurfaceMesh<VertexType>* m_pInputMesh;
		//SurfaceMesh<PositionMaterial>* pMeshOutput;

		void countNoOfNeighboursUsingMaterial(void);
		uint32_t performDecimationPass(float fMinDotProductForCollapse);
		bool isSubset(std::bitset<VF_NO_OF_FLAGS> a, std::bitset<VF_NO_OF_FLAGS> b);

		bool canCollapseEdge(uint32_t uSrc, uint32_t uDest);
		bool canCollapseNormalEdge(uint32_t uSrc, uint32_t uDst);
		bool canCollapseRegionEdge(uint32_t uSrc, uint32_t uDst);
		bool canCollapseMaterialEdge(uint32_t uSrc, uint32_t uDst);
		bool collapseChangesFaceNormals(uint32_t uSrc, uint32_t uDst, float fThreshold);

		//Data structures used during decimation
		std::vector<uint8_t> m_vecNoOfNeighboursUsingMaterial;

		std::vector<bool> vertexLocked;
		std::vector<uint32_t> vertexMapper;

		//vector< list<uint32_t> > trianglesUsingVertexCurrently;

		std::vector<int> vecOfTriCts;
		std::vector<Vector3DFloat> vecOfTriNormals;
		std::vector<Triangle> m_vecTriangles;

		std::vector<InitialVertexMetadata> m_vecInitialVertexMetadata;
		std::vector<CurrentVertexMetadata> m_vecCurrentVertexMetadata;

		float fMinDotProductForCollapse;
	};
}

#include "MeshDecimator.inl"

#endif //__PolyVox_MeshDecimator_H__
