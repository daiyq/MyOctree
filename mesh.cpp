#include "mesh.h"

#include <xtstl.h>
#include <string.h>
#include <xtlocalh.h>
#include <memory.h>
#include <xtstlmesh.h>

#include <cstdio> //fopen, fclose, printf, fread, sscanf
#include <cstring> //strtok, 
#include <cmath>
#include <map>
#include <vector>
#include <queue>
#include <stack>


//#include <cstdlib>


//read .stl file

float max(float t1, float t2) {
	if (t1 >= t2)
		return t1;
	return t2;
}

float max(float t1, float t2, float t3) {
	if (t1 >= t2 && t1 >= t3)
		return t1;
	if (t2 >= t1 && t2 >= t3)
		return t2;
	return t3;
}

float max(float t1, float t2, float t3, float t4) {
	if (t1 >= t2 && t1 >= t3 && t1 >= t4)
		return t1;
	if (t2 >= t1 && t2 >= t3 && t2 >= t4)
		return t2;
	if (t3 >= t1 && t3 >= t2 && t3 >= t4)
		return t3;
	return t4;
}

float min(float t1, float t2) {
	if (t1 <= t2)
		return t1;
	return t2;
}

float min(float t1, float t2, float t3) {
	if (t1 <= t2 && t1 <= t3)
		return t1;
	if (t2 <= t1 && t2 <= t3)
		return t2;
	return t3;
}

float min(float t1, float t2, float t3, float t4) {
	if (t1 <= t2 && t1 <= t3 && t1 <= t4)
		return t1;
	if (t2 <= t1 && t2 <= t3 && t2 <= t4)
		return t2;
	if (t3 <= t1 && t3 <= t2 && t3 <= t4)
		return t3;
	return t4;
}

void compare_point(float triangle[3], Size* s) {
	for (size_t i = 0; i < 3; i++) {
		if (s->min[i] > triangle[i])
			s->min[i] = triangle[i];
	}
	for (size_t i = 0; i < 3; i++) {
		if (s->max[i] < triangle[i])
			s->max[i] = triangle[i];
	}
}

void compare_triangle(float triangle[12], Size* s) {
	for (size_t i = 0; i < 3; i++) {
		if (s->min[i] > min(triangle[i + 3], triangle[i + 6], triangle[i + 9]))
			s->min[i] = min(triangle[i + 3], triangle[i + 6], triangle[i + 9]);
	}
	for (size_t i = 0; i < 3; i++) {
		if (s->max[i] < max(triangle[i + 3], triangle[i + 6], triangle[i + 9]))
			s->max[i] = max(triangle[i + 3], triangle[i + 6], triangle[i + 9]);
	}
}

void read_stl_ascii(char* filename, std::vector<float>* record, Size* s) {
	FILE* fin = std::fopen(filename, "r");
	if (!fin)
		printf("cann't open file %s/n", filename);

	float normal[3];
	float vertex[3];
	bool is_initilize = false;
	char seps[] = " ,\t\n\r";
	while (1) {
		char line[1024];
		fgets(line, 1024, fin);

		char* token = std::strtok(line, seps);

		if (!std::strcmp(token, "solid")) {
			continue;
		}
		else if (!std::strcmp(token, "endsolid")) {
			break;
		}
		else if (!std::strcmp(token, "facet")) {
			token = std::strtok(NULL, seps);
			if (!std::strcmp(token, "normal")) {
				//store 
				for (size_t i = 0; i < 3; i++) {
					token = std::strtok(NULL, seps);
					std::sscanf(token, "%f", &(normal[i]));
				}
				for (size_t i = 0; i < 3; i++) {
					record->push_back(normal[i]);
				}
			}
		}
		else if (!std::strcmp(token, "endfacet")) {
			continue;
		}
		else if (!std::strcmp(token, "outer")) {
			/*
			token = std::strtok(NULL, seps);
			if (std::strcmp(token, "loop")) {
			continue;
			}
			*/
			continue;
		}
		else if (!std::strcmp(token, "endloop")) {
			continue;
		}
		else if (!std::strcmp(token, "vertex")) {
			//store 
			for (size_t i = 0; i < 3; i++) {
				token = std::strtok(NULL, seps);
				std::sscanf(token, "%f", &(vertex[i]));
			}
			for (size_t i = 0; i < 3; i++) {
				record->push_back(vertex[i]);
			}
			if (!is_initilize) {
				s->min[0] = s->max[0] = vertex[0];
				s->min[1] = s->max[1] = vertex[1];
				s->min[2] = s->max[2] = vertex[2];
				is_initilize = true;
			}
			compare_point(vertex, s);
		}

		}//while
		
	std::fclose(fin);
}

void read_stl_binary(char* filename, std::vector<float>* record, Size* s) {
	FILE* fin = std::fopen(filename, "rb");
	if (!fin)
		printf("cann't open file %s/n", filename);

	char header[80];
	fread(header, sizeof(char), 80, fin);
	int count;
	fread(&count, sizeof(int), 1, fin);

	char tem[2];
	float vertex[12];
	bool is_initilize = false;
	size_t count_t = static_cast<size_t>(count);
	for (size_t i = 0; i < count_t; i++) {
		std::fread(vertex, sizeof(float), 12, fin);
		std::fread(tem, sizeof(char), 2, fin);
		for (size_t j = 0; j < 12; j++) {
			record->push_back(vertex[j]);
		}
		if (!is_initilize) {
			s->min[0] = min(vertex[12 * i + 3], vertex[12 * i + 6], vertex[12 * i + 9]);
			s->min[1] = min(vertex[12 * i + 4], vertex[12 * i + 7], vertex[12 * i + 10]);
			s->min[2] = min(vertex[12 * i + 5], vertex[12 * i + 8], vertex[12 * i + 11]);
			s->max[0] = max(vertex[12 * i + 3], vertex[12 * i + 6], vertex[12 * i + 9]);
			s->max[1] = max(vertex[12 * i + 4], vertex[12 * i + 7], vertex[12 * i + 10]);
			s->max[2] = max(vertex[12 * i + 5], vertex[12 * i + 8], vertex[12 * i + 11]);
			is_initilize = true;
		}
		compare_triangle(vertex, s);
	}

	std::fclose(fin);
}

