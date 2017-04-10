#include <iostream>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <memory>

template <typename T>
class binary_tree
{
protected:
	struct node
	{
		T user_data;
		std::shared_ptr<node> parent;
		std::shared_ptr<node> left;
		std::shared_ptr<node> right;
		
		node(T const& user_data, std::shared_ptr<node> parent = std::shared_ptr<node>(nullptr), std::shared_ptr<node> left = std::shared_ptr<node>(nullptr),
		     std::shared_ptr<node> right = std::shared_ptr<node>(nullptr))
				: user_data(user_data)
				, parent(parent)
				, left(left)
				, right(right)
		{}
		
		bool is_leaf() const
		{
			return !left && !right;
		}
		
		bool is_root() const
		{
			return parent == std::shared_ptr<node>(nullptr);
		}
		
		bool is_left_son() const
		{
			return is_root() ? false : parent->left.get() == this;
		}
		
		bool is_right_son() const
		{
			return is_root() ? false : parent->right == this;
		}
		
	};
	
	std::shared_ptr<node> root;
	
	static std::shared_ptr<node> max_node(std::shared_ptr<node> x)
	{
		if (!x)
			return std::shared_ptr<node>(nullptr);
		std::shared_ptr<node> ans = x;
		while (ans->right)
		{
			ans = ans->right;
		}
		return ans;
	}
	
	static std::shared_ptr<node> min_node(std::shared_ptr<node> x)
	{
		if (!x)
			return std::shared_ptr<node>(nullptr);
		std::shared_ptr<node> ans = x;
		while (ans->left)
		{
			ans = ans->left;
		}
		return ans;
	}
	
	std::shared_ptr<node> insert(std::shared_ptr<node> current_node, std::shared_ptr<node> parent_node, std::shared_ptr<node> new_node)
	{
		if (!current_node)
		{
			new_node->parent = parent_node;
			return new_node;
		}
		if (new_node->user_data < current_node->user_data)
			current_node->left =  insert(current_node->left, current_node, new_node);
		else
			current_node->right = insert(current_node->right, current_node, new_node);
		return current_node;
	}
	
	std::shared_ptr<node> find_node(std::shared_ptr<node> current_node, T const& data)
	{
		if (!current_node)
			return std::shared_ptr<node>(nullptr);
		if (current_node->user_data == data)
			return current_node;
		if (data < current_node->user_data)
			return find_node(current_node->left, data);
		return find_node(current_node->right, data);
	}
	
	static const T* get_data(std::shared_ptr<node> n)
	{
		return (n? &n->user_data : nullptr);
	}
	
	void print(std::shared_ptr<node> cur) const
	{
		if (!cur)
			return;
		std::cout << "(";
		print(cur->left);
		std::cout << ") <- " << cur->user_data << "-> (";
		print(cur->right);
		std::cout << ")";
	}

public:
	
	virtual void insert(T const& new_data) = 0;
	
	virtual void erase(T const& old_data) = 0;
	
	virtual bool count(T const& data) = 0;
	
	virtual const T * next(T const& data) = 0;
	
	virtual const T * prev(T const& data) = 0;
	
	void print() const
	{
		print(root);
	}
	
};

template <typename T>
class rotatable_tree : public binary_tree<T>
{

protected:
	using node = typename binary_tree<T>::node;
	
	void rotate_right(std::shared_ptr<node> x)
	{
		if (x->is_root())
			return;
		std::shared_ptr<node> p = x->parent;
		x->parent = p->parent;
		if (x->parent)
		{
			if (p->is_left_son())
				x->parent->left = x;
			else
				x->parent->right = x;
		}
		std::shared_ptr<node> x_r = x->right;
		x->right = p, p->parent = x;
		p->left = x_r;
		if (x_r)
			x_r->parent = p;
	}
	
	void rotate_left(std::shared_ptr<node> x)
	{
		if (x->is_root())
			return;
		std::shared_ptr<node> p = x->parent;
		x->parent = p->parent;
		if (x->parent)
		{
			if (p->is_left_son())
				x->parent->left = x;
			else
				x->parent->right = x;
		}
		std::shared_ptr<node> x_l = x->left;
		x->left = p, p->parent = x;
		p->right = x_l;
		if (x_l)
			x_l->parent = p;
	}
	
};

template <typename T>
class splay_tree : public rotatable_tree<T>
{
private:
	
