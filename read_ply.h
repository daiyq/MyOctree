#ifndef READ_PLY_H_
#define READ_PLY_H_

#include <stdio.h>//fopen
#include <string.h>//strcmp
#include <math.h>
#include <stdlib.h>
#include <vector>

using std::vector;


//First step: deal with inputing
const char typeName[11][10] = { "char","uchar","short","ushort","int","uint","float","double", "uint8", "float32", "int32" };
const int typeSize[11] = { 1,1,2,2,4,4,4,8,1,4,4 };
const int totalTypes = 11;
int num_vertex = 0, num_face = 0;


typedef struct
{
	vector<float> vec_vertex;
	vector<int> vec_vertex_of_face;
}VerArray;

typedef struct
{
	float min[3];
	float max[3];
	float len;
}Size;

void flipBits32(void *x)
{
	unsigned char *temp = (unsigned char*)x;
	unsigned char swap;

	swap = temp[0];
	temp[0] = temp[3];
	temp[3] = swap;

	swap = temp[1];
	temp[1] = temp[2];
	temp[2] = swap;
}

void readPly(char* fname, VerArray& ver_array, Size& size)
{
	FILE* fin;
	//float* vertex_array;
	//float* num_vertex_array;
	if (!(fin = fopen(fname, "rb")))
	{
		printf("Unable to open file %s\n", fname);
	}

	char seps[] = " ,\t\n\r";
	seps[5] = 10;
	int mode = 0;
	//int num_vertex =0, num_face = 0;
	int vmode = 0;
	int lines = 0;
	int extra_vertex = 0;
	float min[3], max[3];
	float rawmin[3], rawmax[3];
	float maxsize = 0;


	while (1)
	{
		char header[1024];
		fgets(header, 1024, fin);

		char* token = strtok(header, seps);

		if (!strcmp(token, "end_header"))
		{
			break;
		}
		else if (!strcmp(token, "format"))
		{
			token = strtok(NULL, seps);
			if (!strcmp(token, "ascii"))
			{
				mode = 0;
			}
			else if (!strcmp(token, "binary_big_endian"))
			{
				mode = 2;
			}
			else
			{
				mode = 1;
			}
		}
		else if (!strcmp(token, "element"))
		{
			token = strtok(NULL, seps);
			if (!strcmp(token, "vertex"))
			{
				token = strtok(NULL, seps);
				sscanf(token, "%d", &num_vertex);
				vmode = 1;
			}
			else if (!strcmp(token, "face"))
			{
				token = strtok(NULL, seps);
				sscanf(token, "%d", &num_face);
				vmode = 2;
			}
		}
		else if (!strcmp(token, "property"))
		{
			switch (vmode)
			{
			case 1: // Vertex mode
				if (lines >= 3)
				{
					// Extra storage for each vertex
					token = strtok(NULL, seps);

					for (int i = 0; i < totalTypes; i++)
					{
						if (!strcmp(token, typeName[i]))
						{
							extra_vertex += typeSize[i];
							break;
						}
					}
				}

				lines++;
				break;

			case 2: // Face mode

				break;
			}
		}
	}

	for (size_t i = 0; i < 3 * num_vertex; ++i)
	{
		ver_array.vec_vertex.push_back(0);
	}

	//ver_array.vec_vertex = new float[3*num_vertex];

	//read vertex
	//int i;
	char temp[1024];
	if (mode>0)
	{
		for (size_t i = 0; i < num_vertex; ++i)
		{
			int haveread = 0;
			if ((haveread = fread(&(ver_array.vec_vertex[3 * i]), sizeof(float), 3, fin)) != 3)
			{
				printf("Read Error");
			}
			if (mode == 2)
			{
				for (size_t j = 0; j < 3; ++j)
				{
					flipBits32(&(ver_array.vec_vertex[3 * i + j]));
				}
			}

			fread(temp, 1, extra_vertex, fin);

			if (i == 0)
			{
				for (size_t j = 0; j < 3; j++)
				{
					rawmin[j] = rawmax[j] = ver_array.vec_vertex[3 * i + j];
				}
			}
			else
			{
				for (size_t k = 0; k < 3; k++)
				{
					if (rawmin[k] > ver_array.vec_vertex[3 * i + k])
					{
						rawmin[k] = ver_array.vec_vertex[3 * i + k];
					}
					if (rawmax[k] < ver_array.vec_vertex[3 * i + k])
					{
						rawmax[k] = ver_array.vec_vertex[3 * i + k];
					}
				}
			}

		}
	}
	else
	{
		//ASCII mode
		for (size_t i = 0; i<num_vertex; ++i)
		{
			fgets(temp, 1024, fin);

			char seps[] = " ,\t\n";
			char* token = strtok(temp, seps);

			sscanf(token, "%f", &(ver_array.vec_vertex[3 * i]));
			token = strtok(NULL, seps);
			sscanf(token, "%f", &(ver_array.vec_vertex[3 * i + 1]));
			token = strtok(NULL, seps);
			sscanf(token, "%f", &(ver_array.vec_vertex[3 * i + 2]));

			if (i == 0)
			{
				for (int j = 0; j < 3; j++)
				{
					rawmin[j] = rawmax[j] = ver_array.vec_vertex[3 * i + j];
				}
			}
			else
			{
				for (int k = 0; k < 3; k++)
				{
					if (rawmin[k] > ver_array.vec_vertex[3 * i + k])
					{
						rawmin[k] = ver_array.vec_vertex[3 * i + k];
					}
					if (rawmax[k] < ver_array.vec_vertex[3 * i + k])
					{
						rawmax[k] = ver_array.vec_vertex[3 * i + k];
					}
				}
			}
		}
	}


	//max size of the cube
	maxsize = rawmax[0] - rawmin[0];
	if (rawmax[1] - rawmin[1] > maxsize)
	{
		maxsize = rawmax[1] - rawmin[1];
	}
	if (rawmax[2] - rawmin[2] > maxsize)
	{
		maxsize = rawmax[2] - rawmin[2];
	}

	for (size_t i = 0; i < 3; i++)
	{
		min[i] = (rawmax[i] + rawmin[i]) / 2 - maxsize / 2;
		max[i] = (rawmax[i] + rawmin[i]) / 2 + maxsize / 2;
	}

	size.len = maxsize;
	for (size_t i = 0; i < 3; i++)
	{
		size.min[i] = min[i];
		size.max[i] = max[i];
	}

	//read face, and record points of face
	//ASCII
	for (size_t i = 0; i < 3 * num_face; ++i)
	{
		ver_array.vec_vertex_of_face.push_back(0);
	}

	//ver_array.vec_num_of_face = new int[3*num_face];

	if (mode > 0)
	{
		int num = 0;
		for (size_t i = 0; i < num_face; ++i)
		{
			fread(&num, sizeof(unsigned char), 1, fin);
			fread(&(ver_array.vec_vertex_of_face[num*i]), sizeof(int), num, fin);
			if (mode == 2)
			{
				for (size_t j = 0; j < num; ++j)
				{
					flipBits32(&(ver_array.vec_vertex_of_face[num*i + j]));
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i<num_face; ++i)
		{
			char temp[1024];
			fgets(temp, 1024, fin);

			char seps[] = " ,\t\n";
			char* token = strtok(temp, seps);

			//sscanf( token, "%d", &num ) ;
			//each face has 3 points, so ignore the number(3)
			//token = strtok( NULL, seps ) ;

			token = strtok(NULL, seps);
			sscanf(token, "%d", &(ver_array.vec_vertex_of_face[3 * i]));
			token = strtok(NULL, seps);
			sscanf(token, "%d", &(ver_array.vec_vertex_of_face[3 * i + 1]));
			token = strtok(NULL, seps);
			sscanf(token, "%d", &(ver_array.vec_vertex_of_face[3 * i + 2]));
			//for ( int i = 0 ; i < num ; i ++ )
			//{
			//	token = strtok( NULL, seps ) ;
			//	//sscanf( token, "%d", &(temparray[i]) ) ;
			//}
		}
	}


}

#endif
