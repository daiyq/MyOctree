#ifndef WRITE_VTK_H_
#define WRITE_VTK_H_

#define _CRT_SECURE_NO_WARNINGS
#include <map>
#include <stdio.h>

using std::map;

typedef struct
{
	float p1;
	float p2;
	float p3;
}Point3d;

bool operator<(const Point3d& lhs, const Point3d& rhs)
{
	if (lhs.p1 < rhs.p1)
		return true;
	else if (lhs.p1 > rhs.p1)
		return false;

	if (lhs.p2 < rhs.p2)
		return true;
	else if (lhs.p2 > rhs.p2)
		return false;

	if (lhs.p3 < rhs.p3)
		return true;
	else if (lhs.p3 > rhs.p3)
		return false;

	return false;
}

typedef struct
{
	map<Point3d, int> point;//for test point's existence

	vector<float> vec_point;
	vector<int> vec_face;
}Record;

int inPointsArray(float point[3], vector<float>& vec_vertex)
{
	for (size_t i = 0; i < vec_vertex.size() / 3; ++i)
	{
		if ((point[0] == vec_vertex[3 * i]) && (point[1] == vec_vertex[3 * i + 1]) && (point[2] == vec_vertex[3 * i + 2]))
			return static_cast<int>(i);
	}
	return -1;
}

void writeOut(char* fname, Record& record)
{
	//rewrite for VTK

	FILE* fout = fopen(fname, "w");
	fprintf(fout, "# vtk DataFile Version 4.1\n");
	fprintf(fout, "vtk output\n");
	fprintf(fout, "ASCII\n");
	fprintf(fout, "DATASET UNSTRUCTURED_GRID\n");

	int num_of_vertex = static_cast<int>(record.vec_point.size() / 3);
	fprintf(fout, "POINTS %d float\n", num_of_vertex);

	for (size_t i = 0; i < num_of_vertex; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			fprintf(fout, "%f", record.vec_point[3 * i + j]);
			fprintf(fout, " ");
		}
		fprintf(fout, "\n");
	}

	int num_of_cell = static_cast<int>(record.vec_face.size() / 8);
	fprintf(fout, "CELLS %d %d\n", num_of_cell, 9 * num_of_cell);

	for (size_t i = 0; i < num_of_cell; ++i)
	{
		int num_points = 8;
		//fwrite(&num_points, sizeof(int), 1, fout);
		fprintf(fout, "%d", num_points);
		fprintf(fout, " ");

		for (size_t j = 0; j < 8; ++j)
		{
			fprintf(fout, "%d", record.vec_face[8 * i + j]);
			fprintf(fout, " ");
		}
		fprintf(fout, "\n");
	}

	fprintf(fout, "CELL_types %d\n", num_of_cell);
	for (size_t i = 0; i < num_of_cell; ++i)
	{
		fprintf(fout, "12\n");
	}

}

void writeRecord(Node* node, Record& record)
{
	/*
	if (node->vec_num_triangle.size() == 0)
	{
	return;
	}
	*/
	

	int marked = 0;
	for (size_t i = 0; i < 8; ++i)
	{
		if (node->child_node[i] != nullptr)
		{
			writeRecord(node->child_node[i], record);
		}
		else
		{
			marked++;
		}
	}

	if (marked == 8)
	{
		//write the points and cube

		for (size_t i = 0; i < 8; ++i)
		{
			//int result = inPointsArray(node->points[i], record.vec_point);
			int before = static_cast<int>(record.point.size());
			Point3d p;
			p.p1 = node->points[i][0];
			p.p2 = node->points[i][1];
			p.p3 = node->points[i][2];
			
			if (record.point.find(p) == record.point.end())
			{
				record.point[p] = before;

				record.vec_face.push_back(static_cast<int>(record.vec_point.size() / 3));

				record.vec_point.push_back(node->points[i][0]);
				record.vec_point.push_back(node->points[i][1]);
				record.vec_point.push_back(node->points[i][2]);
			}
			else
			{
				record.vec_face.push_back(record.point[p]);
			}
		}

	}
}

//free memory

void freeOctree(Node &node)
{
	for (size_t i = 0; i < 8; ++i)
	{
		if (node.child_node[i] != nullptr)
		{
			freeOctree(*(node.child_node[i]));
		}
	}

	for (size_t i = 0; i < 8; ++i)
	{
		delete node.child_node[i];
	}

}

int getDepth(Node& node)
{
	int max_depth = 0;
	int depth;
	for (size_t i = 0; i < 8; ++i)
	{
		if (node.child_node[i] == nullptr)
		{
			depth = 0;
		}
		else
		{
			depth = 1 + getDepth(*(node.child_node[i]));
		}

		if (max_depth < depth)
		{
			max_depth = depth;
		}
	}
	return max_depth;
}

float getMinCellSize(Node& origin_node, int depth)
{
	float origin_len = origin_node.points[1][0] - origin_node.points[0][0];
	while (depth >= 0)
	{
		origin_len /= 2;
		depth--;
	}
	return origin_len;
}


//
//Third Step:read the octree, and make outputing
//TODO:need to be modified
/*
void WriteOctree(char* fname)
{
FILE* fout = fopen(fname, "wb");

int sized = (1<<max_depth);
fwrite(&sized, sizeof(int), 1, fout);
WriteOctree(fout, root, max_depth);
printf("Grid dimension: %d\n", sized);

fclose(fout);
}

void WriteOctree(FILE* fout, Node* node, int depth)
{
char type;
if(depth>0)
{
type = 0;
fwrite(&type, sizeof(char), 1, fout);

char sg = (char)getSign(getChild(node, 0), depth-1, 7-GetChildIndex(node, 0));

int t = 0;
for(int i = 0; i < 8; ++i)
{
if(hasChild(node, i))
{
WriteOctree(fout, getChild(node, t), depth-1);
t++;
}
else
{
type = 1;
fwrite(&type, sizeof(char), 1, fout);
fwrite(&sg, sizeof(char), 1, fout);
}
}
}
else
{
type = 2;
fwrite(&type, sizeof(char), 1, fout);
fwrite(&(node[2]), sizeof(UCHAR), 1, fout);
}

}

bool hasChild(Node* node, int i)
{
if(node->child_node[i])
return true;

return false;
}

Node* getChild(Node* node, int i)
{
if(node->child_node[i])
{
return node->child_node[i];
}
else
return NULL;
}

void getSign()
{

}

*/


#endif // !WRITE_VTK_H_