	using binary_tree<T>::root;
	using node = typename binary_tree<T>::node;
	using rotatable_tree<T>::rotate_right;
	using rotatable_tree<T>::rotate_left;
	using binary_tree<T>::min_node;
	using binary_tree<T>::max_node;
	using binary_tree<T>::insert;
	using binary_tree<T>::find_node;
	
	bool zig(std::shared_ptr<node> x)
	{
		std::shared_ptr<node> p = x->parent;
		if (!p->is_root())
			return false;
		if (x->is_left_son())
			rotate_right(x);
		else
			rotate_left(x);
		return true;
	}
	
	bool zig_zig(std::shared_ptr<node> x)
	{
		if (x->is_root())
			return false;
		std::shared_ptr<node> p = x->parent;
		if (p->is_root() || (x->is_left_son() != p->is_left_son()))
			return false;
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
		return true;
	}
	
	bool zig_zag(std::shared_ptr<node> x)
	{
		if (x->is_root())
			return false;
		std::shared_ptr<node> p = x->parent;
		if (p->is_root() || (x->is_left_son() == p->is_left_son()))
			return false;
		if (x->is_left_son()) {
			rotate_right(x);
			rotate_left(x);
		}
		else
		{
			rotate_left(x);
			rotate_right(x);
		}
		return true;
	}
	
	void splay(std::shared_ptr<node> x)
	{
		if (!x)
		{
			root = std::shared_ptr<node>(nullptr);
			return;
		}
		while (!x->is_root())
		{
			if (zig(x))
				break;
			if(zig_zig(x))
				continue;
			if(zig_zag(x))
				continue;
		}
		root = x;
	}
	
	std::shared_ptr<node> merge(std::shared_ptr<node> left, std::shared_ptr<node> right)
	{
		left = max_node(left);
		splay(left);
		if (left)
		{
			left->right = right;
			if (right)
				right->parent = left;
		}
		else
		{
			left = right;
			if (left)
				left->parent = std::shared_ptr<node>(nullptr);
		}
		return left;
	}
	
	void splay(T const& data)
	{
		std::shared_ptr<node> answer = find_node(root, data);
		if (answer)
			splay(answer);
	}

public:
	using binary_tree<T>::print;
	
	splay_tree()
	{
		root = std::shared_ptr<node>(nullptr);
	}
	
	void insert(T const& new_data) override 
	{
		if (count(new_data))
			return;
		node* new_n = new node(new_data);
		std::shared_ptr<node> new_node(new_n);
		root = insert(root, std::shared_ptr<node>(nullptr), new_node);
		splay(new_node);
	}
	
	void erase(T const& old_data) override
	{
		if (!count(old_data))
			return;
		if (root->left)
			root->left->parent = std::shared_ptr<node>(nullptr);
		if (root->right)
			root->right->parent = std::shared_ptr<node>(nullptr);
		root = merge(root->left, root->right);
		if (root)
			root->parent = std::shared_ptr<node>(nullptr);
		
	}
	
	bool count(T const& data) override
	{
		splay(data);
		return root && root->user_data == data;
	}
	
	
	const T* next(T const& data) override
	{
		bool should_erase = false;
		if (!count(data)) {
			insert(data);
			should_erase = true;
		}
		const T* answer = binary_tree<T>::get_data(min_node(root->right));
		if (should_erase)
			erase(data);
		return answer;
	}
	
	const T* prev(T const& data) override
	{
		bool should_erase = false;
		if (!count(data))
			insert(data), should_erase = true;
		const T* answer = binary_tree<T>::get_data(max_node(root->left));
		if (should_erase)
			erase(data);
		return answer;
	}
};

int main()
{
	splay_tree<int> splay;
	std::string cmd;
	
	while (std::cin >> cmd) {
		if (cmd == "print") {
			splay.print();
			continue;
		}
		int x;
		std::cin >> x;
		if (cmd == "insert")
			splay.insert(x);
		if (cmd == "delete")
			splay.erase(x);
		if (cmd == "exists")
			std::cout << (splay.count(x) ? "true\n" : "false\n");
		if (cmd == "next") {
			const int *answer = splay.next(x);
			if (answer)
				std::cout << *answer << '\n';
			else
				std::cout << "none\n";
		}
		if (cmd == "prev") {
			const int *answer = splay.prev(x);
			if (answer)
				std::cout << *answer << '\n';
			else
				std::cout << "none\n";
		}
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
*/


/*
 * () <- 2 -> (3<- 5 -> ())
 *
 */