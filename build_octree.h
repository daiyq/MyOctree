#ifndef BUILD_OCTREE_H_
#define BUILD_OCTREE_H_


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <vector>

using std::vector;


typedef struct 
{ 
	float x;
	float y;
	bool isExisted;
}Point;

//define Node
typedef struct OctreeNode Node;

struct OctreeNode 
{
	float points[8][3];
	Node* child_node[8];
	vector<size_t> vec_num_triangle;
};

float Min(float a, float b)
{
	return a > b ? b : a;
}
float Max(float a, float b)
{
	return a > b ? a : b;
}

//假设是与坐标系垂直的正方形
bool inSquare(float (&point)[2], float (&square)[4][2])
{
	float min_x = Min(Min(Min(square[0][0], square[1][0]), square[2][0]), square[3][0]);
	float max_x = Max(Max(Max(square[0][0], square[1][0]), square[2][0]), square[3][0]);
	float min_y = Min(Min(Min(square[0][1], square[1][1]), square[2][1]), square[3][1]);
	float max_y = Max(Max(Max(square[0][1], square[1][1]), square[2][1]), square[3][1]);

	if (point[0]<max_x&&point[0]>min_x&&point[1]<max_y&&point[1]>min_y)
		return true;

	return false;
}

bool inTriangle(float (&point)[2], float (&tri)[3][2])
{
	bool mark[3];
	for (size_t i = 0;i < 3;i++)
	{
		float dotResult = (point[0] - tri[i % 3][0])*(tri[(i + 1) % 3][0] - tri[i % 3][0]) + (point[1] - tri[i % 3][1])*(tri[(i + 1) % 3][1] - tri[i % 3][1]);
		if (dotResult < 0)
			mark[i] = false;
	}
	if ((mark[0] == mark[1]) && (mark[0] == mark[2]))
		return true;

	return false;
}

bool onLine(Point& p, float (&point1)[2], float (&point2)[2]) 
{
	float min_x = Min(point1[0], point2[0]);
	float max_x = Max(point1[0], point2[0]);
	float min_y = Min(point1[1], point2[1]);
	float max_y = Max(point1[1], point2[1]);

	if (point1[0] == point2[0] && point1[0] == p.x) //与y轴平行
	{
		if ((p.y > min_y) && (p.y < max_y))
			return true;
		else
			return false;
	}
	else if (point1[1] == point2[1] && point1[1] == p.y) //与x轴平行
	{
		if ((p.x > min_x) && (p.x < max_y))
			return true;
		else
			return false;
	}
	else
	{
		if ((p.x > min_x) && (p.x < max_x) && (p.y > min_y) && (p.y < max_y))
		{
			if (((point1[1] - p.y) / (point1[0] - p.x)) == ((p.y - point2[1]) / (p.x - point2[0])))
				return true;
		}
	}
	return false;
}

Point crossedPoint(float (&tri_point1)[2], float (&tri_point2)[2], float (&square_point1)[2], float (&square_point2)[2]) 
{
	int state = 0;
	Point p;

	if ((square_point1[0] == square_point2[0]) && (tri_point1[0] == tri_point2[0])) //二直线都没有斜率
		state = 0;
	else if ((square_point1[0] == square_point2[0]) && (tri_point1[0] != tri_point2[0])) //正方形直线没有斜率，三角形的直线有
		state = 1;
	else if ((square_point1[1] == square_point2[1]) && (tri_point1[0] == tri_point2[0])) //正方形有，三角形直线没有
		state = 2;
	 else if ((square_point1[1] == square_point2[1]) && (tri_point1[0] != tri_point2[0])) //均有斜率
		state = 3;
	 else
	 {
		 p.x = p.y = 0;
		 p.isExisted = false; //非合法的正方形线段，有可能是对角线
		 return p;
	 }
	switch (state)
	{
	case 0:
		if ((Min(tri_point1[1], tri_point2[1]) < Max(square_point1[1], square_point2[1])) && (Min(tri_point1[1], tri_point2[1]) > Min(square_point1[1], square_point2[1])) && (tri_point1[0] == square_point1[0]))
		{
			p.x = tri_point1[0];
			p.y = Min(tri_point1[1], tri_point2[1]);
			p.isExisted = true; //两线段共线
			return p;
		}
		else 
		{
			p.x = p.y = 0;
			p.isExisted = false; //两平行线，无交点
			return p;
		}
	case 1:
		p.x = square_point1[0]; //利用两点得直线，求坐标。下同
		p.y = (tri_point2[1] - tri_point1[1])*(p.x - tri_point1[0]) / (tri_point2[0] - tri_point1[0]) + tri_point1[1];
		p.isExisted = true;
		return p;
	case 2:
		p.x = tri_point1[0];
		p.y = square_point1[1];
		p.isExisted = true;
		return p;
	case 3:
		p.y = square_point1[1];
		p.x = (p.y - tri_point1[1])*(tri_point2[0] - tri_point1[0]) / (tri_point2[1] - tri_point1[1]) + tri_point1[0];
		p.isExisted = true;
		return p;
	default:
		break;
	}
}