void read_stl(char* filename, bool is_ascii, std::vector<float>* record, Size* s) {
	if (is_ascii)
		read_stl_ascii(filename, record, s);
	else
		read_stl_binary(filename, record, s);
}

void test_size_of_Node() {

	float i[3] = { 1,2,3 };
	std::printf("size of float array: %d\n", sizeof(i));//12
	std::printf("size of float: %d\n", sizeof(i[1]));//4

	std::unique_ptr<Node> un(new Node);
	std::printf("size of unique_ptr: %d\n", sizeof(un));//4
	Node* raw_ptr[8];
	for (size_t i = 0; i < 8; i++) {
		raw_ptr[i] = new Node;
	}
	std::printf("size of raw_ptr: %d\n", sizeof(raw_ptr[0]));//4
	std::printf("size of raw_ptr array: %d\n", sizeof(raw_ptr));//32

	for (size_t i = 0; i < 8; i++) {
		//raw_ptr[i] = new Node;
		delete raw_ptr[i];
	}


	std::vector<size_t> count;
	std::printf("size of vector: %d\n", sizeof(count));//16
	float j = i[1];

}

#ifndef RAW_PTR_NODE_

//build octree
bool has_public_point(float p1[12], float p2[12]) {
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			if (p1[3 + i * 3] == p2[3 + j * 3] && p1[3 + i * 3 + 1] == p2[3 + j * 3 + 1] && p1[3 + i * 3 + 2] == p2[3 + j * 3 + 2])
				return true;
}
	}
	return false;
}

size_t count_public_point(const unique_ptr<Node>& n, std::vector<float>* record) {
	size_t count = 0;
	for (size_t i = 1; i < n->count.size(); i++) {
		if (has_public_point(&((*record)[n->count[0]]), &((*record)[12 * n->count[i]])))
			count++;
	}
	return count;
}

bool has_cos_equal(float p1[12], float p2[12]) {
	float cos = (p1[0] * p2[0] + p1[1] * p2[1] + p1[2] * p2[2]) / (std::sqrt(std::pow(p1[0], 2) + std::pow(p1[1], 2) + std::pow(p1[2], 2))*std::sqrt(std::pow(p2[0], 2) + std::pow(p2[1], 2) + std::pow(p2[2], 2)));
	cos = std::abs(cos);
	if (cos < 0.95)
		return false;
	return true;
}

size_t count_cos_equal(const unique_ptr<Node>& n, std::vector<float>* record) {
	size_t count = 0;
	for (size_t i = 1; i < n->count.size(); i++) {
		if (has_cos_equal(&((*record)[n->count[0]]), &((*record)[12 * n->count[i]])))
			count++;
	}
	return count;
}

void make_child_null(unique_ptr<Node>& n) {
	for (size_t i = 0; i < 8; i++) {
		//n->child[i] = unique_ptr<Node>(new Node);
		n->child[i] = nullptr;
	}
}

void make_child_new(unique_ptr<Node>& n) {
	for (size_t i = 0; i < 8; i++) {
		n->child[i] = unique_ptr<Node>(new Node);
		n->child[i]->len = n->len / 2;
	}
	n->child[0]->points[0] = n->points[0];
	n->child[0]->points[1] = n->points[1];
	n->child[0]->points[2] = n->points[2];
	n->child[1]->points[0] = n->points[0] + n->len / 2;
	n->child[1]->points[1] = n->points[1];
	n->child[1]->points[2] = n->points[2];
	n->child[2]->points[0] = n->points[0] + n->len / 2;
	n->child[2]->points[1] = n->points[1] + n->len / 2;
	n->child[2]->points[2] = n->points[2];
	n->child[3]->points[0] = n->points[0];
	n->child[3]->points[1] = n->points[1] + n->len / 2;
	n->child[3]->points[2] = n->points[2];
	n->child[4]->points[0] = n->points[0];
	n->child[4]->points[1] = n->points[1];
	n->child[4]->points[2] = n->points[2] + n->len / 2;
	n->child[5]->points[0] = n->points[0] + n->len / 2;
	n->child[5]->points[1] = n->points[1];
	n->child[5]->points[2] = n->points[2] + n->len / 2;
	n->child[6]->points[0] = n->points[0] + n->len / 2;
	n->child[6]->points[1] = n->points[1] + n->len / 2;
	n->child[6]->points[2] = n->points[2] + n->len / 2;
	n->child[7]->points[0] = n->points[0];
	n->child[7]->points[1] = n->points[1] + n->len / 2;
	n->child[7]->points[2] = n->points[2] + n->len / 2;
}

bool is_point_in_square(float square[4][2], float tri[2]) {
	float min_x = min(square[0][0], square[1][0], square[2][0], square[3][0]);
	float max_x = max(square[0][0], square[1][0], square[2][0], square[3][0]);
	float min_y = min(square[0][1], square[1][1], square[2][1], square[3][1]);
	float max_y = max(square[0][1], square[1][1], square[2][1], square[3][1]);
	if (tri[0] > min_x&&tri[0]<max_x&&tri[1]>min_y&&tri[1] < max_y)
		return true;
	return false;
}

