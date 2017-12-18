#ifndef MESH_H_
#define MESH_H_
#include <vector>

struct Size {
	float min[3];
	float max[3];
};
void read_stl(char* filename, bool is_ascii, std::vector<float>* record, Size* s);

typedef struct Node Node;

struct Node {
	float points[3];
	float len;
	Node* child[8];
	std::vector<size_t> count;
};
void build(Node* n, std::vector<float>* record, Size* s);

void write_tecplot(char* filename, Node* n);

void free_node(Node* n);
#endif 