bool isCrossed(float (&tri)[3][2], float (&square)[4][2])
{
	for (size_t i = 0;i < 4;i++)
	{
		for (size_t j = 0;j < 3;j++)
		{
			Point p = crossedPoint(tri[j % 3], tri[(j + 1) % 3], square[i % 4], square[(i + 1) % 4]);
			if (p.isExisted)
			{
				if (onLine(p, tri[j % 3], tri[(j + 1) % 3]) && onLine(p, square[i % 4], square[(i + 1) % 4]))
					return true; //交点在三角形上，也在正方形上
			}
		}
	}

	//三角形与正方形不相交，有三种情况
	//第一种：三角形在正方形中
	float centrl_tri[2];
	float a[4][2]; //传值。 问题：怎么优化？可以减少数据拷贝??
	for (size_t i = 0;i < 4;i++)
		for (size_t j = 0;j < 2;j++)
			a[i][j] = square[i][j];
	centrl_tri[0] = (tri[0][0] + tri[1][0] + tri[2][0]) / 3;
	centrl_tri[1] = (tri[0][1] + tri[1][1] + tri[2][1]) / 3;
	if (inSquare(centrl_tri, a))
		return true;

	//第二种：正方形在三角形中
	float central_square[2];
	float b[3][2];
	for (size_t i = 0;i < 3;i++)
		for (size_t j = 0;j < 2;j++)
			b[i][j] = tri[i][j];
	central_square[0] = (square[0][0] + square[1][0] + square[2][0] + square[3][0]) / 4;
	central_square[1] = (square[0][1] + square[1][1] + square[2][1] + square[3][1]) / 4;
	if (inTriangle(central_square, b))
		return true;

	//第三种：正方形与三角形不相交
	return false;
}

bool projection(float (&triangle)[3][3], float (&cube)[8][3])
{
	float tri[3][3][2];
	float square[3][4][2];
	/*
	tri[0][0][0] = triangle[0][0];
	tri[0][0][1] = triangle[0][1];
	tri[0][1][0] = triangle[1][0];
	tri[0][1][1] = triangle[1][1];
	tri[0][2][0] = triangle[2][0];
	tri[0][2][1] = triangle[2][1];
	
	tri[1][0][0] = triangle[0][1];
	tri[1][0][1] = triangle[0][2];

	tri[2][0][0] = triangle[0][0];
	tri[2][0][1] = triangle[0][2];
	*/
	for (size_t i = 0;i < 3;i++)          //三角面的投影坐标
	{
		tri[0][i][0] = triangle[i][0]; //投影到xy平面
		tri[0][i][1] = triangle[i][1];
		tri[1][i][0] = triangle[i][1]; //投影到yz平面
		tri[1][i][1] = triangle[i][2];
		tri[2][i][0] = triangle[i][0]; //投影到xz平面
		tri[2][i][1] = triangle[i][2];
	}

	//正方体的投影要充分考虑到接口处定义的八个点的顺序，
	//在这里我假定点的顺序是从右到左，从上到下的顺序进行排列
	for (size_t i = 0;i < 4;i++)   //投影到xy平面
	{
		square[0][i][0] = cube[i][0];
		square[0][i][1] = cube[i][1];
	}
	for (size_t i = 0;i < 2;i++)   //投影到yz平面
	{
		square[1][0][i] = cube[0][i + 1];
		square[1][1][i] = cube[3][i + 1];
		square[1][2][i] = cube[7][i + 1];
		square[1][3][i] = cube[4][i + 1];
	}
	square[2][0][0] = cube[0][0];	//投影到xz平面
	square[2][0][1] = cube[0][2];
	square[2][1][0] = cube[1][0];
	square[2][1][1] = cube[1][2];
	square[2][2][0] = cube[5][0];
	square[2][2][1] = cube[5][2];
	square[2][3][0] = cube[4][0];
	square[2][3][1] = cube[4][2];

	if (isCrossed(tri[0], square[0]) && isCrossed(tri[1], square[1]) && isCrossed(tri[2], square[2]))
		return true;
	else
		return false;

}