bool is_line_crossed(float s_line[2][2], float t_line[2][2]) {
	float s_min_x = min(s_line[0][0], s_line[1][0]);
	float s_max_x = max(s_line[0][0], s_line[1][0]);
	float s_min_y = min(s_line[0][1], s_line[1][1]);
	float s_max_y = max(s_line[0][1], s_line[1][1]);

	float t_min_x = min(t_line[0][0], t_line[1][0]);
	float t_max_x = max(t_line[0][0], t_line[1][0]);
	float t_min_y = min(t_line[0][1], t_line[1][1]);
	float t_max_y = max(t_line[0][1], t_line[1][1]);

	if (t_line[0][0] == t_line[1][0] && t_line[1][0] == t_line[1][1]) {
		return false;
	}

	if (s_line[0][0] == s_line[1][0]) {
		if (t_line[0][0] == t_line[1][0]) {
			return false;
		}
		else {
			if (s_line[0][0] > t_min_x && s_line[0][0] < t_max_x) {
				float y = (t_line[1][1] - t_line[0][1])*(s_line[0][0] - t_line[0][0]) / (t_line[1][0] - t_line[0][0]) + t_line[0][1];
				if (y > s_min_y && y < s_max_y)
					return true;
			}
		}
		return false;
	}

	if (s_line[0][1] == s_line[1][1]) {
		if (t_line[0][1] == t_line[1][1]) {
			return false;
		}
		else {
			if (s_line[0][1] > t_min_y && s_line[0][1] < t_max_y) {
				if (t_line[0][0] == t_line[1][0]) {
					if (t_line[0][0] > s_min_x&&t_line[0][0] < s_max_x)
						return true;
					return false;
				}
				float x = (t_line[1][0] - t_line[0][0])*(s_line[0][1] - t_line[0][1]) / (t_line[1][1] - t_line[0][1]) + t_line[0][0];
				if (x > t_min_x && x < t_max_x)
					return true;
			}
		}
		return false;
	}
}

bool is_crossed(float square[4][2], float tri[3][2]) {
	if (is_point_in_square(square, tri[0]) || is_point_in_square(square, tri[1]) || is_point_in_square(square, tri[2]))
		return true;

	float s_line[4][2][2];
	float t_line[3][2][2];
	for (size_t i = 0; i < 4; i++) {
		s_line[i][0][0] = square[i][0];
		s_line[i][0][1] = square[i][1];
		s_line[i][1][0] = square[(i + 1) % 4][0];
		s_line[i][1][1] = square[(i + 1) % 4][1];
	}
	for (size_t i = 0; i < 3; i++) {
		s_line[i][0][0] = square[i][0];
		s_line[i][0][1] = square[i][1];
		s_line[i][1][0] = square[(i + 1) % 3][0];
		s_line[i][1][1] = square[(i + 1) % 3][1];
	}

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 3; j++) {
			if (is_line_crossed(s_line[i], t_line[j]))
				return true;
		}
	}
	return false;
}

bool projection(float cube_point[3], float cube_len, float triangle[12]) {
	float square[3][4][2];
	float tri[3][3][2];

	square[0][0][0] = cube_point[0];
	square[0][0][1] = cube_point[1];
	square[0][1][0] = cube_point[0] + cube_len;
	square[0][1][1] = cube_point[1];
	square[0][2][0] = cube_point[0] + cube_len;
	square[0][2][1] = cube_point[1] + cube_len;
	square[0][3][0] = cube_point[0];
	square[0][3][1] = cube_point[1] + cube_len;

	square[1][0][0] = cube_point[1];
	square[1][0][1] = cube_point[2];
	square[1][1][0] = cube_point[1] + cube_len;
	square[1][1][1] = cube_point[2];
	square[1][2][0] = cube_point[1] + cube_len;
	square[1][2][1] = cube_point[2] + cube_len;
	square[1][3][0] = cube_point[1];
	square[1][3][1] = cube_point[2] + cube_len;

	square[2][0][0] = cube_point[0];
	square[2][0][1] = cube_point[2];
	square[2][1][0] = cube_point[0] + cube_len;
	square[2][1][1] = cube_point[2];
	square[2][2][0] = cube_point[0] + cube_len;
	square[2][2][1] = cube_point[2] + cube_len;
	square[2][3][0] = cube_point[0];
	square[2][3][1] = cube_point[2] + cube_len;

	tri[0][0][0] = triangle[3];
	tri[0][0][1] = triangle[4];
	tri[0][1][0] = triangle[6];
	tri[0][1][1] = triangle[7];
	tri[0][2][0] = triangle[9];
	tri[0][2][1] = triangle[10];

	tri[1][0][0] = triangle[4];
	tri[1][0][1] = triangle[5];
	tri[1][1][0] = triangle[7];
	tri[1][1][1] = triangle[8];
	tri[1][2][0] = triangle[10];
	tri[1][2][1] = triangle[11];

	tri[2][0][0] = triangle[3];
	tri[2][0][1] = triangle[5];
	tri[2][1][0] = triangle[6];
	tri[2][1][1] = triangle[8];
	tri[2][2][0] = triangle[9];
	tri[2][2][1] = triangle[11];

	if (!is_crossed(square[0], tri[0]))
		return false;
	if (!is_crossed(square[1], tri[1]))
		return false;
	if (!is_crossed(square[2], tri[2]))
		return false;
	return true;
}

void spilt_to_child(unique_ptr<Node>& n, std::vector<float>* record) {
	for (size_t i = 0; i < n->count.size(); i++) {
		for (size_t j = 0; j < 8; j++) {
			if (projection(n->child[j]->points, n->child[j]->len, &((*record)[12 * n->count[i]])))
				n->child[j]->count.push_back(n->count[i]);
		}
	}
}

void octree(unique_ptr<Node>& n, std::vector<float>* record) {
	if (n->count.size() < 5) {
		make_child_null(n);
		return;
	}

	size_t count1 = count_cos_equal(n, record);
	if (count1 == record->size() - 1) {
		make_child_null(n);
		return;
	}

	size_t count2 = count_public_point(n, record);
	if (count2 == record->size() - 1) {
		make_child_null(n);
		return;
	}

	make_child_new(n);
	spilt_to_child(n, record);
	n->count.clear();
	n->count.shrink_to_fit();

	for (size_t i = 0; i < 8; i++) {
		octree(n->child[i], record);
	}
}

