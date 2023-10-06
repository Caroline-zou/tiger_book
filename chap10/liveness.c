#include <stdio.h>
#include "util.h"
#include "table.h"
#include "symbol.h"
#include "temp.h"
#include "assem.h"
#include "graph.h"
#include "flowgraph.h"
#include "liveness.h"
#include "tree.h"
#include "frame.h"

typedef struct Live_entry_ *Live_entry;
typedef struct TAB_table_ *Temp_table; // Temp_temp -> void*
// For each move instruction, if the src and dst could be assigned into the
// same register, then there would be edge from src to dst in the forest.
// All the nodes in the same tree of the forest could be assigned into the 
// same register.
// In the forest, there would be a few node whose information is null pointer,
// which are to maintain the move relations bewteen nodes with non null 
// information.
// In other words, if one node with non null information could go to another node
// which contains non null information, then there exist a move operation
// corresponding to that edge.

// However, the above solution was throughly stupid that I have decided to 
// abondon.
// And then I think I could take advantage of the dataflow analysis to eliminate
// the copy propagation.
// Now there are two registers named 'src' and 'dst' and there exists such 
// instruction "dst <- src".
// If any execution paths from the node which represents the above instruction 
// where each node all satisfies such an implication that if both 'src' and 
// 'dst' are in the live-out set of that node, and then neither 'src' 
// or 'dst' could occur in the definition set of that node, then 'src' and 'dst'
// could be assigned in the same register.
// Though the fresh solution seems somewhat correct, it would takes lots of time 
// to elimiate the copy propagation.
// In fact, almost all temporaries would only be defined and used once except 
// some temporaries which store the local varibales and we call those 
// temporaries as transience temporary.
// Therefore, we could construct a table to distinguish transience temporary
// according to the number of definition of temporary.
// So if a source temporary in the 'dst <- src' instruction is a transience
// temporary, and then 'dst' and 'src' could be assigned in the same register.
//typedef G_graph G_forest;

struct Live_entry_ {
	Temp_tempList in;
	Temp_tempList out;
	Temp_tempList use;
	Temp_tempList def;
};

// Though the global variables may be somewhat convenient, but those would 
// add a few information coupling into the function whether the function is 
// private or not.
// Eventually, I constrain the usage of global variables in the form of
// tables.

// store node -> {in, out, def, use} mappings in the control flow graph
static G_table flowTab = NULL;
// store temp -> node mappings in the interfere graph
static Temp_table interTab = NULL;
// store temp -> node mappings in the forest
//static Temp_table forestTab = NULL;

static Live_entry Live_Entry(Temp_tempList in, Temp_tempList out, Temp_tempList use, Temp_tempList def) {
	Live_entry entry = checked_malloc(sizeof(*entry));
	entry->in = in;
	entry->out = out;
	entry->use = use;
	entry->def = def;	
}

static void* Temp_find(Temp_table tab, Temp_temp temp) {
	// name Temp_find to avoid conflict Temp_look in module Temp
	return TAB_look(tab, temp);
}

static void Temp_insert(Temp_table tab, Temp_temp temp, void* value) {
	// name Temp_insert to avoid conflict Temp_enter in module Temp
	return TAB_enter(tab, temp, value);
} 

static G_node addInterNode(G_graph inter, Temp_temp temp) {
	// create a node with information about temporary in the interfere graph
	G_node node = G_Node(inter, temp);
	Temp_insert(interTab, temp, node);
	return node;
}

static G_node inter_find(G_graph inter, Temp_temp temp) {
	G_node node = Temp_find(interTab, temp);
	if(!node)
		node = addInterNode(inter, temp);
	return node;
}

static void addInterEdge(G_graph inter, Temp_temp tempA, Temp_temp tempB) {
	// link the nodes which represent 'tempA' or 'tempB' in the interfere graph
	G_node nodeA = inter_find(inter, tempA);
	G_node nodeB = inter_find(inter, tempB);
	G_addEdge(nodeA, nodeB);
	G_addEdge(nodeB, nodeA);
}

