# MyOctree
#This about my octree for projection
#Until now, it can be used in inputing and outputing file format of .ply 


3.13总结
八叉树现在的用处都体现在本身的现成的用法，用其来减少内存开销，加快运算，很少有对于八叉树这一方法本身的研究，网格剖分思想本身的研究很少。

类似于“什么时候停止剖分”的问题，那既然指定了八叉树的深度，这个问题貌似没有多少意义。
（要自动检测的话），不设置八叉树的深度，就一直迭代下去，直到满足某种条件来进行处理。处理后得到的正方体网格就可以来计算物体的尺寸。
对于三角面片的STL文件，如何用正方体来进行处理？
分不同相交的情况