bool pointOfTriangle(float point[3], float triangle[3][3]){
	for(size_t i = 0; i < 3; ++i)
	{
		if((point[0] == triangle[i][0]) && (point[1] == triangle[i][1]) && (point[2] == triangle[2][0]))
			return true;
	}
	return false;
}
bool pointOfTriangle(float point[3], float tri_point1[3], float tri_point2[3], float tri_point3[3])
{
	if((point[0] == tri_point1[0]) && (point[1] == tri_point1[1]) && (point[2] == tri_point1[2]))
		return true;
	if((point[0] == tri_point2[0]) && (point[1] == tri_point2[1]) && (point[2] == tri_point2[2]))
		return true;
	if((point[0] == tri_point3[0]) && (point[1] == tri_point3[1]) && (point[2] == tri_point3[2]))
		return true;

	return false;
}
/*
void ProduceChildNode(float (&triangle)[3][3], Node &node, float stop_length) {
	//这里有严重的性能问题，但是一直得不到解决
	//原因在于数组作为参数，是严禁拷贝数组的
	//但是给函数内部的函数传参数时，对数组形参，实参必须是值而非引用
	//导致在这里会有大量拷贝产生
	
	float triangle_[3][3];
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			triangle_[i][j] = triangle[i][j];
	Node node_;
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			node_.points[i][j] = node.points[i][j];
		}
		node_.clild_node[i] = node.clild_node[i];
	}

	float length_side = (node_.points[1][0] - node_.points[0][0]) / 2;//正方体边长
	if (!Projection(triangle, node_.points) || (length_side <= stop_length))	{
		for (int i = 0; i < 8; ++i)			
			node_.clild_node[i] = nullptr;		
	}
	else {	
		(*(node_.clild_node[0])).points[0][0] = node_.points[0][0];
		(*(node_.clild_node[0])).points[0][1] = node_.points[0][1];
		(*(node_.clild_node[0])).points[0][2] = node_.points[0][2]; // 0
		(*(node_.clild_node[1])).points[0][0] = node_.points[0][0] + length_side;
		(*(node_.clild_node[1])).points[0][1] = node_.points[0][1];
		(*(node_.clild_node[1])).points[0][2] = node_.points[0][2]; // 1
		(*(node_.clild_node[2])).points[0][0] = node_.points[0][0] + length_side;
		(*(node_.clild_node[2])).points[0][1] = node_.points[0][1] + length_side;
		(*(node_.clild_node[2])).points[0][2] = node_.points[0][2]; // 2
		(*(node_.clild_node[3])).points[0][0] = node_.points[0][0];
		(*(node_.clild_node[3])).points[0][1] = node_.points[0][1] + length_side;
		(*(node_.clild_node[3])).points[0][2] = node_.points[0][2]; // 3
		(*(node_.clild_node[4])).points[0][0] = node_.points[0][0];
		(*(node_.clild_node[4])).points[0][1] = node_.points[0][1];
		(*(node_.clild_node[4])).points[0][2] = node_.points[0][2] + length_side; // 4
		(*(node_.clild_node[5])).points[0][0] = node_.points[0][0] + length_side;
		(*(node_.clild_node[5])).points[0][1] = node_.points[0][1];
		(*(node_.clild_node[5])).points[0][2] = node_.points[0][2] + length_side; // 5
		(*(node_.clild_node[6])).points[0][0] = node_.points[0][0] + length_side;
		(*(node_.clild_node[6])).points[0][1] = node_.points[0][1] + length_side;
		(*(node_.clild_node[6])).points[0][2] = node_.points[0][2] + length_side; // 6 
		(*(node_.clild_node[7])).points[0][0] = node_.points[0][0];
		(*(node_.clild_node[7])).points[0][1] = node_.points[0][1] + length_side;
		(*(node_.clild_node[7])).points[0][2] = node_.points[0][2] + length_side; // 7

		for (int i = 0; i < 8; ++i) {
			(*(node_.clild_node[i])).points[1][0] = (*(node_.clild_node[i])).points[0][0] + length_side;
			(*(node_.clild_node[i])).points[1][1] = (*(node_.clild_node[i])).points[0][1];
			(*(node_.clild_node[i])).points[1][2] = (*(node_.clild_node[i])).points[0][2];
			(*(node_.clild_node[i])).points[2][0] = (*(node_.clild_node[i])).points[0][0] + length_side;
			(*(node_.clild_node[i])).points[2][1] = (*(node_.clild_node[i])).points[0][1] + length_side;
			(*(node_.clild_node[i])).points[2][2] = (*(node_.clild_node[i])).points[0][2];
			(*(node_.clild_node[i])).points[3][0] = (*(node_.clild_node[i])).points[0][0];
			(*(node_.clild_node[i])).points[3][1] = (*(node_.clild_node[i])).points[0][1] + length_side;
			(*(node_.clild_node[i])).points[3][2] = (*(node_.clild_node[i])).points[0][2];
			(*(node_.clild_node[i])).points[4][0] = (*(node_.clild_node[i])).points[0][0];
			(*(node_.clild_node[i])).points[4][1] = (*(node_.clild_node[i])).points[0][1];
			(*(node_.clild_node[i])).points[4][2] = (*(node_.clild_node[i])).points[0][2] + length_side;
			(*(node_.clild_node[i])).points[5][0] = (*(node_.clild_node[i])).points[0][0] + length_side;
			(*(node_.clild_node[i])).points[5][1] = (*(node_.clild_node[i])).points[0][1];
			(*(node_.clild_node[i])).points[5][2] = (*(node_.clild_node[i])).points[0][2] + length_side;
			(*(node_.clild_node[i])).points[6][0] = (*(node_.clild_node[i])).points[0][0] + length_side;
			(*(node_.clild_node[i])).points[6][1] = (*(node_.clild_node[i])).points[0][1] + length_side;
			(*(node_.clild_node[i])).points[6][2] = (*(node_.clild_node[i])).points[0][2] + length_side;
			(*(node_.clild_node[i])).points[7][0] = (*(node_.clild_node[i])).points[0][0];
			(*(node_.clild_node[i])).points[7][1] = (*(node_.clild_node[i])).points[0][1] + length_side;
			(*(node_.clild_node[i])).points[7][2] = (*(node_.clild_node[i])).points[0][2] + length_side;
		}
		//迭代进行
		for (int i = 0; i < 8; ++i) {
			ProduceChildNode(triangle_, *(node_.clild_node[i]), stop_length);
		}
	}
	


}
*/
//向量叉乘
void crossProduct(float point1[3], float point2[3], float result[3])
{
	result[0] = point1[1] * point2[2] - point1[2] * point2[1];
	result[1] = point1[2] * point2[0] - point1[0] * point2[2];
	result[2] = point1[0] * point2[1] - point1[1] * point2[0];	
}
float pointProduct(float point1[3], float point2[3])
{
	return point1[0]*point2[0]+point1[1]*point2[1]+point1[2]*point2[2];
}

