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

void test_size_of_Node();

#define RAW_PTR_NODE_

#ifndef RAW_PTR_NODE_

typedef struct Node Node;
struct Node {
	unique_ptr<Node> child[8];
	std::vector<size_t> count;
	float points[3];
	float len;
};

void build_octree(unique_ptr<Node>& n, std::vector<float>* record, Size* s);

void write_tecplot(char* filename1, char* filename2, const unique_ptr<Node>& n);

void wrapper(char* argv1, char* argv2);

#else

typedef struct Node Node;
struct Node {
	Node* child[8];
	std::vector<size_t> count;
	float points[3];
	float len;
	size_t mark;
};

void build_octree(Node* n, std::vector<float>* record, Size* s);

void smooth_tree(Node* n);

//void write_out(char* inputfile, char* outputfile, const Node* n);
void write_out(char* inputfile, char* outputfile, char* modelfile, char* axisfile, char* testfile, Node* n);

void free_tree(Node* n);

void wrapper(char* argv1, char* argv2, char* argv3, char* argv4, char* argv5);



#endif // !RAW_PTR_NODE_


#endif 
