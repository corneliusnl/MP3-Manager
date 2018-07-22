#include <string>

#ifndef _STACKER_CPP_
#define _STACKER_CPP_
#include "stacker.h"

// default constructors
Stacker::Stacker(int qsize):_max_nodes(qsize)
{
	_node_count = 0;
	_first = NULL;
	_last = NULL;
}

Stacker::Stacker(string ustring, int qsize):_max_nodes(qsize)
{
	_node_count = 0;
	_first = NULL;
	_last = NULL;
	push(ustring);
}

// default deconstructor.  
Stacker::~Stacker()
{
	while (!is_empty()) {
		pop();
	}

}

// some functions to determine the queue status
bool Stacker::is_empty()
const {
	return _node_count == 0;
} bool Stacker::is_full()
const {
	return _node_count == _max_nodes;
} unsigned int Stacker::count()
const {
	return _node_count;
} bool Stacker::push(string ustring)
{
	if (is_full()) {
		return false;
	}
	Node *newnode = new Node;

	if (newnode == NULL) {
		// no memory!
		return false;
	}
	if (_first == NULL) {	// are we node zero?

		newnode->data = ustring;
		newnode->next = NULL;
		_first = newnode;
	} else {
		newnode->data = ustring;
		newnode->next = _first;
		_first = newnode;
	}
	_node_count++;
	return true;
}

bool Stacker::pop(string & ustring)
{
	if (_first == NULL) {	// nothing here yet.    

		return false;
	}
	ustring = _first->data;

	Node *tmp = _first;
	_first = _first->next;
	delete tmp;

	_node_count--;

	if (_node_count == 0) {
		_first = NULL;
	}
	return true;
}

bool Stacker::pop()
{
	if (_first == NULL) {
		return false;
	}
	Node *tmp = _first;
	_first = _first->next;
	delete tmp;

	_node_count--;

	if (_node_count == 0) {
		_first = NULL;
	}
	return true;
}
#endif
