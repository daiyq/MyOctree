//#include <string.h>
#include "read_ply.h"
#include "build_octree.h"
#include "write_vtk.h"

int main(int args, char* argv[])
{
	if(args!=3)
	{
		printf("Projection program need 3 parameters.\n");
		printf("Projection: projection.\n");
		printf("input_file: .ply.\n");
		printf("output_file: .txt.\n");
	}
	
	char* fin = argv[1];
	char* fout = argv[2];
	if(strstr(fin, ".ply") == NULL && strstr(fout, ".txt") == NULL)
	{
		printf("wrong input or output format. Quiting.\n");
		exit(1);
	}

	VerArray ver_array_;
	Size size_;
	readPly(fin, ver_array_, size_);

	Node origin_node;
	buildOctree(origin_node, ver_array_, size_);

	Record record;
	writeRecord(&origin_node, record);
	writeOut(fout, record);

	freeOctree(origin_node);

}