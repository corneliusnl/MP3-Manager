#ifndef _STACKER_H_
#define _STACKER_H_

const unsigned int DEFAULT_MAX_NODES = 10;

class Stacker
{
	struct Node {
		string data;
		struct Node *next;
	};

	 private:
	const int _max_nodes;
	int _node_count;
	Node *_first;
	Node *_last;
	 public:
	 Stacker(int _max_nodes = DEFAULT_MAX_NODES);
	 Stacker(string ustring, int _max_nodes = DEFAULT_MAX_NODES);
	~Stacker();
	bool is_full() const;
	bool is_empty() const;
	unsigned int count() const;
	bool push(string);
	bool pop(string & ustring);
	bool pop();
};

#endif
