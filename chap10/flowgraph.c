#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "assem.h"
#include "graph.h"
#include "flowgraph.h"

Temp_tempList FG_def(G_node n) {
	assert(n);
	AS_instrList *refInsList = G_getNodeInfo(n);
	// If refInsList is a nullptr, then the instruction corresponding to that node
	// has been eliminated
	if(!refInsList)
		return NULL; 
	AS_instrList insList = *refInsList;
	assert(insList);
	AS_instr ins = insList->head;
	assert(ins);
	if(ins->kind == I_LABEL) 
		return NULL;
	return ins->u.OPER.dst;
}

Temp_tempList FG_use(G_node n) {
	assert(n);
	AS_instrList *refInsList = G_getNodeInfo(n);
	if(!refInsList)
		return NULL; 
	AS_instrList insList = *refInsList;
	assert(insList);
	AS_instr ins = insList->head;
	assert(ins);
	if(ins->kind == I_LABEL) 
		return NULL;
	return ins->u.OPER.src;
}

bool FG_isMove(G_node n) {
	assert(n);
	AS_instrList *refInsList = G_getNodeInfo(n);
	if(!refInsList)
		return FALSE;
	AS_instrList insList = *refInsList;
	assert(insList);
	AS_instr ins = insList->head;
	assert(ins);
	return ins->kind == I_MOVE;
}

bool FG_removeIns(G_node n) {
	// return whether the instruction corresponding to the node 'n' could be
	// removed
	// if that instruction could be removed, then modify the value of specified
	// pointer to skip that instruction and set the information field of the node
	// 'n' with nullptr
	assert(n);
	AS_instrList *refInsList = G_getNodeInfo(n);
	if(!refInsList) 
		return FALSE; // the instruction has been removed
	AS_instrList insList = *refInsList;
	assert(insList);
	AS_instr ins = insList->head;
	assert(ins);
	switch(ins->kind) {
		case I_OPER: {
			if(!ins->u.OPER.dst)
				return FALSE; // normal jump or mult or div instruction
			if(ins->u.OPER.jumps)
				return FALSE; // function call
			break;
		}
		case I_LABEL: return FALSE;
		case I_MOVE: break;
	}	
	*refInsList = (*refInsList)->tail;
	G_setNodeInfo(n, NULL); // mark the instruction has been removed
	return TRUE;
}


static G_node traceGraph(G_graph g, AS_instrList *refInsList, S_table tab, G_nodeList *jumpList) {
	// In order to eliminate the unused definintion in the liveness analysis, I
	// decide to change the type of information in the node of control flow graph
	// to AS_instrList* to delete some instruction in the list conveniently.
	G_node node = G_Node(g, refInsList); // type of node->info is AS_instrList*
	assert(refInsList);
	AS_instrList insList = *refInsList;
	assert(insList);
	AS_instr ins = insList->head;
	assert(ins);
	if(ins->kind == I_LABEL) {
		// record label -> node mappings for JUMP and CJUMP instruction
		S_enter(tab, ins->u.LABEL.label, node);
	}
	if(ins->kind == I_OPER && ins->u.OPER.jumps) {
		// record all the graph nodes whose type of corresponding instruction is
		// JUMP and CJUMP 
		*jumpList = G_NodeList(node, *jumpList);
	}
	if(insList->tail)
		G_addEdge(node, traceGraph(g, &insList->tail, tab, jumpList));
	return node;
}

G_graph FG_AssemFlowGraph(AS_instrList *il) {
	G_graph g = G_Graph();
	S_table tab = S_empty();
	G_nodeList jumpList = NULL;
	traceGraph(g, il, tab, &jumpList);
	for(G_nodeList jump = jumpList; jump; jump = jump->tail) {
		G_node from = jump->head;
		assert(from);
		AS_instrList *refInsList = G_getNodeInfo(from);
		assert(refInsList);
		AS_instrList insList = *refInsList;
		assert(insList);
		AS_instr ins = insList->head;
		assert(ins);
		assert(ins->kind == I_OPER);
		assert(ins->u.OPER.jumps);
		Temp_labelList labels = ins->u.OPER.jumps->labels;
		assert(labels);
		G_node to = S_look(tab, labels->head);
		// According to the length of 'labels', to judge the kind of jump
		if(!labels->tail) {
			// unconditional jump
			G_nodeList succ = G_succ(from);
			if(succ && to) {
				// remove an edge
				G_rmEdge(from, succ->head);
			}
		}
		if(to)
			G_addEdge(from, to);
	}
	return g;		
}



Temp_tempList FG_def(G_node n) {
	/*
	这个函数的作用是根据节点中包含的指令（AS_instr）的类型来确定该节点定义了哪些寄存器（Temp_tempList）
	首先，它从节点的信息中提取了一个指令对象 ai，这是一个抽象的表示汇编指令的数据结构
	然后，它检查 ai 的类型：
	如果指令类型是 I_OPER，则返回指令的目标操作数（dst）
	如果指令类型是 I_MOVE，则同样返回指令的目标操作数
	如果指令类型是 I_LABEL，则返回 NULL（因为标签指令通常不会定义寄存器）
	如果指令类型不是上述三种类型中的任何一种，它会触发一个断言错误*/
  AS_instr ai = (AS_instr)G_nodeInfo(n);
  switch (ai->kind) {
    case I_OPER:
      return ai->u.OPER.dst;
    case I_MOVE:
      return ai->u.MOVE.dst;
    case I_LABEL:
      return NULL;
    default:
      assert(0);
  }
}
G_nodeList FG_succ(G_node n) {
	/*这个函数的作用是获取与给定节点 n 相邻的后继节点列表
     它使用 G_succ 函数来获取后继节点，并将它们作为一个节点列表返回。*/
  return G_succ(n);
}


Temp_tempList FG_use(G_node n) {
  AS_instr ai = (AS_instr)G_nodeInfo(n);
  switch (ai->kind)
  {
  case I_OPER:
    return ai->u.OPER.src;
  case I_MOVE:
    return ai->u.MOVE.src;
  case I_LABEL:
    return NULL;
  default:
    assert(0);
  }
}
bool FG_isMove(G_node n) { //检查给定节点的指令是否是一个 MOVE 指令
  AS_instr ai = (AS_instr)G_nodeINfo(n);
  return (ai->kind == I_MOVE);
}