/*static void rmInterEdge(Temp_temp tempA, Temp_temp tempB) {
	// remove the edge bewteen the nodes which represent 'tempA' or 'tempB' in
	// the interfere graph
	G_node nodeA = Temp_find(interTab, tempA);
	G_node nodeB = Temp_find(interTab, tempB);
	if(!nodeA || !nodeB || !G_goesTo(nodeA, nodeB))
		return;
	G_rmEdge(nodeA, nodeB);
	G_rmEdge(nodeB, nodeA);	
}*/

/*static void replaceForestNode(G_forest forest, G_node src) {
	// create a node which copy all the edges associated with node 'src'
	// and eliminate all the edges linked with node 'src'
	G_node copy = G_Node(forest, NULL);
	for(G_nodeList pred = G_pred(src); pred; pred = pred->tail) {
		G_addEdge(pred->head, copy);
		G_rmEdge(pred->head, src);
	}
	for(G_nodeList succ = G_succ(src); succ; succ = succ->tail) {
		G_addEdge(copy, succ->head);
		G_rmEdge(src, succ->head);
	}
}*/

/*static void addForestEdge(G_forest forest, G_nodeList *rootNodes, Temp_temp src, Temp_temp dst) {
	// add the edge bewteen the nodes which represent 'src' or 'dst' in the 
	// forest grpah
	G_node from = Temp_find(forestTab, src);
	G_node to = Temp_find(forestTab, dst);
	if(!from) {
		from = G_Node(forest, src);
		*rootNodes = G_NodeList(from, *rootNodes);
		Temp_insert(forestTab, src, from);
	}
	if(!to) {
		to = G_Node(forest, dst);
		Temp_insert(forestTab, dst, to);
	}
	else 
		replaceForestNode(forest, to);
	G_addEdge(from, to);
}*/

/*static void moveForestNode(G_forest forest, G_nodeList *rootNodes, Temp_temp temp) {
	// replace the node which represent 'temp' with information nil and make that
	// node become the new root node
	G_node node = Temp_find(forestTab, temp);
	if(!node)
		return;
	replaceForestNode(forest, node);
	*rootNodes = G_NodeList(node, *rootNodes);
}*/

static Temp_tempList sortList(Temp_tempList src) {
	// copy, sort and unique the orgin list
	if(!src)
		return NULL;
	Temp_tempList dst = Temp_TempList(NULL, Temp_TempList(src->head, NULL));
	for(Temp_tempList temp = src->tail; temp; temp = temp->tail) {
		Temp_tempList p = dst;
		Temp_tempList q = dst->tail;
		// compare the pointer value
		while(q && q->head < temp->head) {
			p = q;
			q = q->tail;
		}
		// copy the node and insert between p and q
    if(!q || q->head != temp->head) {
			// eliminate redundancy
			p->tail = Temp_TempList(temp->head, q);	
		}
	}
	return dst->tail;
}

static Temp_tempList unionList(Temp_tempList src1, Temp_tempList src2) {
	// precondition: both src1 and src2 are sorted link list where each node
	// in the list is unique
	if(!src1)
		return src2;
	if(!src2)
		return src1;
	Temp_tempList dst, last, p, q;
	dst = last = Temp_TempList(NULL, NULL);
	p = src1;
	q = src2;	
	while(p && q) {
		if(p->head < q->head) {
			last = last->tail = Temp_TempList(p->head, NULL);
			p = p->tail;		
		}
		else if(p->head > q->head) {
			last = last->tail = Temp_TempList(q->head, NULL);
			q = q->tail;			
		}
		else {
			last = last->tail = Temp_TempList(p->head, NULL);
			p = p->tail;	
			q = q->tail;
		}
	}
	while(p) {
		last = last->tail = Temp_TempList(p->head, NULL);
		p = p->tail;		
	}
	while(q) {
		last = last->tail = Temp_TempList(q->head, NULL);
		q = q->tail;		
	}
	return dst->tail;
}

static Temp_tempList diffList(Temp_tempList src1, Temp_tempList src2) {
	// precondition: both src1 and src2 are sorted link list where each node
	// in the list is unique
	if(!src1 || !src2)
		return src1;
	Temp_tempList dst, last, p, q;
	dst = last = Temp_TempList(NULL, NULL);
	p = src1;
	q = src2;
	while(p && q) {
		while(p && p->head <= q->head) {
			if(p->head != q->head) 
				last = last->tail = Temp_TempList(p->head, NULL);
			p = p->tail;
		}
		q = q->tail;
	}
	while(p) {
		last = last->tail = Temp_TempList(p->head, NULL);
		p = p->tail;
	}			
	return dst->tail;
}

