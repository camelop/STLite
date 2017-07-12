#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 * it should be based on the vector written by yourself.
 */

template<typename T> class pq_node;
template<typename T, class Compare> class Bernoulli_tree;
template<typename T, class Compare> class Bernoulli_forest;
template<typename T, class Compare> class priority_queue;

template<typename T>
class vector {
public:
	vector() :data(NULL), s(0), c(0) {}
	vector(const vector& e) {
		s = e.s; c = e.c;
		if (e.c != 0) {
			data = new T[c];
			for (int i = 0; i < (int)s; i++) data[i] = e.data[i];
		}
	}
	vector(vector&& e) {
		s = e.s; c = e.c;
		if (e.c != 0) {
			data = new T[c];
			for (int i = 0; i < (int)s; i++) data[i] = e.data[i];
		}
	}
	~vector() { if (c>0) delete[] data; }
	vector& operator =(const vector& e) {
		if (this == &e) return *this;
		if (c != 0) delete[] data;
		s = e.s; c = e.c;
		if (c == 0) {
			data = NULL;
			return *this;
		}
		data = new T[c];
		for (int i = 0; i < (int)s; i++) data[i] = e.data[i];
		return *this;
	}
	vector& operator =(vector&& e) {
		if (this == &e) return *this;
		if (c != 0) delete[] data;
		s = e.s; c = e.c;
		if (c == 0) {
			data = NULL;
			return *this;
		}
		data = new T[c];
		for (int i = 0; i < (int)s; i++) data[i] = e.data[i];
		return *this;
	}
	void push_back(const T& e) {
		if (s == 0) {
			s = c = 1;
			data = new T[1];
			data[0] = e;
		}
		else {
			if (s >= c) double_size();
			data[s++] = e;
		}
	}
	T& operator [](int index) {
		if (index<0 || index >= (int)s) throw index_out_of_bound("from vector");
		return data[index];
	}
	const T& operator [](int index) const {
		if (index<0 || index >= (int)s) throw index_out_of_bound("from vector");
		return data[index];
	}
	size_t s;
private:
	T* data;
	size_t c;
	void double_size(){
		c = c * 2;
		T* ndata= new T[c];
		for (int i = 0; i < (int)s; i++) ndata[i] = data[i];
		delete[] data;
		data = ndata;
	}
};

template<typename T>
class pq_node {
public:
	pq_node() {}
	pq_node(const T& x) : d(x) {};
	pq_node(const pq_node& other) :d(other.d), son(other.son), father(other.father) {}
	T d;
	vector< pq_node* > son;
	pq_node *father;
};