void build_octree(unique_ptr<Node>& n, std::vector<float>* record, Size* s) {
	//initialization
	float length = s->max[0] - s->min[0];
	if (length < (s->max[1] - s->min[1]))
		length = s->max[1] - s->min[1];
	if (length < (s->max[2] - s->min[2]))
		length = s->max[2] - s->min[2];
	n->len = length;

	for (size_t i = 0; i < 3; i++) {
		n->points[i] = s->min[i];
	}
	for (size_t i = 0; i < record->size() / 12; i++) {
		n->count.push_back(i);
	}

	octree(n, record);
}


//output in tecplot

typedef struct {
	float x;
	float y;
	float z;
}Point;

typedef struct {
	int numbering;
	float distance;
}Value;

bool operator<(const Point& lhs, const Point& rhs) {
	if (lhs.x < rhs.x)
		return false;
	else if (lhs.x > rhs.x)
		return true;
	else if (lhs.y < rhs.y)
		return false;
	else if (lhs.y > rhs.y)
		return true;
	else if (lhs.z < rhs.z)
		return false;
	else if (lhs.z > rhs.z)
		return true;

	return false;
}


typedef std::vector<float> vec_point;
typedef std::vector<int> vec_element;
typedef std::map<Point, int> point_container;

void insert_point_and_element(Point& p, point_container* pcontainer, vec_point* ppoint, vec_element* pelement, xtStlMesh* stlmesh) {
	if (pcontainer->find(p) != pcontainer->end()) {
		pelement->push_back((*pcontainer)[p]);
	}
	else {
		int size = static_cast<int>(pcontainer->size());

		(*pcontainer)[p] = size + 1;
		ppoint->push_back(p.x);
		ppoint->push_back(p.y);
		ppoint->push_back(p.z);
		//the distance come from bb_tree
		double pt[3];
		pt[0] = static_cast<double>(p.x);
		pt[1] = static_cast<double>(p.y);
		pt[2] = static_cast<double>(p.z);
		double dist = xt_stlmesh_isosurface(pt, stlmesh);
		float dist1 = static_cast<float>(dist);
		ppoint->push_back(dist1);


		pelement->push_back(size + 1);
	}
}

void traverse(const unique_ptr<Node>& n, point_container* pcontainer, vec_point* ppoint, vec_element* pelement, xtStlMesh* stlmesh) {
	if (n->child[0] == nullptr) {
		Point p;
		//first point
		p.x = n->points[0];
		p.y = n->points[1];
		p.z = n->points[2];
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		//second point,
		p.x += n->len;//2
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		p.y += n->len;//3
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		p.x -= n->len;//4
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		p.y -= n->len;//5
		p.z += n->len;
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		p.x += n->len;//6
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		p.y += n->len;//7
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		p.x -= n->len;//8
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);

		return;
	}
	traverse(n->child[0], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[1], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[2], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[3], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[4], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[5], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[6], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[7], pcontainer, ppoint, pelement, stlmesh);
}

void write_tecplot(char* filename1, char* filename2, const unique_ptr<Node>& n) {
	FILE* fout = std::fopen(filename2, "w");
	if (!fout)
		std::printf("cann't creat file %s/n", filename2);

	point_container container;
	vec_point point;
	vec_element element;
	xtStlMesh* stlmesh = xt_stlmesh_simple_new_with_bbtree(filename1, 0);
	traverse(n, &container, &point, &element, stlmesh);

	xt_stlmesh_destroy(stlmesh);

	std::fprintf(fout, "TITLE=\"finite element grid\"\n");
	std::fprintf(fout, "VARIABLES=\'X\', \'Y\', \'Z\', \'distance\'\n");
	//std::fprintf(fout, "VARIABLES=\"X\", \"Y\", \"Z\", \"DISTANCE\" \n");
	int number_of_point = static_cast<int>(point.size()) / 4;
	int number_of_element = static_cast<int>(element.size()) / 8;
	std::fprintf(fout, "ZONE N=%d E=%d F=FEPOINT, ET=BRICK\n", number_of_point, number_of_element);

	for (int i = 0; i < number_of_point; i++) {
		std::fprintf(fout, "%f %f %f %f\n", point[4 * i], point[4 * i + 1], point[4 * i + 2], point[4 * i + 3]);
	}
	for (int i = 0; i < number_of_element; i++) {
		std::fprintf(fout, "%d %d %d %d %d %d %d %d\n", element[8 * i], element[8 * i + 1], element[8 * i + 2], element[8 * i + 3], element[8 * i + 4], element[8 * i + 5], element[8 * i + 6], element[8 * i + 7]);
	}

	std::fclose(fout);
}

void wrapper(char* argv1, char* argv2) {
	Size s;
	std::vector<float> record;
	read_stl(argv1, true, &record, &s);

	//Node n;
	unique_ptr<Node> n = unique_ptr<Node>(new Node);
	build_octree(n, &record, &s);

	write_tecplot(argv1, argv2, n);

}


#else

//build octree
bool has_public_point(float p1[12], float p2[12]) {
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			if (p1[3 + i * 3] == p2[3 + j * 3] && p1[3 + i * 3 + 1] == p2[3 + j * 3 + 1] && p1[3 + i * 3 + 2] == p2[3 + j * 3 + 2])
				return true;
		}
	}
	return false;
}

size_t count_public_point(const Node* n, std::vector<float>* record) {
	size_t count = 0;
	for (size_t i = 1; i < n->count.size(); i++) {
		if (has_public_point(&((*record)[n->count[0]]), &((*record)[12 * n->count[i]])))
			count++;
	}
	return count;
}

bool has_cos_equal(float p1[12], float p2[12]) {
	float cos = (p1[0] * p2[0] + p1[1] * p2[1] + p1[2] * p2[2]) / (std::sqrt(std::pow(p1[0], 2) + std::pow(p1[1], 2) + std::pow(p1[2], 2))*std::sqrt(std::pow(p2[0], 2) + std::pow(p2[1], 2) + std::pow(p2[2], 2)));
	cos = std::abs(cos);
	if (cos < 0.95)
		return false;
	return true;
}