static bool equalList(Temp_tempList src1, Temp_tempList src2) {
	if(src1 == src2)
		return TRUE;
	if(!src1 || !src2)
		return FALSE;
	Temp_tempList p, q;
	p = src1;
	q = src2;
	while(p && q) {
		if(p->head != q->head)
			return FALSE;
		p = p->tail;
		q = q->tail;
	}
	return !p && !q; 
}

static bool intersectList(Temp_tempList src1, Temp_tempList src2) {
	// return whether list 'src1' intersect with list 'src2'
	// precondition: both src1 and src2 are sorted link list where each node
	// in the list is unique
	Temp_tempList p = src1;
	Temp_tempList q = src2;
	while(p && q) {
		while(p && p->head < q->head)
			p = p->tail;
		if(p && p->head == q->head)
			return TRUE;
		q = q->tail;		
	}
	return FALSE;
}

static Temp_tempList spliceList(Temp_tempList a, Temp_tempList b) {
	// put list b in the end of list a
	if(!a)
		return b;
	if(!b)
		return a;
	Temp_tempList p;
	for(p = a; p->tail; p = p->tail);
	p->tail = b;
	return a;
}

static void dataflowEquation(G_graph flow) {
	G_nodeList nodeList = G_nodes(flow);
	bool loopFlag = TRUE;
	// initailize the in and out set for each node in the control flow graph
	for(G_nodeList node = nodeList; node; node = node->tail) {
		Live_entry entry = Live_Entry(NULL, NULL, sortList(FG_use(node->head)), sortList(FG_def(node->head)));
		G_enter(flowTab, node->head, entry);
	}
	while(loopFlag) {
		loopFlag = FALSE;
		for(G_nodeList node = nodeList; node; node = node->tail) {
			Live_entry entry = G_look(flowTab, node->head);
			assert(entry);
			Temp_tempList out = NULL, in;
			// compute the out set	
			for(G_nodeList succ = G_succ(node->head); succ; succ = succ->tail) {
				Live_entry succEntry = G_look(flowTab, succ->head);
				assert(succEntry);
				out = unionList(succEntry->in, out);
			}
			// compute the in set
			in = unionList(entry->use, diffList(out, entry->def));
			if(!equalList(out, entry->out) || !equalList(in, entry->in))	
				loopFlag = TRUE;
			// update the out and in set
			entry->out = out;
			entry->in = in;
		}
	}
}

static void killUnusedDef(G_graph flow) {
	// precondition: the in and out set of each node in the control flow graph
	// has been computed
	G_nodeList nodeList = G_nodes(flow);
	bool loopFlag;
	while(TRUE) {
		loopFlag = FALSE;
		for(G_nodeList p = nodeList; p; p = p->tail) {
			G_node node = p->head;
			Live_entry entry = G_look(flowTab, node);
			assert(entry);
			if(intersectList(entry->def, entry->out))
				continue; // useful variable definition
			loopFlag |= FG_removeIns(node);
		}
		if(!loopFlag)
			break;
		dataflowEquation(flow);
	}
}

/*static G_forest createMoveForest(G_graph flow, G_nodeList *rootNodes) {
	G_forest forest = G_Graph();
	*rootNodes = NULL;
	for(G_nodeList p = G_nodes(flow); p; p = p->tail) {
		G_node node = p->head;
		Temp_tempList def = FG_def(node);
		if(FG_isMove(node)) {
			Temp_tempList use = FG_use(node);
			assert(use && def);
			Temp_temp src = use->head;
			Temp_temp dst = def->head;
			addForestEdge(forest, rootNodes, src, dst);
		}
		else {
			for(Temp_tempList dst = def; dst; dst = dst->tail)
				moveForestNode(forest, rootNodes, dst->head);
		}
	}
	return forest;
}*/