template<typename T, class Compare = std::less<T> >
class Bernoulli_tree {
	friend class Bernoulli_forest<T, Compare>;
	friend class priority_queue<T, Compare>;
public:
	Bernoulli_tree() :head(NULL) {}
	Bernoulli_tree(const T& e):h(1) {
		head = new pq_node<T>(e);
	}
	Bernoulli_tree(const Bernoulli_tree& other) {
		if (other.head == NULL) {
			head = NULL;
			return;
		}
		head = new pq_node<T>(other.head->d);
		copy(head, other.head);
		h = other.h;
	}
	Bernoulli_tree(Bernoulli_tree&& other) {
		if (other.head == NULL) {
			head = NULL;
			return;
		}
		head = new pq_node<T>(other.head->d);
		copy(head, other.head);
		h = other.h;
	}
	Bernoulli_tree(pq_node<T>* e, int hh) {
		if (e == NULL) {
			head = NULL;
			return;
		}
		head = new pq_node<T>((*e).d);
		copy(head, e);
		h = hh;
	}
	~Bernoulli_tree() {
		erase(head);
	}
	Bernoulli_tree& operator = (const Bernoulli_tree& rhs) {
		if (this == &rhs) return *this;
		erase(head);
		if (rhs.head == NULL) {
			head = NULL;
			return *this;
		}
		head = new pq_node<T>(rhs.head->d);
		copy(head, rhs.head);
		h = rhs.h;
		return *this;
	}
	Bernoulli_tree& operator = (Bernoulli_tree&& rhs) {
		if (this == &rhs) return *this;
		erase(head);
		if (rhs.head == NULL) {
			head = NULL;
			return *this;
		}
		head = new pq_node<T>(rhs.head->d);
		copy(head, rhs.head);
		h = rhs.h;
		return *this;
	}
	void clear() {
		erase(head);
		head = NULL;
	}
	Bernoulli_tree operator +(const Bernoulli_tree& rhs) const {
		if (h == 0) return rhs;
		if (rhs.h == 0) return *this;
		if (h != rhs.h) throw runtime_error("Bernoulli tree of different sizes was added");
		Compare cmp;
		if (cmp(head->d, rhs.head->d)) {
			Bernoulli_tree nw(*this);
			Bernoulli_tree* other = new Bernoulli_tree(rhs);
			nw.h++;
			nw.head->son.push_back(other->head);
			other->head->father = nw.head;
			return nw;
		}
		else {
			Bernoulli_tree nw(rhs);
			Bernoulli_tree* other = new Bernoulli_tree(*this);
			nw.h++;
			nw.head->son.push_back(other->head);
			other->head->father = nw.head;
			return nw;
		}
	}
	explicit operator Bernoulli_forest<T, Compare>() {
		Bernoulli_forest<T, Compare> nw;
		nw.s = 1;
		nw.w = h;
		int tp = h - 1;
		while (tp--) { nw.s *= 2; }
		for (int i = 0; i < nw.w - 1; i++) nw.dig.push_back(Bernoulli_tree());
		nw.dig.push_back(*this);
		return nw;
	}
	const bool empty() const {
		return (head == NULL);
	}
	const T& top() const {
		return head->d;
	}
private:
	pq_node<T>* head;
	size_t h;
	void copy(pq_node<T>* nw, pq_node<T>* ori) {
		int size = ori->son.s;
		for (int i = 0; i < size; i++) {
			pq_node<T>* x = new pq_node<T>(ori->son[i]->d);
			nw->son.push_back(x);
			copy(nw->son[i], ori->son[i]);
		}
	}
	void erase(pq_node<T>* nw) {
		if (nw == NULL) return;
		int size = nw->son.s;
		for (int i = 0; i < size; i++) erase(nw->son[i]);
		delete nw;
	}
};

template<typename T, class Compare = std::less<T> >
class Bernoulli_forest {
	friend class Bernoulli_tree<T, Compare>;
	friend class priority_queue<T, Compare>;
public:
	size_t s;
	Bernoulli_forest() :s(0), w(0) {}
	Bernoulli_forest(const Bernoulli_forest& other) :s(other.s), dig(other.dig), w(other.w) {}
	Bernoulli_forest(Bernoulli_forest&& other) :s(other.s), dig(other.dig), w(other.w) {}
	Bernoulli_forest& operator =(const Bernoulli_forest& rhs) {
		if (this == &rhs) return *this;
		s = rhs.s;
		w = rhs.w;
		dig = rhs.dig;
		return *this;
	}
	Bernoulli_forest& operator =(Bernoulli_forest&& rhs) {
		if (this == &rhs) return *this;
		s = rhs.s;
		w = rhs.w;
		dig = rhs.dig;
		return *this;
	}
	Bernoulli_tree<T, Compare>& operator [](int index) {
		if (index<0 || index >= w) throw index_out_of_bound("from Bernoulli_forest");
		return dig[index];
	}
	const Bernoulli_tree<T, Compare>& operator [](int index) const {
		if (index<0 || index >= w) throw index_out_of_bound("from Bernoulli_forest");
		return dig[index];
	}
	Bernoulli_forest operator +(const Bernoulli_forest& rhs) const {
		Bernoulli_forest nw;
		int size = w;
		if (rhs.w>size) size = rhs.w;
		Bernoulli_tree<T, Compare> tp;
		nw.s = s + rhs.s;
		nw.w = size;
		for (int i = 0; i < size; i++) {
			nw.dig.push_back(Bernoulli_tree<T, Compare>());
			if (tp.empty()) {
				if (i>=w || dig[i].empty()) {
					if (i >= rhs.w || rhs[i].empty()) {
						//do nothing
					}
					else {
						nw[i] = rhs[i];
					}
				}
				else {
					if (i >= rhs.w || rhs[i].empty()) {
						nw[i] = dig[i];
					}
					else {
						tp = dig[i] + rhs[i];
					}
				}
			}
			else {
				if (i>=w || dig[i].empty()) {
					if (i >= rhs.w || rhs[i].empty()) {
						nw[i] = tp;
						tp.clear();
					}
					else {
						tp = tp + rhs[i];
					}
				}
				else {
					if (i>=rhs.w || rhs[i].empty()) {
						tp = tp + dig[i];
					}
					else {
						nw[i] = dig[i];
						tp = tp + rhs[i];
					}
				}
			}
		}
		if (!tp.empty()) {
			nw.dig.push_back(Bernoulli_tree<T, Compare>());
			nw.w++;
			nw[size] = tp;
		}
		return nw;
	}
	//aaa
	Bernoulli_forest operator +(const Bernoulli_tree<T, Compare>& rhs) const {
		Bernoulli_forest nw(*this);
		Bernoulli_tree<T, Compare> tp(rhs);
		int p = rhs.h - 1;
		while (p >= nw.w) {
			nw.w++;
			nw.dig.push_back(Bernoulli_tree<T, Compare>());
		}
		while (!nw[p].empty()) {
			tp = tp + nw[p];
			nw[p].clear();
			p++;
			if (p >= nw.w) {
				nw.w++;
				nw.dig.push_back(Bernoulli_tree<T, Compare>());
			}
		}
		nw[p] = tp;
		int s = 1;
		for (int i = 1; i < rhs.h; i++, s *= 2);
		nw.s += s;
		return nw;
	}
	//aaa
	const bool empty() const {
		return (s == 0);
	}
private:
	int w;
	vector<Bernoulli_tree<T, Compare>> dig;
};

