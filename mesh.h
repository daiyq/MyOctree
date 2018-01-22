#ifndef MESH_H_
#define MESH_H_
#include <vector>

#include <memory>
using std::unique_ptr;

struct Size {
	float min[3];
	float max[3];
};
void read_stl(char* filename, bool is_ascii, std::vector<float>* record, Size* s);

typedef struct Node Node;

struct Node {
	unique_ptr<Node> child[8];
	std::vector<size_t> count;
	float points[3];
	float len;
};

void test_size_of_Node();

void build(unique_ptr<Node>& n, std::vector<float>* record, Size* s);

void write_tecplot(char* filename1, char* filename2, const unique_ptr<Node>& n);

void wrapper(char* argv1,char* argv2);

//void free_node(Node* n);
#endif 
