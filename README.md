# Go
**Go Engine based on Monte-Carlo using C++ programming language**

---

### Calendar
*2015-Dec.10th:*  

+ Node for Monte-Carlo searching-tree  
	/* 搜索树结点 */  
	`Node.h`  
	`Node.cpp`  

+ Go-board  
	/* 棋盘 */  
	`Board.h`  
	`Board.cpp`  

*2015-Dec.11th*

+ UCT-gtp bond  
	实现基础的蒙特卡洛搜索  
	不做终局提前优化等
+ Works 读代码-gnuaplus  
	
	代码位置在./reference/gnuaplus-light/aplus_final_code/目录下  

	卞文杰: aplus, board, uct_pure  
	赖凡: tricks_chess, tricks_chess_manual  
	邱丰: bit_process, gtp, interface  

*2015-Dec.12th*  

+ Works 读代码-Hara  
	
	代码位置在./reference/hara-revised/目录下  

	All公共部分 大家都要看: size, goban, gobanconst, priors, group  

	卞文杰: amaf, engine, tree  
	赖凡: heuristics, zobrist  
	邱丰: gtp, gtpcommand  
	
*2015-Dec.18th*  

+ 基本的问题  

	UCT并行  
	棋谱加入到现在的算法中  
	复制棋盘context 避免重复history  
	Zobrist哈希去重 必要性验证  
	
*2015-Dec.20th*  

+ 发现与改进  

	UCT并行  
	启发式搜索 加入棋谱  
	验证证明restore方法即重复history并不比clone棋盘差, 考虑多种方法优化restore  
	尝试使用-std=c++11中提供的新数据结构加速优化  
	