/*static Temp_tempList collectMoveInformation(G_node rootNode, Live_moveList *moveList) {
	// collect move information and accumulate it in *moveList
	// and return a Temp_temp list which is composed by the information of each
	// node in the tree represented by 'rootNode' except the null pointer
	// rootNode is the node in the forest
	assert(rootNode);
	Temp_temp rootTemp = G_getNodeInfo(rootNode);
	Temp_tempList list = NULL;
	G_nodeList succ = G_succ(rootNode);
	if(rootTemp) {
		list = Temp_TempList(rootTemp, NULL);
		printf("%s:", Temp_look(Temp_name(), rootTemp));
		// collect the information about move operation which could be deleted where 
		// the dst and the src could be assigned into the same register
		for(G_nodeList p = succ; p; p = p->tail) {
			G_node succNode = p->head;
			assert(succNode);
			Temp_temp succTemp = G_getNodeInfo(succNode);
			if(!succTemp)
				continue;
			// if both temps corresponding to'from' and 'to' about the edge are non
			// null, then there exist a move operation bewteen them
			// both 'srcNode' and 'dstNode' resides in the interfere graph
			printf(" %s", Temp_look(Temp_name(), succTemp));
			G_node srcNode = Temp_find(interTab, rootTemp); 
			G_node dstNode = Temp_find(interTab, succTemp);
			*moveList = Live_MoveList(srcNode, dstNode, *moveList);
		}
		printf("\n");
	}
	for(G_nodeList p = succ; p; p = p->tail) 
		list = spliceList(collectMoveInformation(p->head, moveList), list);
	return list;
}*/

/*static void collectTranInfo(G_graph flow, Temp_table useTab, Temp_table defTab) {
	// collect the information about whether a temporary is a transience temporary
	// useTab(defTab) store temp ->  {-1, 1} 
	// -1 means use(def) more than once 
	// while 1 means use(def) only once 
	for(G_nodeList p = G_nodes(flow); p; p = p->tail) {
		G_node node = p->head;
		Live_entry entry = G_look(flowTab, node);
		assert(entry);
		for(Temp_tempList q = entry->use; q; q = q->tail) {
			Temp_temp use = q->head;
			if(Temp_find(useTab, use)) 
				Temp_insert(useTab, use, (void*) -1);
			else
				Temp_insert(useTab, use, (void*) 1);		
		}
		for(Temp_tempList q = entry->def; q; q = q->tail) {
			Temp_temp def = q->head;
			if(Temp_find(defTab, def)) 
				Temp_insert(defTab, def, (void*) -1);
			else
				Temp_insert(defTab, def, (void*) 1);
		}
	}
}*/

/*static void createInterGraph_1(G_graph flow, G_graph inter) {
	// If two temps live in the same point, then we add a edge between two temps
	// in the inter graph
	for(G_nodeList p = G_nodes(flow); p; p = p->tail) {
		G_node node = p->head;
		Live_entry entry = G_look(flowTab, node);
		assert(entry);
		for(Temp_tempList tempA = entry->def; tempA; tempA = tempA->tail) {
			for(Temp_tempList tempB = entry->out; tempB; tempB = tempB->tail) 
				addInterEdge(inter, tempA->head, tempB->head);
		}			
	}
}*/

/*static void createInterGraph_2(G_nodeList rootNodes, Live_moveList *moveList) {
	// If two temps could be assigned into the same register because of move 
	// operation, then we should assure that there is no edge bewteen that two
	// temps.
	for(G_nodeList p = rootNodes; p; p = p->tail) {
		G_node rootNode = p->head;
		Temp_tempList list = collectMoveInformation(rootNode, moveList);
		for(Temp_tempList tempA = list; tempA; tempA = tempA->tail) {
			for(Temp_tempList tempB = tempA->tail; tempB; tempB = tempB->tail)
				rmInterEdge(tempA->head, tempB->head);
		}
	}
}*/