size_t count_cos_equal(const Node* n, std::vector<float>* record) {
	size_t count = 0;
	for (size_t i = 1; i < n->count.size(); i++) {
		if (has_cos_equal(&((*record)[n->count[0]]), &((*record)[12 * n->count[i]])))
			count++;
	}
	return count;
}

void make_child_null(Node* n) {
	for (size_t i = 0; i < 8; i++) {
		//n->child[i] = unique_ptr<Node>(new Node);
		n->child[i] = nullptr;
	}
}

void make_child_new(Node* n) {
	for (size_t i = 0; i < 8; i++) {
		n->child[i] = new Node;
		n->child[i]->len = n->len / 2;
		make_child_null(n->child[i]);
	}
	n->child[0]->points[0] = n->points[0];
	n->child[0]->points[1] = n->points[1];
	n->child[0]->points[2] = n->points[2];
	n->child[0]->mark = n->mark * 8 + 1;

	n->child[1]->points[0] = n->points[0] + n->len / 2;
	n->child[1]->points[1] = n->points[1];
	n->child[1]->points[2] = n->points[2];
	n->child[1]->mark = n->mark * 8 + 2;

	n->child[2]->points[0] = n->points[0] + n->len / 2;
	n->child[2]->points[1] = n->points[1] + n->len / 2;
	n->child[2]->points[2] = n->points[2];
	n->child[2]->mark = n->mark * 8 + 3;

	n->child[3]->points[0] = n->points[0];
	n->child[3]->points[1] = n->points[1] + n->len / 2;
	n->child[3]->points[2] = n->points[2];
	n->child[3]->mark = n->mark * 8 + 4;

	n->child[4]->points[0] = n->points[0];
	n->child[4]->points[1] = n->points[1];
	n->child[4]->points[2] = n->points[2] + n->len / 2;
	n->child[4]->mark = n->mark * 8 + 5;

	n->child[5]->points[0] = n->points[0] + n->len / 2;
	n->child[5]->points[1] = n->points[1];
	n->child[5]->points[2] = n->points[2] + n->len / 2;
	n->child[5]->mark = n->mark * 8 + 6;

	n->child[6]->points[0] = n->points[0] + n->len / 2;
	n->child[6]->points[1] = n->points[1] + n->len / 2;
	n->child[6]->points[2] = n->points[2] + n->len / 2;
	n->child[6]->mark = n->mark * 8 + 7;

	n->child[7]->points[0] = n->points[0];
	n->child[7]->points[1] = n->points[1] + n->len / 2;
	n->child[7]->points[2] = n->points[2] + n->len / 2;
	n->child[7]->mark = n->mark * 8 + 8;
}

bool is_point_in_square(float square[4][2], float tri[2]) {
	float min_x = min(square[0][0], square[1][0], square[2][0], square[3][0]);
	float max_x = max(square[0][0], square[1][0], square[2][0], square[3][0]);
	float min_y = min(square[0][1], square[1][1], square[2][1], square[3][1]);
	float max_y = max(square[0][1], square[1][1], square[2][1], square[3][1]);
	if (tri[0] > min_x&&tri[0]<max_x&&tri[1]>min_y&&tri[1] < max_y)
		return true;
	return false;
}

bool is_line_crossed(float s_line[2][2], float t_line[2][2]) {
	float s_min_x = min(s_line[0][0], s_line[1][0]);
	float s_max_x = max(s_line[0][0], s_line[1][0]);
	float s_min_y = min(s_line[0][1], s_line[1][1]);
	float s_max_y = max(s_line[0][1], s_line[1][1]);

	float t_min_x = min(t_line[0][0], t_line[1][0]);
	float t_max_x = max(t_line[0][0], t_line[1][0]);
	float t_min_y = min(t_line[0][1], t_line[1][1]);
	float t_max_y = max(t_line[0][1], t_line[1][1]);

	if (t_line[0][0] == t_line[1][0] && t_line[1][0] == t_line[1][1]) {
		return false;
	}

	if (s_line[0][0] == s_line[1][0]) {
		if (t_line[0][0] == t_line[1][0]) {
			return false;
		}
		else {
			if (s_line[0][0] > t_min_x && s_line[0][0] < t_max_x) {
				float y = (t_line[1][1] - t_line[0][1])*(s_line[0][0] - t_line[0][0]) / (t_line[1][0] - t_line[0][0]) + t_line[0][1];
				if (y > s_min_y && y < s_max_y)
					return true;
			}
		}
		return false;
	}

	if (s_line[0][1] == s_line[1][1]) {
		if (t_line[0][1] == t_line[1][1]) {
			return false;
		}
		else {
			if (s_line[0][1] > t_min_y && s_line[0][1] < t_max_y) {
				if (t_line[0][0] == t_line[1][0]) {
					if (t_line[0][0] > s_min_x&&t_line[0][0] < s_max_x)
						return true;
					return false;
				}
				float x = (t_line[1][0] - t_line[0][0])*(s_line[0][1] - t_line[0][1]) / (t_line[1][1] - t_line[0][1]) + t_line[0][0];
				if (x > t_min_x && x < t_max_x)
					return true;
			}
		}
		return false;
	}
}

bool is_crossed(float square[4][2], float tri[3][2]) {
	if (is_point_in_square(square, tri[0]) || is_point_in_square(square, tri[1]) || is_point_in_square(square, tri[2]))
		return true;

	float s_line[4][2][2];
	float t_line[3][2][2];
	for (size_t i = 0; i < 4; i++) {
		s_line[i][0][0] = square[i][0];
		s_line[i][0][1] = square[i][1];
		s_line[i][1][0] = square[(i + 1) % 4][0];
		s_line[i][1][1] = square[(i + 1) % 4][1];
	}
	for (size_t i = 0; i < 3; i++) {
		s_line[i][0][0] = square[i][0];
		s_line[i][0][1] = square[i][1];
		s_line[i][1][0] = square[(i + 1) % 3][0];
		s_line[i][1][1] = square[(i + 1) % 3][1];
	}

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 3; j++) {
			if (is_line_crossed(s_line[i], t_line[j]))
				return true;
		}
	}
	return false;
}

