/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	class map_node {
	public:
		value_type* data;
		int h, s;
		map_node *l, *r;
		map_node *pr, *nx;
		map_node() :l(NULL), r(NULL), nx(NULL), pr(NULL), h(-1), s(0), data(NULL) {}
		map_node(const map_node& other) :l(NULL), r(NULL), nx(NULL), pr(NULL), h(other.h), s(other.s) {
			data = new value_type(*other.data);
		}
		map_node(const value_type& e) :l(NULL), r(NULL), nx(NULL), pr(NULL), h(0), s(1) {
			data = new value_type(e);
		}
		map_node operator =(const map_node& other) {
			l = NULL; r = NULL; nx = NULL; pr = NULL; h = 0; s = 1;
			data = new value_type(*other.data);
		}
		~map_node() { delete data; }
		int bd() {
			int lh, rh;
			if (l == NULL) lh = -1; else lh = l->h;
			if (r == NULL) rh = -1; else rh = r->h;
			return lh - rh;
		}
		void h_update() {
			int lh, rh, ls, rs;
			if (l == NULL) lh = -1, ls = 0; else lh = l->h, ls = l->s;
			if (r == NULL) rh = -1, rs = 0; else rh = r->h, rs = r->s;
			if (lh > rh) h = lh + 1; else h = rh + 1;
			s = ls + rs + 1;
		}
	};
	class const_iterator;
	class iterator {
		friend class map;
		friend class const_iterator;
	private:
		map_node* p;
		const map* t;
	public:
		iterator():p(NULL),t(NULL) {}
		iterator(map_node* p, const map* t) :p(p),t(t) {}
		iterator(const iterator &other) :p(other.p),t(other.t) {}
		iterator(const const_iterator &other) :p(other.p),t(other.t) {}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/**
		 * TODO ++iter
		 */
		iterator& operator =(const iterator &other) {
			if (this == &other) return *this;
			p = other.p;
			t = other.t;
			return *this;
		}
		iterator & operator++() {
			if (p == NULL) throw invalid_iterator("from map::iterator::operator++");
			p = p->nx;
			if (p == NULL) throw invalid_iterator("from map::iterator::operator++");
			return *this;
		}
		/**
		* TODO iter++
		*/
		iterator operator++(int) {
			iterator nw(*this);
			++(*this);
			return nw;
		}
		/**
		 * TODO --iter
		 */
		iterator & operator--() {
			if (p == NULL) throw invalid_iterator("from map::iterator::operator--");
			p = p->pr;
			if (p == NULL) throw invalid_iterator("from map::iterator::operator--");
			return *this;
		}
		/**
		* TODO iter--
		*/
		iterator operator--(int) {
			iterator nw(*this);
			--(*this);
			return nw;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {
			return *(p->data);
		}
		bool operator==(const iterator &rhs) const {
			return (p == rhs.p);
		}
		bool operator==(const const_iterator &rhs) const {
			return (p == rhs.p);
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return (p != rhs.p);
		}
		bool operator!=(const const_iterator &rhs) const {
			return (p != rhs.p);
		}
		/**
		 * for the support of it->first.
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {
			return p->data;
		}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		friend class map;
		friend class iterator;
	private:
		map_node* p;
		const map* t;
	public:
		const_iterator() :p(NULL),t(NULL) {}
		const_iterator(map_node* p,const map* t) :p(p),t(t) {}
		const_iterator(const const_iterator &other) :p(other.p),t(other.t) {}
		const_iterator(const iterator &other) :p(other.p),t(other.t) {}
		/**
		* return a new iterator which pointer n-next elements
		*   even if there are not enough elements, just return the answer.
		* as well as operator-
		*/
		/**
		* a operator to check whether two iterators are same (pointing to the same memory).
		*/
		const_iterator& operator =(const const_iterator &other) {
			if (this == &other) return *this;
			p = other.p;
			t = other.t;
			return *this;
		}
		const value_type & operator*() const {
			return *(p->data);
		}
		const_iterator & operator++() {
			if (p == NULL) throw invalid_iterator("from map::const_iterator::operator++");
			p = p->nx;
			if (p == NULL) throw invalid_iterator("from map::const_iterator::operator++");
			return *this;
		}
		/**
		* TODO iter++
		*/
		const_iterator operator++(int) {
			const_iterator nw(*this);
			++(*this);
			return nw;
		}
		/**
		* TODO --iter
		*/
		const_iterator & operator--() {
			if (p == NULL) throw invalid_iterator("from map::const_iterator::operator--");
			p = p->pr;
			if (p == NULL) throw invalid_iterator("from map::const_iterator::operator--");
			return *this;
		}
		/**
		* TODO iter--
		*/
		const_iterator operator--(int) {
			const_iterator nw(*this);
			--(*this);
			return nw;
		}

		bool operator==(const iterator &rhs) const {
			return (p == rhs.p);
		}
		bool operator==(const const_iterator &rhs) const {
			return (p == rhs.p);
		}
		bool operator!=(const iterator &rhs) const {
			return (p != rhs.p);
		}
		bool operator!=(const const_iterator &rhs) const {
			return (p != rhs.p);
		}
		/**
		* for the support of it->first.
		* See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		*/
		const value_type* operator->() const noexcept {
			return p->data;
		}
	};
	/**
	 * TODO two constructors
	 */
	map() :head(NULL),bg(NULL),ed(NULL) {
		bg = ed = new map_node;
	}
	map(const map &other):head(NULL),bg(NULL),ed(NULL) {
		if (other.head == NULL) {
			bg = ed = new map_node;
			return;
		}
		head = new map_node(*other.head);
		ed = new map_node;
		copy(head, other.head);
		map_node* his = NULL;
		ldr(head, his);
		his->nx = ed;
		ed->pr = his;
	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {
		if (this == &other) return *this;
		clear();
		if (other.head == NULL) {
			return *this;
		}
		head = new map_node(*other.head);
		copy(head, other.head);
		map_node* his = NULL;
		ldr(head, his);
		his->nx = ed;
		ed->pr = his;
		return *this;
	}
	/**
	 * TODO Destructors
	 */
	~map() {
		clear();
		delete ed;
	}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
		map_node* p = search(head, key);
		if (p == NULL) throw index_out_of_bound("from map::at");
		return (p->data->second);
	}
	const T & at(const Key &key) const {
		map_node* p = search(head, key);
		if (p == NULL) throw index_out_of_bound("from map::at");
		return (p->data->second);
	}
	/**
	 * TODO
	 * access specified element
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
		map_node* p = insert(pair<Key, T>(key, T())).first.p; //friendship required
		return (p->data->second);
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
		return at(key);
	}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() { return iterator(bg, this); }
	const_iterator cbegin() const { return const_iterator(bg, this); }
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() { return iterator(ed, this); }
	const_iterator cend() const { return const_iterator(ed, this); }
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const { return head == NULL; }
	/**
	 * returns the number of elements.
	 */
	size_t size() const { return (head == NULL) ? 0 : head->s; }
	/**
	 * clears the contents
	 */
	void clear() {
		clean(head);
		delete ed;
		bg = ed = new map_node;
		head = NULL;
	}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion),
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		map_node* p = search(head, value.first);
		if (p != NULL) return pair<iterator, bool>(iterator(p, this), false);
		p = insert(head, value);
		if (head->s == 1) {
			p->nx = ed;
			ed->pr = p;
			bg = p;
		}
		return pair<iterator, bool>(iterator(p, this), true);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
		if (pos.t != this) throw invalid_iterator("from map::erase Not for this map");
		if (pos.p == NULL) throw invalid_iterator("from map::erase NULL");
		if (pos.p->data == NULL) throw invalid_iterator("from map::erase end()");
		remove(head, pos.p);
		//checktree(head);
	}
	/**
	 * Returns the number of elements with key
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {
		if (search(head, key) == NULL) return 0;
		return 1;
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
		map_node* p = search(head, key);
		if (p == NULL) return end();
		iterator it(p, this);
		return it;
	}
	const_iterator find(const Key &key) const {
		map_node* p = search(head, key);
		if (p == NULL) return cend();
		const_iterator it(p, this);
		return it;
	}
	iterator rank(int t) {
		map_node* p = find_rank(head, t);
		if (p == NULL) return end();
		iterator it(p, this);
		return it;
	}
	const_iterator rank(int t) const {
		map_node* p = find_rank(head, t);
		if (p == NULL) return cend();
		const_iterator it(p, this);
		return it;
	}
private:
	map_node* head;
	Compare cmp;
	map_node *bg, *ed;
	void remove(map_node* &nw, map_node* t) {
		if (nw == NULL) return;
		if (cmp(t->data->first, nw->data->first)) {
			remove(nw->l, t);
			adjust(nw);
		}
		else {
			if (cmp(nw->data->first, t->data->first)) {
				remove(nw->r, t);
				adjust(nw);
			}
			else {
				if ((nw->l != NULL) && (nw->r != NULL)) {
					map_node* p = nw->nx; //stable iterator!!!!!!
					//value_type* temp = nw->data; nw->data = p->data; p->data = temp;
					int hh = nw->h; nw->h = p->h; p->h = hh;
					hh = nw->s; nw->s = p->s; p->s = hh;
					map_node* q;
					if (p != nw->r) {
						q = nw->r;
						while (q->l != p) q = q->l;
						q->l = nw;
						q = p->r; p->r = nw->r; nw->r = q;
						p->l = nw->l;
						nw->l = NULL;
					}
					else {
						nw->r = p->r;
						p->l = nw->l;
						p->r = nw;
						nw->l = NULL;
					}
					q = nw; nw = p; p =	q;
					remove(nw->r, p);
					adjust(nw);
				}
				else {
					map_node* p = nw;
					if (nw == bg) {
						bg = bg->nx;
						bg->pr = NULL;
					}
					else {
						nw->pr->nx = nw->nx;
						nw->nx->pr = nw->pr;
					}
					if (nw->l == NULL) nw = nw->r; else nw = nw->l;
				//	std::cout << "delete " << p->data->second << std::endl;
					delete p;
				}
			}
		}
	}
	map_node* insert(map_node* &nw, const value_type& v) {
		map_node* x;
		bool rm;
		if (nw == NULL) {
			nw = new map_node(v);
			return nw;
		}
		else {
			if (cmp(v.first, nw->data->first)) {
				rm = (nw->l == NULL) ? 1 : 0;
				x = insert(nw->l, v);
				if (rm) {
					x->nx = nw;
					x->pr = nw->pr;
					nw->pr = x;
					if (x->pr != NULL) {
						x->pr->nx = x;
					}
					else {
						bg = x;
					}
				}
				maintain(nw, v);
				return x;
			}
			else {
				rm = (nw->r == NULL) ? 1 : 0;
				x = insert(nw->r, v);
				if (rm) {
					x->pr = nw;
					x->nx = nw->nx;
					nw->nx = x;
					x->nx->pr = x;
				}
				maintain(nw, v);
				return x;
			}
		}
	}
	map_node* search(map_node* nw, const Key &key) const {
		if (nw == NULL) return NULL;
		if (cmp(nw->data->first, key)) {
			return search(nw->r, key);
		}
		else {
			if (cmp(key, nw->data->first)) {
				return search(nw->l, key);
			}
			else {
				return nw;
			}
		}
	}
	map_node* find_rank(map_node* nw, int s) const {
		if (nw == NULL) return NULL;
		if (nw->l == NULL) {
			if (s == 1) return nw;
			return find_rank(nw->r, s - 1);
		}
		if (nw->l->s >= s) return find_rank(nw->l, s);
		if (s == nw->l->s + 1) return nw;
		return find_rank(nw->r, s - 1 - nw->l->s);
	}
	void copy(map_node* nw, map_node* ori) {
		if (ori->l != NULL) {
			nw->l = new map_node(*(ori->l));
			copy(nw->l, ori->l);
		}
		if (ori->r != NULL) {
			nw->r = new map_node(*(ori->r));
			copy(nw->r, ori->r);
		}
		nw->h_update();
	}
	void ldr(map_node* nw, map_node* &his) {
		if (nw->l != NULL) {
			ldr(nw->l, his);
		}
		if (his == NULL) bg = nw;
		nw->pr = his;
		if (his != NULL) his->nx = nw;
		his = nw;
		if (nw->r != NULL) {
			ldr(nw->r, his);
		}
	}
	void clean(map_node* x) {
		if (x == NULL) return;
		clean(x->l);
		clean(x->r);
		delete x;
	}
	inline void LL(map_node* &x) {
//		std::cout << "LL" << std::endl;
		map_node* p = x->l;
		x->l = p->r;
		p->r = x;
		x = p;
		x->r->h_update();
		x->h_update();
	}
	inline void RR(map_node* &x) {
//		std::cout << "RR" << std::endl;
		map_node* p = x->r;
		x->r = p->l;
		p->l = x;
		x = p;
		x->l->h_update();
		x->h_update();
	}
	inline void LR(map_node* &x) {
//		std::cout << "LR" << std::endl;
		map_node* p = x->l;
		map_node* q = p->r;
		p->r = q->l;
		x->l = q->r;
		q->l = p;
		q->r = x;
		x = q;
		x->l->h_update();
		x->r->h_update();
		x->h_update();
	}
	inline void RL(map_node* &x) {
//		std::cout << "RL" << std::endl;
		map_node* p = x->r;
		map_node* q = p->l;
		x->r = q->l;
		p->l = q->r;
		q->l = x;
		q->r = p;
		x = q;
		x->l->h_update();
		x->r->h_update();
		x->h_update();
	}
	inline void maintain(map_node* &x, const value_type& v) {
		x->h_update();
		if (x->bd() == 2) {
			if (cmp(v.first, x->l->data->first)) LL(x); else LR(x);
		}
		if (x->bd() == -2) {
			if (cmp(x->r->data->first, v.first)) RR(x); else RL(x);
		}
	}
	inline void adjust(map_node*& x) {
		//delete l
		x->h_update();
		if (x->bd() == -2) {
			int bfr = x->r->bd();
			if (bfr == 0 || bfr == -1) RR(x); else RL(x);
		}
		//delete r
		if (x->bd() == 2) {
			int bfr = x->l->bd();
			if (bfr == 0 || bfr == 1) LL(x); else LR(x);
		}
	}
};

}

#endif
