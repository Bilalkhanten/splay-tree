#include <iostream>
#include <algorithm>
#include <string>
//#include <tkDecls.h>

template <typename T>
class splay_tree
{
protected:
	
	struct node
	{
		T user_data;
		size_t size;
		node *parent;
		node *left;
		node *right;
		
		node(T user_data)
				: user_data(user_data)
				, parent(nullptr)
				, left(nullptr)
				, right(nullptr)
		{}
		
		node(T user_data, node* parent, node* left, node* right)
				: user_data(user_data)
				, parent(parent)
				, left(left)
				, right(right)
		{}
		
		bool is_leaf()
		{
			return !left && !right;
		}
		
		bool is_root()
		{
			return parent == nullptr;
		}
		
		bool is_left_son()
		{
			return is_root() ? false : parent->left == this;
		}
		
		bool is_right_son()
		{
			return is_root() ? false : parent->right == this;
		}
		
	}* root;
	
	void rotate_right(node* x)
	{
		if (x->is_root())
			return;
		node* p = x->parent;
		x->parent = p->parent;
		if (x->parent)
		{
			if (p->is_left_son())
				x->parent->left = x;
			else
				x->parent->right = x;
		}
		node* x_r = x->right;
		x->right = p, p->parent = x;
		p->left = x_r;
		if (x_r)
			x_r->parent = p;
	}
	
	void rotate_left(node* x)
	{
		if (x->is_root())
			return;
		node* p = x->parent;
		x->parent = p->parent;
		if (x->parent)
		{
			if (p->is_left_son())
				x->parent->left = x;
			else
				x->parent->right = x;
		}
		node* x_l = x->left;
		x->left = p, p->parent = x;
		p->right = x_l;
		if (x_l)
			x_l->parent = p;
	}
	
	void zig(node* x)
	{			
		node* p = x->parent;
		if (!p->is_root())
			return;
		if (x->is_left_son())
			rotate_right(x);
		else
			rotate_left(x);
	}
	
	void zig_zig(node* x)
	{
		if (x->is_root())
			return;
		node* p = x->parent;
		if (p->is_root() || (x->is_left_son() != p->is_left_son()))
			return;
		if (x->is_left_son())
		{
			rotate_right(p);
			rotate_right(x);
		}
		else
		{
			rotate_left(p);
			rotate_left(x);
		}
	}
	
	void zig_zag(node* x)
	{
		if (x->is_root())
			return;
		node *p = x->parent;
		if (p->is_root() || (x->is_left_son() == p->is_left_son()))
			return;
		if (x->is_left_son()) {
			rotate_right(x);
			rotate_left(x);
		}
		else 
		{
			rotate_left(x);
			rotate_right(x);
		}
	}
	
	void splay(node* x)
	{
		if (!x)
			return;
		while (!x->is_root())
		{
			zig(x);
			zig_zig(x);
			zig_zag(x);
		}
		root = x;
	}
	
	node* insert(node* current_node, node* parent_node, node* new_node)
	{
		if (!current_node) 
		{
			new_node->parent = parent_node;
			return current_node = new_node;
		}
		if (new_node->user_data < current_node->user_data)
			current_node->left =  insert(current_node->left, current_node, new_node);
		else
			current_node->right = insert(current_node->right, current_node, new_node);
		return current_node;
	}
	
	node* max_element(node* x)
	{
		if (!x)
			return nullptr;
		if (!x->right)
			return x;
		return max_element(x->right);
	}

	node* min_element(node* x)
	{
		if (!x)
			return nullptr;
		if (!x->left)
			return x;
		return min_element(x->left);
	}
	
	node* merge(node* left, node* right)
	{
		left = max_element(left);
		splay(left);
		left->right = right;
		return left;
	}
	
	node* find_node(node* current_node, T data)
	{
		if (!current_node)
			return nullptr;
		if (current_node->user_data == data)
			return current_node;
		if (data < current_node->user_data)
			return find_node(current_node->left, data);
		return find_node(current_node->right, data);
	}
	
	T get_data(node* n)
	{
		return (n? n->user_data : (T) NULL);
	}
	
	void print(node* cur)
	{
		if (!cur)
			return;
		
		std::cout << "(";
		print(cur->left);
		std::cout << ") <- " << cur->user_data << "-> (";
		print(cur->right);
		std::cout << ")";
	}

private:
	
	node* find_and_splay(T data)
	{
		node* answer = find_node(root, data);
		splay(answer);
		return answer;
	}
	
public:
	
	splay_tree()
	{
		root = nullptr;
	}
	
	void insert(T new_data)
	{
		if (count(new_data))
			return;
		node* new_node = new node(new_data);
		root = insert(root, nullptr, new_node);
		/*print();
		std::cout << "\n";*/
		splay(new_node);
		int tmp;
		/*std::cin >> tmp;
		print();*/
	}
	
	void erase(T old_data)
	{
		if (!count(old_data))
			return;
		node* node_to_delete = find_and_splay(old_data);
		root = merge(node_to_delete->left, node_to_delete->right);
	}
	
	bool count(T data)
	{
		return find_and_splay(data) != nullptr;
	}
	
	size_t size()
	{
		return root->size;
	}
	
	T next(T data)
	{
		bool should_erase = false;
		if (!count(data))
			insert(data), should_erase = true;
		T answer = get_data(min_element(root->right));
		if (should_erase)
			erase(data);
		return answer;
	}
	
	T prev(T data)
	{
		bool should_erase = false;
		if (!count(data))
			insert(data), should_erase = true;
		node* prev_node = root->left;
		prev_node = max_element(prev_node);
		T answer = get_data(prev_node);
		if (should_erase)
			erase(data);
		return answer;
	}
	
	void print()
	{
		print(root);
		std::cout << '\n';
	}
	
	void rotate_left(T key)
	{
		node* cur = find_node(root, key);
		rotate_left(cur);
	}
	
	void rotate_right(T key)
	{
		node* cur = find_node(root, key);
		rotate_right(cur);
	}
	
	void ins(T new_data)
	{
		if (count(new_data))
			return;
		node* new_node = new node(new_data);
		root = insert(root, nullptr, new node(new_data));
	}
	
	void splay(T key)
	{
		splay(find_node(root, key));
	}
	
};

int main()
{
	splay_tree<int> splay = *new splay_tree<int>();
	std::string cmd;
	
	while (std::cin >> cmd)
	{
		int x;
		std::cin >> x;
		if (cmd == "insert")
			splay.insert(x);
		if (cmd == "delete")
			splay.erase(x);
		if (cmd == "exists")
			std::cout << (splay.count(x) ? "true\n" : "false\n");
		if (cmd == "next")
		{
			int answer = splay.next(x);
			if (!answer)
				std::cout << answer << '\n';
			else
				std::cout << "none\n";
		}
		if (cmd == "prev")
		{
			int answer = splay.prev(x);
			if (!answer)
				std::cout << answer << '\n';
			else
				std::cout << "none\n";
		}
		if (cmd == "print")
			splay.print();
		if (cmd == "rotate_left")
			splay.rotate_left(x);
		if (cmd == "rotate_right")
			splay.rotate_right(x);
		if (cmd == "ins")
			splay.ins(x);
		
	}
	return 0;
}

/*
insert 2
insert 5
insert 3
exists 2
exists 4
next 4
prev 4
delete 5
next 4
prev 4
 
insert 2
insert 5
insert 3
print 0
ins 4
print 0
splay 4
*/


/*
 * () <- 2 -> (3<- 5 -> ())
 *
 */