//面法矢
void crossProductOfFace(float point1[3], float point2[3], float point3[3], float vector_of_face[3])
{
	float vector_side1[3], vector_side2[3];
	vector_side1[0] = point2[0] - point1[0];
	vector_side1[1] = point2[1] - point1[1];
	vector_side1[2] = point2[2] - point1[2];

	vector_side2[0] = point3[0] - point2[0];
	vector_side2[1] = point3[1] - point2[1];
	vector_side2[2] = point3[2] - point2[2];

	crossProduct(vector_side1, vector_side2, vector_of_face);
}

void produceChildNode(Node &node, VerArray &ver)
{
	/*
	TODO:when to stop	
	*/
	//test 
	
	if (node.vec_num_triangle.size() < 4)
	{
		//TODO:make child null
		for (size_t i = 0; i<8; ++i)
		{
			node.child_node[i] = nullptr;
		}
		return;
	}
	
	//无三角形或者只有一个三角形
	//条件太过苛刻
	/*
	if((node.vec_num_triangle.size() == 0) || (node.vec_num_triangle.size() == 1))
	{
	//TODO:make child null
	for(size_t i=0;i<8;++i)
	{
	node.child_node[i] = nullptr;
	}
	return;
	}
	*/
	
	//共顶点
	for(size_t i = 0; i < 3; ++i)
	{
		int marked_in = 0;
		for(size_t j = 1; j < node.vec_num_triangle.size(); ++j)
		{
			//if(!pointOfTriangle(g_triangle[node.vec_num_triangle[0]][i], g_triangle[node.vec_num_triangle[j]]))
			if (pointOfTriangle(&ver.vec_vertex[3 * ver.vec_vertex_of_face[3 * node.vec_num_triangle[0] + i]], &ver.vec_vertex[3 * ver.vec_vertex_of_face[3 * node.vec_num_triangle[j] + 0]], &ver.vec_vertex[3 * ver.vec_vertex_of_face[3 * node.vec_num_triangle[j] + 1]], &ver.vec_vertex[3 * ver.vec_vertex_of_face[3 * node.vec_num_triangle[j] + 2]]))
			{
				marked_in++;
			}
			else
			{
				break;
			}												    
		}
		//TODO:make child null
		if (marked_in == node.vec_num_triangle.size() - 1)
		{
			for (size_t i = 0; i<8; ++i)
			{
				node.child_node[i] = nullptr;
			}
			return;
		}		
	}		
	//方向相近??
	float vector_of_face_1[3];
	//crossProductOfFace(g_triangle[node.vec_num_triangle[0]][0],g_triangle[node.vec_num_triangle[0]][1],g_triangle[node.vec_num_triangle[0]][2],vector_of_face_1);
	crossProductOfFace(&ver.vec_vertex[3 * ver.vec_vertex_of_face[3*node.vec_num_triangle[0]]], &ver.vec_vertex[3 * ver.vec_vertex_of_face[3*node.vec_num_triangle[0]+1]], &ver.vec_vertex[3 * ver.vec_vertex_of_face[3*node.vec_num_triangle[0]+2]], vector_of_face_1);
	float vector_of_face_2[3];
	int count_cos_=0;
	for(size_t i = 1; i < node.vec_num_triangle.size(); ++i)
	{
		//crossProductOfFace(g_triangle[node.vec_num_triangle[i]][0],g_triangle[node.vec_num_triangle[i]][1],g_triangle[node.vec_num_triangle[i]][2],vector_of_face_2);
		crossProductOfFace(&ver.vec_vertex[3 * ver.vec_vertex_of_face[3*node.vec_num_triangle[i]]], &ver.vec_vertex[3 * ver.vec_vertex_of_face[3*node.vec_num_triangle[i]+1]], &ver.vec_vertex[3*ver.vec_vertex_of_face[3*node.vec_num_triangle[i]+2]], vector_of_face_2);
		float cos_=pointProduct(vector_of_face_1,vector_of_face_2)/(sqrt(pow(vector_of_face_1[0],2)+pow(vector_of_face_1[1],2)+pow(vector_of_face_1[2],2))*sqrt(pow(vector_of_face_2[0],2)+pow(vector_of_face_2[1],2)+pow(vector_of_face_2[2],2)));
		float cos_abs=abs(cos_);
		if(cos_abs<0.95) break;
		++count_cos_;
	}
	if(count_cos_ == node.vec_num_triangle.size()-1) 
	{
		//TODO:make child null
		for(size_t i=0;i<8;++i)
		{
			node.child_node[i] = nullptr;
		}
		return;
	}


	//排除掉上面不用切分的情况，切分开始

	//allocate memory of childNode
	for(size_t i=0;i<8;++i)
	{
		node.child_node[i] = new Node;
	}


	float half_length_of_side = (node.points[1][0] - node.points[0][0]) / 2;//正方体边长一半

	(node.child_node[0])->points[0][0] = node.points[0][0];
	(node.child_node[0])->points[0][1] = node.points[0][1];
	(node.child_node[0])->points[0][2] = node.points[0][2]; // 0
	(node.child_node[1])->points[0][0] = node.points[0][0] + half_length_of_side;
	(node.child_node[1])->points[0][1] = node.points[0][1];
	(node.child_node[1])->points[0][2] = node.points[0][2]; // 1
	(node.child_node[2])->points[0][0] = node.points[0][0] + half_length_of_side;
	(node.child_node[2])->points[0][1] = node.points[0][1] + half_length_of_side;
	(node.child_node[2])->points[0][2] = node.points[0][2]; // 2
	(node.child_node[3])->points[0][0] = node.points[0][0];
	(node.child_node[3])->points[0][1] = node.points[0][1] + half_length_of_side;
	(node.child_node[3])->points[0][2] = node.points[0][2]; // 3
	(node.child_node[4])->points[0][0] = node.points[0][0];
	(node.child_node[4])->points[0][1] = node.points[0][1];
	(node.child_node[4])->points[0][2] = node.points[0][2] + half_length_of_side; // 4
	(node.child_node[5])->points[0][0] = node.points[0][0] + half_length_of_side;
	(node.child_node[5])->points[0][1] = node.points[0][1];
	(node.child_node[5])->points[0][2] = node.points[0][2] + half_length_of_side; // 5
	(node.child_node[6])->points[0][0] = node.points[0][0] + half_length_of_side;
	(node.child_node[6])->points[0][1] = node.points[0][1] + half_length_of_side;
	(node.child_node[6])->points[0][2] = node.points[0][2] + half_length_of_side; // 6 
	(node.child_node[7])->points[0][0] = node.points[0][0];
	(node.child_node[7])->points[0][1] = node.points[0][1] + half_length_of_side;
	(node.child_node[7])->points[0][2] = node.points[0][2] + half_length_of_side; // 7

	for (size_t i = 0; i < 8; ++i)
	{
		(node.child_node[i])->points[1][0] = ((node.child_node[i]))->points[0][0] + half_length_of_side;
		(node.child_node[i])->points[1][1] = ((node.child_node[i]))->points[0][1];
		(node.child_node[i])->points[1][2] = ((node.child_node[i]))->points[0][2];
		(node.child_node[i])->points[2][0] = ((node.child_node[i]))->points[0][0] + half_length_of_side;
		(node.child_node[i])->points[2][1] = ((node.child_node[i]))->points[0][1] + half_length_of_side;
		(node.child_node[i])->points[2][2] = ((node.child_node[i]))->points[0][2];
		(node.child_node[i])->points[3][0] = ((node.child_node[i]))->points[0][0];
		(node.child_node[i])->points[3][1] = ((node.child_node[i]))->points[0][1] + half_length_of_side;
		(node.child_node[i])->points[3][2] = ((node.child_node[i]))->points[0][2];
		(node.child_node[i])->points[4][0] = ((node.child_node[i]))->points[0][0];
		(node.child_node[i])->points[4][1] = ((node.child_node[i]))->points[0][1];
		(node.child_node[i])->points[4][2] = ((node.child_node[i]))->points[0][2] + half_length_of_side;
		(node.child_node[i])->points[5][0] = ((node.child_node[i]))->points[0][0] + half_length_of_side;
		(node.child_node[i])->points[5][1] = ((node.child_node[i]))->points[0][1];
		(node.child_node[i])->points[5][2] = ((node.child_node[i]))->points[0][2] + half_length_of_side;
		(node.child_node[i])->points[6][0] = ((node.child_node[i]))->points[0][0] + half_length_of_side;
		(node.child_node[i])->points[6][1] = ((node.child_node[i]))->points[0][1] + half_length_of_side;
		(node.child_node[i])->points[6][2] = ((node.child_node[i]))->points[0][2] + half_length_of_side;
		(node.child_node[i])->points[7][0] = ((node.child_node[i]))->points[0][0];
		(node.child_node[i])->points[7][1] = ((node.child_node[i]))->points[0][1] + half_length_of_side;
		(node.child_node[i])->points[7][2] = ((node.child_node[i]))->points[0][2] + half_length_of_side;
	}	
	//for(auto j : node.vec_num_triangle){
	//for(auto j = node.vec_num_triangle.begin(); j != node.vec_num_triangle.end(); ++j)
	for(size_t j = 0; j< node.vec_num_triangle.size(); ++j)
	{
		float tri[3][3];
		for (size_t m = 0; m < 3; ++m)
		{
			for (size_t n = 0; n < 3; ++n)
			{
				tri[m][n] = ver.vec_vertex[3 * ver.vec_vertex_of_face[3 * node.vec_num_triangle[j] + m] + n];
			}
		}

		for(size_t k = 0; k < 8; ++k)
		{					
			if (projection(tri, node.child_node[k]->points))
			{
				(node.child_node[k])->vec_num_triangle.push_back(node.vec_num_triangle[j]);
				break;
			}									
		}	
	}


	for(size_t i = 0; i < 8; ++i)
	{
		produceChildNode(*(node.child_node[i]), ver);
	}
	
	
}