/*static void createInterGraph_2(G_graph flow, Live_moveList *moves) {
	Temp_table useTab = TAB_empty();
	Temp_table defTab = TAB_empty();
	collectTranInfo(flow, useTab, defTab);
  printf("-----------------------------\n");
	for(G_nodeList p = G_nodes(flow); p; p = p->tail) {
		G_node node = p->head;
		if(!FG_isMove(node))
			continue;
		Temp_tempList srcList = FG_use(node);
		assert(srcList);
		Temp_temp src = srcList->head;
		long long useCount = (long long)Temp_find(useTab, src);
		if(useCount < 1)
			continue; // use more than once
		long long defCount = (long long)Temp_find(defTab, src);
		if(defCount < 1)
			continue;
		Temp_tempList dstList = FG_def(node);
		assert(dstList);
		Temp_temp dst = dstList->head;
		G_node srcNode = Temp_find(interTab, src);
		G_node dstNode = Temp_find(interTab, dst);
		assert(srcNode && dstNode);
		// eliminate interfere
		*moves = Live_MoveList(srcNode, dstNode, *moves);
		rmInterEdge(src, dst);
		printf("%s <- %s\n", Temp_look(Temp_layerMap(F_TempMap(), Temp_name()), dst), Temp_look(Temp_layerMap(F_TempMap(), Temp_name()), src));
	}
}*/

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail) {
	Live_moveList moveList = checked_malloc(sizeof(*moveList));
	moveList->src = src;
	moveList->dst = dst;
	moveList->tail = tail;
	return moveList;
}

Temp_temp Live_gtemp(G_node n) {
	return G_getNodeInfo(n);
}

static struct Live_graph build(G_graph flow) {
	struct Live_graph inter = {G_Graph(), NULL};
	for(G_nodeList p = G_nodes(flow); p; p = p->tail) {
		G_node node = p->head;
		Live_entry entry = G_look(flowTab, node);
		assert(entry);
		Temp_tempList defs = entry->def;
		Temp_tempList lives = diffList(entry->out, defs);
		if(FG_isMove(node)) {
			Temp_tempList useList = FG_use(node);
			assert(useList);
			Temp_temp src = useList->head;
			Temp_tempList defList = FG_def(node);
			assert(defList);
			Temp_temp dst = defList->head;
			inter.moves = Live_MoveList(inter_find(inter.graph, src),inter_find(inter.graph, dst), inter.moves);
			lives = diffList(lives, Temp_TempList(src, NULL));
		}
		for(Temp_tempList def = defs; def; def = def->tail) {
			for(Temp_tempList live = lives; live; live = live->tail) 
				addInterEdge(inter.graph, def->head, live->head);
		}
	}
	return inter;
}

struct Live_graph Live_liveness(G_graph flow) {
	//struct Live_graph inter = {G_Graph(), NULL};
	//G_nodeList rootNodes;
	//G_forest forest;

	// initialize global environment
	flowTab = G_empty();
	interTab = TAB_empty();
	//forestTab = TAB_empty();

	dataflowEquation(flow); // compute the dataflow equation
  killUnusedDef(flow); // eliminate unused definition instruction
	//forest = createMoveForest(flow, &rootNodes);
	//createInterGraph_1(flow, inter.graph); // def interfere out
	//createInterGraph_2(flow, &inter.moves); // eliminate some interfere 
	return build(flow);
}

void Live_show(G_graph flow, FILE *out, Temp_map m) {
	for(G_nodeList p = G_nodes(flow); p; p = p->tail) {
		G_node node = p->head;
		assert(node);
		AS_instrList *refInsList = G_getNodeInfo(node);
		if(!refInsList)
			continue;
		AS_instrList insList = *refInsList;
		assert(insList);
		AS_instr ins = insList->head;
		assert(ins);
		AS_print(out, ins, m);
		Live_entry entry = G_look(flowTab, node);
		assert(entry);
		fprintf(out, "  in: ");
		for(Temp_tempList temp = entry->in; temp; temp = temp->tail) 
			fprintf(out, "%s ", Temp_look(m, temp->head));
		fprintf(out, "\n");
		fprintf(out, "  out: ");
		for(Temp_tempList temp = entry->out; temp; temp = temp->tail) 
			fprintf(out, "%s ", Temp_look(m, temp->head));
		fprintf(out, "\n");
		fprintf(out, "  use: ");
		for(Temp_tempList temp = entry->use; temp; temp = temp->tail) 
			fprintf(out, "%s ", Temp_look(m, temp->head));
		fprintf(out, "\n");
		fprintf(out, "  def: ");
		for(Temp_tempList temp = entry->def; temp; temp = temp->tail) 
			fprintf(out, "%s ", Temp_look(m, temp->head));
		fprintf(out, "\n");
	}
}

