//#include <string.h>
//#include "read_ply.h"
//#include "build_octree.h"
//#include "write_vtk.h"
#include "mesh.h"


int main(int args, char* argv[])
{
	/*
	if(args!=4)
	{
	    printf("Projection program need 3 parameters.\n");
		printf("Projection: projection.\n");
		printf("input_file: .ply.\n");
		printf("output_file: .txt.\n");
	}
	
	char* fin = argv[1];
	char* fout = argv[2];
	char* expection = argv[3];
	if(strstr(fin, ".ply") == NULL && strstr(fout, ".txt") == NULL && strstr(expection, ".stl") == NULL)
	{
		printf("wrong input or output format. Quiting.\n");
		exit(1);
	}

	VerArray ver_array_;
	Size size_;
	readPly(fin, ver_array_, size_);

	Node origin_node;
	vector<size_t> v;  //trinangle for stl show
	buildOctree(origin_node, ver_array_, size_, &v);

	if (v.size() != 0) {
		FILE* fout1 = fopen(expection, "w");
		fprintf(fout1, "soild\n");
		for (size_t i = 0; i < v.size(); i++) {	//第几个三角形
			float p[3][3];
			for (size_t j = 0; j < 3; j++) {	//第几个点
				p[j][0] = ver_array_.vec_vertex[3 * (ver_array_.vec_vertex_of_face[3 * i] + j)];
				p[j][1] = ver_array_.vec_vertex[3 * (ver_array_.vec_vertex_of_face[3 * i] + j) + 1];
				p[j][3] = ver_array_.vec_vertex[3 * (ver_array_.vec_vertex_of_face[3 * i] + j) + 2];
			}
			float v[3];
			crossProductOfFace(p[0], p[1], p[2], v);
			fprintf(fout1, "  facet normal %g %g %g\n", v[0], v[1], v[2]);
			fprintf(fout1, "    outer loop\n");
			fprintf(fout1, "      vertex %g %g %g\n", p[0][0], p[0][1], p[0][2]);
			fprintf(fout1, "      vertex %g %g %g\n", p[1][0], p[1][1], p[1][2]);
			fprintf(fout1, "      vertex %g %g %g\n", p[2][0], p[2][1], p[2][2]);
			fprintf(fout1, "    endloop\n");
			fprintf(fout1, "  endfacet\n");
		}
		fprintf(fout1, "endsoild\n");
	}

	Record record;
	writeRecord(&origin_node, record);
	writeOut(fout, record);

	freeOctree(origin_node);
	*/

	std::printf("number of parameters: %d\n", args);
	for (int i = 0; i < args; i++) {
		std::printf("%s\n", argv[i]);
	}

	Size s;
	std::vector<float> record;
	read_stl(argv[1], true, &record, &s);

	Node n;
	build(&n, &record, &s);

	write_tecplot(argv[2], &n);

	free(&n);
}