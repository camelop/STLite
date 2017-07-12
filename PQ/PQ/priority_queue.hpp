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
template<typename T, class Compare> class priority_queue;

template<typename T>
T& max(const T& a, const T& b) {
	if (a < b) return b; else return a;
}

template<typename T>
void swap(T& a, T& b) {
	T temp = a; a = b; b = temp;
}

template<typename T>
class pq_node {
public:
	int npl;
	pq_node<T> *l, *r;
	T data;
	pq_node() :npl(-1), l(NULL), r(NULL) {}
	pq_node(const T& data) :npl(0),l(NULL),r(NULL),data(data) {}
	pq_node(const pq_node& other) :npl(other.npl), l(NULL), r(NULL), data(other.data) {};
};

template<typename T, class Compare = std::less<T> >
class priority_queue {
public:
	/**
	 * TODO constructors
	 */
	priority_queue() :head(NULL),s(0) {}
	priority_queue(const priority_queue &other): s(other.s){
		head = new pq_node<T>(*other.head);
		copy(head, other.head);
	}
	priority_queue(const T& e) {
		head = new pq_node<T>(e);
		s = 1;
	}
	/**
	 * TODO deconstructor
	 */
	~priority_queue() { erase(head); }
	/**
	 * TODO Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) {
		if (this == &other) return *this;
		if (head != NULL) erase(head);
		s = other.s;
		head = new pq_node<T>(*other.head);
		copy(head, other.head);
		return *this;
	}
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const {
		if (empty()) throw container_is_empty("from priority_queue::top");
		return head->data;
	}
	/**
	 * TODO
	 * push new element to the priority queue.
	 */
	void push(const T &e) {
		pq_node<T>* oth = new pq_node<T>(e);
		head = node_merge(head, oth);
		++s;
	}
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() {
		if (empty()) throw container_is_empty("from priority_queue::pop");
		pq_node<T>* l = head->l;
		pq_node<T>* r = head->r;
		delete head;
		head = node_merge(l, r);
		--s;
	}
	/**
	 * return the number of the elements.
	 */
	size_t size() const {
		return s;
	}
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const {
		return (s == 0);
	}
	/**
	 * return a merged priority_queue with at least O(logn) complexity.
	 */
	void merge(priority_queue &other) {
		if (other.empty()) return;
		head = node_merge(head, other.head);
		s += other.s;
		other.head = NULL;
		other.s = 0;
	}
private:
	pq_node<T>* head;
	size_t s;
	void copy(pq_node<T>* t, pq_node<T>* s) {
		if (s->l != NULL) {
			t->l = new pq_node<T>(*s->l);
			copy(t->l, s->l);
		}
		if (s->r != NULL) {
			t->r = new pq_node<T>(*s->r);
			copy(t->r, s->r);
		}
	}
	void erase(pq_node<T>* t) {
		if (t == NULL) return;
		if (t->l != NULL) erase(t->l);
		if (t->r != NULL) erase(t->r);
		delete t;
	}
	pq_node<T>* node_merge(pq_node<T>* x, pq_node<T>* y) {
		if (x == NULL) return y;
		if (y == NULL) return x;
		Compare cmp;
		if (cmp(x->data, y->data)) {
			swap(x, y);
		}
		x->r = node_merge(x->r, y);
		maintain(x);
		return x;
	}
	void maintain(pq_node<T>* x) {
		int lnpl = (x->l == NULL) ? -1 : x->l->npl;
		int rnpl = (x->r == NULL) ? -1 : x->r->npl;
		if (lnpl < rnpl) {
			swap(x->l, x->r);
			swap(lnpl, rnpl);
		}
		x->npl = lnpl + 1;
	}
};

}

#endif