bool projection(float cube_point[3], float cube_len, float triangle[12]) {
	float square[3][4][2];
	float tri[3][3][2];

	square[0][0][0] = cube_point[0];
	square[0][0][1] = cube_point[1];
	square[0][1][0] = cube_point[0] + cube_len;
	square[0][1][1] = cube_point[1];
	square[0][2][0] = cube_point[0] + cube_len;
	square[0][2][1] = cube_point[1] + cube_len;
	square[0][3][0] = cube_point[0];
	square[0][3][1] = cube_point[1] + cube_len;

	square[1][0][0] = cube_point[1];
	square[1][0][1] = cube_point[2];
	square[1][1][0] = cube_point[1] + cube_len;
	square[1][1][1] = cube_point[2];
	square[1][2][0] = cube_point[1] + cube_len;
	square[1][2][1] = cube_point[2] + cube_len;
	square[1][3][0] = cube_point[1];
	square[1][3][1] = cube_point[2] + cube_len;

	square[2][0][0] = cube_point[0];
	square[2][0][1] = cube_point[2];
	square[2][1][0] = cube_point[0] + cube_len;
	square[2][1][1] = cube_point[2];
	square[2][2][0] = cube_point[0] + cube_len;
	square[2][2][1] = cube_point[2] + cube_len;
	square[2][3][0] = cube_point[0];
	square[2][3][1] = cube_point[2] + cube_len;

	tri[0][0][0] = triangle[3];
	tri[0][0][1] = triangle[4];
	tri[0][1][0] = triangle[6];
	tri[0][1][1] = triangle[7];
	tri[0][2][0] = triangle[9];
	tri[0][2][1] = triangle[10];

	tri[1][0][0] = triangle[4];
	tri[1][0][1] = triangle[5];
	tri[1][1][0] = triangle[7];
	tri[1][1][1] = triangle[8];
	tri[1][2][0] = triangle[10];
	tri[1][2][1] = triangle[11];

	tri[2][0][0] = triangle[3];
	tri[2][0][1] = triangle[5];
	tri[2][1][0] = triangle[6];
	tri[2][1][1] = triangle[8];
	tri[2][2][0] = triangle[9];
	tri[2][2][1] = triangle[11];

	if (!is_crossed(square[0], tri[0]))
		return false;
	if (!is_crossed(square[1], tri[1]))
		return false;
	if (!is_crossed(square[2], tri[2]))
		return false;
	return true;
}

void spilt_to_child(Node* n, std::vector<float>* record) {
	for (size_t i = 0; i < n->count.size(); i++) {
		for (size_t j = 0; j < 8; j++) {
			if (projection(n->child[j]->points, n->child[j]->len, &((*record)[12 * n->count[i]])))
				n->child[j]->count.push_back(n->count[i]);
		}
	}
}

void octree(Node* n, std::vector<float>* record) {
	//if the depth is bigger than 10, stop and return
	if (n->mark > 134217728) {
		make_child_null(n);
		return;
	}

	if (n->count.size() < 5) {
		make_child_null(n);
		return;
	}

	size_t count1 = count_cos_equal(n, record);
	if (count1 == record->size() - 1) {
		make_child_null(n);
		return;
	}

	size_t count2 = count_public_point(n, record);
	if (count2 == record->size() - 1) {
		make_child_null(n);
		return;
	}

	make_child_new(n);
	spilt_to_child(n, record);
	n->count.clear();
	n->count.shrink_to_fit();

	for (size_t i = 0; i < 8; i++) {
		octree(n->child[i], record);
	}
}

void build_octree(Node* n, std::vector<float>* record, Size* s) {
	//initialization
	float length = s->max[0] - s->min[0];
	if (length < (s->max[1] - s->min[1]))
		length = s->max[1] - s->min[1];
	if (length < (s->max[2] - s->min[2]))
		length = s->max[2] - s->min[2];
	n->len = length;

	n->mark = 0;

	for (size_t i = 0; i < 3; i++) {
		n->points[i] = s->min[i];
	}
	for (size_t i = 0; i < record->size() / 12; i++) {
		n->count.push_back(i);
	}

	octree(n, record);
}

//smooth the tree
//like AVL tree

//to get the Node* according to mark(m)
Node* get_node(Node* root, size_t m) {
	/*
	if (m == 0) {
		return root;
	}
	*/
	std::stack<size_t> s;
	while (m != 0) {
		if (m % 8 == 0) {
			s.push(7);
			m = m / 8 - 1;
		}
		else {
			s.push(m % 8 - 1);
			m = m / 8;
		}
	}

	while (!s.empty()) {
		root = root->child[s.top()];
		s.pop();
	}

	return root;
}

//if the node is existed, return nullptr
//if not, split to set it, and return the splited node
Node* set_node(Node* root, size_t m) {
	Node* splited_node = nullptr;
	bool splited = false;
	std::stack<size_t> s;
	while (m != 0) {
		if (m % 8 == 0) {
			s.push(7);
			m = m / 8 - 1;
		}
		else {
			s.push(m % 8 - 1);
			m = m / 8;
		}
	}

	while (s.size() > 1) {
		if (root->child[s.top()] == nullptr) {
			make_child_new(root);
			if (!splited) {
				splited_node = root;
			}
			splited = true;
		}
		root = root->child[s.top()];
		s.pop();
	}
	return splited_node;
}

struct Neighbor {
	Node* neighbor[6];
};

