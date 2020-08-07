#include "mesh.h"

#include <vcg/complex/algorithms/mesh_to_matrix.h>
#include <vcg/complex/allocate.h>
#include <vcg/../wrap/io_trimesh/import_obj.h>

void loadMesh(CMeshO& m, std::string filename){
	vcg::tri::io::ImporterOBJ<CMeshO>::Info oi;
	vcg::tri::io::ImporterOBJ<CMeshO>::Open(m, filename.c_str(), oi);
	vcg::tri::UpdateBounding<CMeshO>::Box(m);
}

void pymeshlab::Mesh::updateBBox(CMeshO& mesh)
{
	vcg::tri::UpdateBounding<CMeshO>::Box(mesh);
}

void pymeshlab::Mesh::updateTopology(CMeshO& mesh)
{
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(mesh);
	vcg::tri::UpdateTopology<CMeshO>::EdgeEdge(mesh);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(mesh);
	vcg::tri::UpdateTopology<CMeshO>::VertexEdge(mesh);
}

void pymeshlab::Mesh::compact(CMeshO& mesh)
{
	vcg::tri::Allocator<CMeshO>::CompactEveryVector(mesh);
}

bool pymeshlab::Mesh::isCompact(const CMeshO& mesh)
{
	return
			mesh.vert.size() == (unsigned int)mesh.VN() &&
			mesh.face.size() == (unsigned int)mesh.FN() &&
			mesh.edge.size() == (unsigned int)mesh.EN();
}

Eigen::MatrixXd pymeshlab::Mesh::vertexMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireCompactness(mesh);

	// create eigen matrix of vertices
	Eigen::MatrixXd vert(mesh.VN(), 3);

	// copy vertices
	for (int i = 0; i < mesh.VN(); i++)
		for (int j = 0; j < 3; j++)
			vert(i,j) = mesh.vert[i].cP()[j];

	return vert;
}

Eigen::MatrixXi pymeshlab::Mesh::faceMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireCompactness(mesh);

	// create eigen matrix of faces
	Eigen::MatrixXi faces(mesh.FN(), 3);

	// copy faces
	for (int i = 0; i < mesh.FN(); i++)
		for (int j = 0; j < 3; j++)
			faces(i,j) = (int)vcg::tri::Index(mesh,mesh.face[i].cV(j));

	return faces;
}

Eigen::MatrixXd pymeshlab::Mesh::vertexNormalMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireCompactness(mesh);

	// create eigen matrix of vertex normals
	Eigen::MatrixXd vertexNormals(mesh.VN(), 3);

	// per vertices normals
	for (int i = 0; i < mesh.VN(); i++)
		for (int j = 0; j < 3; j++)
			vertexNormals(i,j) = mesh.vert[i].cN()[j];

	return vertexNormals;
}

Eigen::MatrixXd pymeshlab::Mesh::faceNormalMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireCompactness(mesh);

	// create eigen matrix of face normals
	Eigen::MatrixXd faceNormals(mesh.FN(), 3);

	// per face normals
	for (int i = 0; i < mesh.FN(); i++)
		for (int j = 0; j < 3; j++)
			faceNormals(i,j) = mesh.face[i].cN()[j];

	return faceNormals;
}

Eigen::VectorXf pymeshlab::Mesh::vertexQualityMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireCompactness(mesh);
	vcg::tri::RequirePerVertexQuality(mesh);

	Eigen::VectorXf qv(mesh.VN());
	for (int i = 0; i < mesh.VN(); i++){
		qv(i) = mesh.vert[i].cQ();
	}
	return qv;
}

Eigen::VectorXf pymeshlab::Mesh::faceQualityMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireCompactness(mesh);
	vcg::tri::RequirePerFaceQuality(mesh);

	Eigen::VectorXf qf(mesh.FN());
	for (int i = 0; i < mesh.FN(); i++){
		qf(i) = mesh.face[i].cQ();
	}
	return qf;
}

Eigen::MatrixXf pymeshlab::Mesh::vertexTexCoordMatrix(const CMeshO& mesh)
{
	Eigen::MatrixXf m;
	vcg::tri::MeshToMatrix<CMeshO>::GetUVData(mesh, m);
	return m;
}

Eigen::MatrixXi pymeshlab::Mesh::faceFaceAdjacency(const CMeshO& mesh)
{
	vcg::tri::RequireCompactness(mesh);
	vcg::tri::RequireFFAdjacency(mesh);

	Eigen::MatrixXi faceFaceMatrix(mesh.FN(),3);

	for (int i = 0; i < mesh.FN(); i++) {
		for (int j = 0; j < 3; j++) {
			auto AdjF= mesh.face[i].cFFp(j);
			if (AdjF==&mesh.face[i])
				faceFaceMatrix(i,j)=-1;
			else
				faceFaceMatrix(i,j)=mesh.face[i].cFFi(j);
		}
	}

	return faceFaceMatrix;
}

pymeshlab::Mesh::Mesh()
{
}