template<typename T, class Compare = std::less<T> >
class priority_queue {
public:
	/**
	 * TODO constructors
	 */
	priority_queue() :ptop(-1) {}
	priority_queue(const priority_queue &other) :data(other.data), ptop(-1) {}
	priority_queue(priority_queue&& other) :data(other.data), ptop(-1) {}
	/**
	 * TODO deconstructor
	 */

	//No need to deconstruct
	
	/**
	 * TODO Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) {
		if (this == &other) return *this;
		data = other.data;
		return *this;
	}
	priority_queue &operator=(priority_queue&& other) {
		if (this == &other) return *this;
		data = other.data;
		return *this;
	}
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const {
		if (data.s == 0) throw container_is_empty("from priority_queue::top");
		return data[ptop].top();
	}
	/**
	 * TODO
	 * push new element to the priority queue.
	 */
	void push(const T &e) {
		Bernoulli_tree<T, Compare> nw(e);
		data = data + nw;
		findtop();
	}
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() {
		if (data.empty()) throw container_is_empty("from priority_queue::pop");
		Bernoulli_forest<T, Compare> nw;
		Bernoulli_tree<T, Compare>& trans = data[ptop];
		nw.w = trans.h - 1;
		nw.s = 1;
		for (int i = 0; i < nw.w; i++) {
			nw.dig.push_back(Bernoulli_tree<T, Compare>(trans.head->son[i], i + 1));
			nw.s *= 2;
		}
		nw.s--;
		trans.clear();
		int size = data.s - 1;
		data = data + nw;
		data.s = size;
		findtop();
	}
	/**
	 * return the number of the elements.
	 */
	size_t size() const {
		return data.s;
	}
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const {
		return data.empty();
	}
	/**
	 * return a merged priority_queue with at least O(logn) complexity.
	 */
	Bernoulli_forest<T, Compare>& merge(priority_queue &other) {
		data = data + other.data;
		findtop();
		return *this;
	}
private:
	int ptop;
	Bernoulli_forest<T, Compare> data;

	void findtop() {
		if (data.s == 0) return;
		Compare cmp;
		if (ptop>-1 && data[ptop].empty()) ptop = -1;
		for (int i = 0; i < data.w; i++) {
			if (!data[i].empty()) {
				if (ptop == -1) {
					ptop = i;
				}
				else {
					if (cmp(data[i].top(), data[ptop].top())) {
						ptop = i;
					}
				}
			}
		}
	}
};

}

#endif