size_t direction_neighbor[8][3] = { { 7,5,4 }/*8*/,{ 2,4,5 },{ 1,3,6 },{ 4,2,7 },
									{ 3,1,8 },{ 6,8,1 },{ 5,7,2 },{ 8,6,3 } };

bool cell_neighbor[8][6] = { { true,false,false,true,false,true },//8
							{ true,false,true,false,true,false },//1
							{ false,true,true,false,true,false },//2
							{ false,true,false,true,true,false },//3
							{ true,false,false,true,true,false },//4
							{ true,false,true,false,false,true },//5
							{ false,true,true,false,false,true },//6
							{ false,true,false,true,false,true } };//7		


//find 6 neighbors (in X, Y, Z) of n,
Neighbor find_and_balance(Node* n, Node* root) {	
	Neighbor neighbor;
	std::stack<size_t> marking;

	for (size_t i = 0; i < 6; i++) {
		size_t m = n->mark;
		while (!cell_neighbor[m % 8][i] && m != 0) {
			//m = m / 8;
			//the same depth's mark has been pushed
			if (m % 8 == 0) {
				marking.push(direction_neighbor[m % 8][i / 2]);//7
				m = m / 8 - 1;
			}
			else {
				marking.push(direction_neighbor[m % 8][i / 2]);
				m = m / 8;
			}
		}
		
		//to get the brother mark which marks the brother node
		//brother node means the brother in the same cell
		size_t brother_mark;
		if (m == 0) { 
			//the cell is on the border
			brother_mark = 0;
		}
		else if (m % 8 == 0) {
			brother_mark = m - 8 + direction_neighbor[m % 8][i / 2];
		}
		else {
			brother_mark = (m / 8) * 8 + direction_neighbor[m % 8][i / 2];
		}
		
		size_t depth_from_brother_to_neighbor = marking.size();

		size_t neighbor_mark = brother_mark;
		while (!marking.empty()) {
			neighbor_mark = neighbor_mark * 8 + marking.top();
			marking.pop();
		}

		//if the neighboring node is balanced, make it nullpttr
		//if not, balance it, and return the brother node
		if (brother_mark == 0) {
			neighbor.neighbor[i] = nullptr;
		}
		else if (depth_from_brother_to_neighbor < 2) {
			//in the same depth or its father's depth
			neighbor.neighbor[i] = nullptr;
		}
		else {
			Node* splited_node = set_node(root, neighbor_mark);
			if (splited_node == nullptr) {
				neighbor.neighbor[i] = nullptr;
			}
			else {
				neighbor.neighbor[i] = splited_node;
			}
			
			/*
			//split the node to balance
			Node* tmp = neighbor.neighbor[i];
			if (n->child[0] == nullptr) {
				//split the node to its father's depth
				while (marking.size() > 1) {
					make_child_new(tmp);
					tmp = tmp->child[marking.top()];
					marking.pop();
				}
			}
			else {
			    //split the node to the same depth
				while (!marking.empty()) {
					make_child_new(tmp);
					tmp = tmp->child[marking.top()];
					marking.pop();
				}
			}
			*/
		}
		
	}

	return neighbor;
}

//find neighbors, the tree has been balanced
Neighbor find_neighbor(Node* n, Node* root) {

}

//discard!!
//if the neighbor is balanced( in the same depth or less than 1 level), return true.
//else return false after split the neighbor to balance
/*
bool balance_neighbor(Node* root, Node* n, Node* neighbor) {
	if (root == neighbor) {
		return true;
	}

}
*/

void smooth_tree(Node* root) {
	std::queue<Node*> q;
	//q.push(root);
	for (size_t i = 0; i < 8; i++) {
		q.push(root->child[i]);
	}

	Node* father;
	Neighbor t;
	while (!q.empty()) {
		father = q.front();
		q.pop();

		if (father->child[0] != nullptr) {
			for (size_t i = 0; i < 8; i++) {
				q.push(father->child[i]);
			}
		}

		t = find_and_balance(father, root);
		for (size_t i = 0; i < 6; i++) {
			/*
			if (!balance_neighbor(root, father, t.neighbor[i])) {
				for (size_t j = 0; j < 8; j++) {
					q.push(t.neighbor[i]->child[j]);
				}
			}
			*/
			if (t.neighbor[i] == nullptr) {
			}
			else {
				for (size_t j = 0; j < 8; j++) {
					q.push(t.neighbor[i]->child[j]);
				}
			}
		}

	}
}


//find axis of model according to mesh

//the node n must be leaf node
//to confirm whether the node satisfy that the node contain the axis of the model
bool is_axis(Node* n, xtStlMesh* stlmesh) {
	float points[3][2][3];
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 2; j++) {
			points[i][j][0] = n->points[0];
			points[i][j][1] = n->points[1];
			points[i][j][2] = n->points[2];
		}
	}
	/*
	points[0][0][0] -= n->len / 2;//X
	points[0][1][0] += n->len / 2;
	points[1][0][1] -= n->len / 2;//Y
	points[1][1][1] += n->len / 2;
	points[2][0][2] -= n->len / 2;//Z
	points[2][1][2] += n->len / 2;
	*/
	for (size_t i = 0; i < 3; i++) {
		points[i][0][i] -= n->len / 2;
		points[i][1][i] += n->len / 2;
	}

	for (size_t i = 0; i < 3; i++) {
		
	}

	return false;
}

bool is_axis_x(Node* back, Node* front, xtStlMesh* stlmesh) {

}

bool is_axis_y(Node* left, Node* right, xtStlMesh* stlmesh) {

}

bool is_axis_z(Node* lower, Node* upper, xtStlMesh* stlmesh) {

}

void queue_node(Node* n, std::queue<Node*>* q_ptr) {
	if (n == nullptr)
		return;

	if (n->child[0] == nullptr) {
		q_ptr->push(n);
		return;
	}

	for (size_t i = 0; i < 8; i++) {
		queue_node(n->child[i], q_ptr);
	}
}