//Recursive practice, there is a problem with the replication of Node
//error:The child node pointer in the parent node can not pass through to children
/*
void buildOctree(Node& node, VerArray& ver)
{
stack<Node> node_stack;
node_stack.push(node);
while (!node_stack.empty())
{
Node t = node_stack.top();
produceChildNode(t, ver);
node_stack.pop();
if (t.child_node[0] != nullptr)
{
for (int i = 7; i > -1; --i)
{
node_stack.push(*(t.child_node[i]));
}
}

}
}
*/


void buildOctree(Node& origin_node, VerArray& ver, Size& size)
{
	origin_node.points[0][0] = size.min[0];
	origin_node.points[0][1] = size.min[1];
	origin_node.points[0][2] = size.min[2];
	origin_node.points[1][0] = size.min[0]+size.len;
	origin_node.points[1][1] = size.min[1];
	origin_node.points[1][2] = size.min[2];
	origin_node.points[2][0] = size.min[0]+size.len;
	origin_node.points[2][1] = size.min[1]+size.len;
	origin_node.points[2][2] = size.min[2];
	origin_node.points[3][0] = size.min[0];
	origin_node.points[3][1] = size.min[1]+size.len;
	origin_node.points[3][2] = size.min[2];
	origin_node.points[4][0] = size.min[0];
	origin_node.points[4][1] = size.min[1];
	origin_node.points[4][2] = size.min[2]+size.len;
	origin_node.points[5][0] = size.min[0]+size.len;
	origin_node.points[5][1] = size.min[1];
	origin_node.points[5][2] = size.min[2]+size.len;
	origin_node.points[6][0] = size.min[0]+size.len;
	origin_node.points[6][1] = size.min[1]+size.len;
	origin_node.points[6][2] = size.min[2]+size.len;
	origin_node.points[7][0] = size.min[0];
	origin_node.points[7][1] = size.min[1]+size.len;
	origin_node.points[7][2] = size.min[2]+size.len;

	for(int i = 0; i < num_face; ++i)
		origin_node.vec_num_triangle.push_back(i);

	produceChildNode(origin_node, ver);
	//buildOctree(origin_node, ver);
}

#endif