void find_axis(Node* n) {
	//TODO:
	xtStlMesh* stlmesh;

	std::queue<Node*> q;
	queue_node(n, &q);

	//the Node* satisfy the axis's condition
	std::queue<Node*> axis;
	//to be make sure according to its neighbor
	//to be used caculating
	std::queue<Node*> tmp;

	//the single cell is on axis
	while (!q.empty()) {
		Node* t = q.front();
		if (is_axis(t, stlmesh)) {
			axis.push(t);
		}
		else {
			tmp.push(t);
		}
	}

	//with the neighbor to be on axis
	while (!tmp.empty()) {

	}
}

//output in tecplot

typedef struct {
	float x;
	float y;
	float z;
}Point;

typedef struct {
	int numbering;
	float distance;
}Value;

bool operator<(const Point& lhs, const Point& rhs) {
	if (lhs.x < rhs.x)
		return false;
	else if (lhs.x > rhs.x)
		return true;
	else if (lhs.y < rhs.y)
		return false;
	else if (lhs.y > rhs.y)
		return true;
	else if (lhs.z < rhs.z)
		return false;
	else if (lhs.z > rhs.z)
		return true;

	return false;
}


typedef std::vector<float> vec_point;
typedef std::vector<int> vec_element;
typedef std::map<Point, int> point_container;

void insert_point_and_element(Point& p, point_container* pcontainer, vec_point* ppoint, vec_element* pelement, xtStlMesh* stlmesh) {
	if (pcontainer->find(p) != pcontainer->end()) {
		pelement->push_back((*pcontainer)[p]);
	}
	else {
		int size = static_cast<int>(pcontainer->size());

		(*pcontainer)[p] = size + 1;
		ppoint->push_back(p.x);
		ppoint->push_back(p.y);
		ppoint->push_back(p.z);
		//the distance come from bb_tree
		double pt[3];
		pt[0] = static_cast<double>(p.x);
		pt[1] = static_cast<double>(p.y);
		pt[2] = static_cast<double>(p.z);
		double dist = xt_stlmesh_isosurface(pt, stlmesh);
		float dist1 = static_cast<float>(dist);
		ppoint->push_back(dist1);


		pelement->push_back(size + 1);
	}
}

void traverse(const Node* n, point_container* pcontainer, vec_point* ppoint, vec_element* pelement, xtStlMesh* stlmesh) {
	if (n->child[0] == nullptr) {
		Point p;
		//first point
		p.x = n->points[0];
		p.y = n->points[1];
		p.z = n->points[2];
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		//second point,
		p.x += n->len;//2
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		p.y += n->len;//3
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		p.x -= n->len;//4
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		p.y -= n->len;//5
		p.z += n->len;
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		p.x += n->len;//6
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		p.y += n->len;//7
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);
		p.x -= n->len;//8
		insert_point_and_element(p, pcontainer, ppoint, pelement, stlmesh);

		return;
	}
	traverse(n->child[0], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[1], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[2], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[3], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[4], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[5], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[6], pcontainer, ppoint, pelement, stlmesh);
	traverse(n->child[7], pcontainer, ppoint, pelement, stlmesh);
}

void write_tecplot(char* filename1, char* filename2, const Node* n) {
	FILE* fout = std::fopen(filename2, "w");
	if (!fout)
		std::printf("cann't creat file %s/n", filename2);

	point_container container;
	vec_point point;
	vec_element element;
	xtStlMesh* stlmesh = xt_stlmesh_simple_new_with_bbtree(filename1, 0);
	traverse(n, &container, &point, &element, stlmesh);

	xt_stlmesh_destroy(stlmesh);

	std::fprintf(fout, "TITLE=\"finite element grid\"\n");
	std::fprintf(fout, "VARIABLES=\'X\', \'Y\', \'Z\', \'distance\'\n");
	//std::fprintf(fout, "VARIABLES=\"X\", \"Y\", \"Z\", \"DISTANCE\" \n");
	int number_of_point = static_cast<int>(point.size()) / 4;
	int number_of_element = static_cast<int>(element.size()) / 8;
	std::fprintf(fout, "ZONE N=%d E=%d F=FEPOINT, ET=BRICK\n", number_of_point, number_of_element);

	for (int i = 0; i < number_of_point; i++) {
		std::fprintf(fout, "%f %f %f %f\n", point[4 * i], point[4 * i + 1], point[4 * i + 2], point[4 * i + 3]);
	}
	for (int i = 0; i < number_of_element; i++) {
		std::fprintf(fout, "%d %d %d %d %d %d %d %d\n", element[8 * i], element[8 * i + 1], element[8 * i + 2], element[8 * i + 3], element[8 * i + 4], element[8 * i + 5], element[8 * i + 6], element[8 * i + 7]);
	}

	std::fclose(fout);
}

void free_tree(Node* n) {

	//recursive
	
	if (n == nullptr)
	return;

	free_tree(n->child[0]);
	free_tree(n->child[1]);
	free_tree(n->child[2]);
	free_tree(n->child[3]);
	free_tree(n->child[4]);
	free_tree(n->child[5]);
	free_tree(n->child[6]);
	free_tree(n->child[7]);
	delete n;
	

	//use a queue
	/*
	std::queue<Node*> q;
	q.push(n);
	Node* father;
	while (!q.empty()) {
		father = q.front();
		q.pop();

		if (father->child[0] != nullptr) {
			for (size_t i = 0; i < 8; i++) {
				q.push(father->child[i]);
			}
		}

		delete father;
	}
	*/
}


void wrapper(char* argv1, char* argv2) {
	Size s;
	std::vector<float> record;
	read_stl(argv1, true, &record, &s);

	//Node n;
	Node* n = new Node;
	build_octree(n, &record, &s);

	smooth_tree(n);

	write_tecplot(argv1, argv2, n);

	free_tree(n);

}



#endif // !RAW_PTR_NODE_




