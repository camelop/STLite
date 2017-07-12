#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include<iostream>
#include <cstddef>

namespace sjtu { 

template<class T>
const T& min(const T& x, const T& y) {
	if (x < y) return x; else return y;
}

int abs(const int& a) {
	if (a >= 0) return a; else return -a;
}

const int B = 1000;

template<class T>
class deque {
public:
	class block {
		friend class deque;
	public:
		block(int c) :c(c), s(0), nx(NULL), pr(NULL), data(NULL) {
			data = new T*[c];
		}
		block(const block& rhs) :c(rhs.c), s(rhs.s), nx(NULL), pr(NULL), data(NULL) {
			data = new T*[c];
			for (int i = 0; i < rhs.s; i++) data[i] = new T(*rhs.data[i]);
		}
		block& operator =(const block& rhs) {
			if (this == &rhs) return *this;
			for (int i = 0; i < s; i++) delete data[i];
			delete[] data;
			c = rhs.c, s = rhs.s;
			data = new T*[c];
			for (int i = 0; i < rhs.s; i++) data[i] = new T(rhs[i]);
		}
		void clear() {
			for (int i = 0; i < s; i++) delete data[i];
			delete[] data;
			s = 0;
		}
		~block() {
			for (int i = 0; i < s; i++) delete data[i];
			delete[] data;
		}
		T& at(int idx) {
			if (idx<0 || idx >= s) throw index_out_of_bound("from block::operator[]");
			return *(data[idx]);
		}
		T& operator [](int idx) {
			return *(data[idx]);
		}
		void insert(int loc, const T& e) {
			if (loc == s) {
				data[s] = new T(e);
				++s;
				return;
			}
			for (int i = s ; i > loc; i--) data[i] = data[i - 1];
			data[loc] = new T(e);
			++s;
		}
		void erase(int loc) {
			if (loc == s - 1) {
				delete data[s - 1];
				--s;
				return;
			}
			delete data[loc];
			for (int i = loc; i < s-1; i++) data[i] = data[i + 1];
			--s;
		}
		bool full() { return s == c; }

		int s;
	private:
		int c;
		T** data;
		block *nx, *pr;
	};
	class const_iterator;
	class iterator {
		friend class deque;
		friend class const_iterator;
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		const deque* deq;
		block* b;
		int p;
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		iterator() :deq(NULL), b(NULL), p(0) {}
		iterator(const deque* deq, block* b, int p = 0) :deq(deq), b(b), p(p) {}
		iterator(const iterator& rhs) :deq(rhs.deq), b(rhs.b), p(rhs.p) {}
		iterator operator+(int n) const {
			if (n < 0) return operator-(-n);
			if (n == 0) return *this;
			iterator re(*this);
			while (re.b->nx->nx != NULL && (re.b->s - re.p <= n)) 
				n -= re.b->s - re.p, re.p = 0, re.b = re.b->nx;
			if (re.b->nx == NULL) throw index_out_of_bound("from deque::iterator::operator+");
			re.p += n;
			return re;
		}
		iterator operator-(int n) const {
			if (n < 0) return operator+(-n);
			if (n == 0) return *this;
			iterator re(*this);
			while (re.b->pr != NULL && (re.p < n)) n -= re.p + 1, re.b = re.b->pr, re.p = re.b->s - 1;
			if (re.p < n && re.b->pr == NULL) throw index_out_of_bound("from deque::iterator::operator+");
			re.p -= n;
			return re;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			if (deq != rhs.deq) throw invalid_iterator("from deque::iterator::operator-");
			if (b == rhs.b) return p - rhs.p;
			//try if this < b
			block* nw = b->nx;
			int ans = b->s - p;
			while (nw != NULL && nw != rhs.b) ans += nw->s, nw = nw->nx;
			if (nw != NULL) return -(ans + rhs.p);
			ans = rhs.b->s - rhs.p;
			nw = rhs.b->nx;
			while (nw != NULL && nw != b) ans += nw->s, nw = nw->nx;
			if (nw != NULL) return (ans + p);
			throw runtime_error("from deque::iterator::operator-");
		}
		iterator operator+=(const int &n) {
			*this = (*this) + n;
			return *this;
		}
		iterator operator-=(const int &n) {
			*this = (*this) - n;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator re(*this);
			++(*this);
			return re;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			if (p == b->s - 1) {
				p = 0;
				if (b->nx == NULL) throw index_out_of_bound("from deque::iterator::operator++");
				b = b->nx;
			}
			else {
				++p;
			}
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator re(*this);
			--(*this);
			return re;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			if (p == 0) {
				if (b->pr == NULL) throw index_out_of_bound("from deque::iterator::operator--");
				b = b->pr;
				p = b->s - 1;
			}
			else {
				--p;
			}
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const { return (b->at(p)); }
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept { return b->data[p]; }
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return (b == rhs.b && p == rhs.p) ||
				(b->nx == NULL && rhs.b->nx == b && rhs.p == rhs.b->s) ||
				(rhs.b->nx == NULL && b->nx == rhs.b && p == b->s);
		}
		bool operator==(const const_iterator &rhs) const {
			return (b == rhs.b && p == rhs.p) ||
				(b->nx == NULL && rhs.b->nx == b && rhs.p == rhs.b->s) ||
				(rhs.b->nx == NULL && b->nx == rhs.b && p == b->s);
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return !(*this==rhs);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(*this == rhs);
		}
	};
	//copy const_iterator from iterator again!!!
	class const_iterator {
		friend class deque;
		friend class iterator;
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
	private:
		// data members.
		const deque* deq;
		block* b;
		int p;
	public:
		const_iterator() :deq(NULL), b(NULL), p(0) {}
		const_iterator(const deque* deq, block* b, int p = 0) :deq(deq), b(b), p(p) {}
		const_iterator(const const_iterator& rhs) :deq(rhs.deq), b(rhs.b), p(rhs.p) {}
		const_iterator(const iterator& rhs) :deq(rhs.deq), b(rhs.b), p(rhs.p) {}
		// And other methods in iterator.
		// copy from iterator
		const_iterator operator+(int n) const {
			if (n < 0) return operator-(-n);
			if (n == 0) return *this;
			const_iterator re(*this);
			while (re.b->nx->nx != NULL && (re.b->s - re.p <= n))
				n -= re.b->s - re.p, re.p = 0, re.b = re.b->nx;
			if (re.b->nx == NULL) throw index_out_of_bound("from deque::const_iterator::operator+");
			re.p += n;
			return re;
		}
		const_iterator operator-(int n) const {
			if (n < 0) return operator+(-n);
			if (n == 0) return *this;
			const_iterator re(*this);
			while (re.b->pr != NULL && (re.p < n)) n -= re.p + 1, re.b = re.b->pr, re.p = re.b->s - 1;
			if (re.p < n && re.b->pr == NULL) throw index_out_of_bound("from deque::const_iterator::operator+");
			re.p -= n;
			return re;
		}
		// return th distance between two const_iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const const_iterator &rhs) const {
			if (deq != rhs.deq) throw invalid_iterator("from deque::const_iterator::operator-");
			if (b == rhs.b) return p - rhs.p;
			//try if this < b
			block* nw = b->nx;
			int ans = b->s - p;
			while (nw != NULL && nw != rhs.b) ans += nw->s, nw = nw->nx;
			if (nw != NULL) return -(ans + rhs.p);
			ans = rhs.b->s - rhs.p;
			nw = rhs.b->nx;
			while (nw != NULL && nw != b) ans += nw->s, nw = nw->nx;
			if (nw != NULL) return (ans + p);
			throw runtime_error("from deque::const_iterator::operator-");
		}
		const_iterator operator+=(const int &n) {
			*this = (*this) + n;
			return *this;
		}
		const_iterator operator-=(const int &n) {
			*this = (*this) - n;
			return *this;
		}
		/**
		* TODO iter++
		*/
		const_iterator operator++(int) {
			const_iterator re(*this);
			++(*this);
			return re;
		}
		/**
		* TODO ++iter
		*/
		const_iterator& operator++() {
			if (p == b->s - 1) {
				p = 0;
				if (b->nx == NULL) throw index_out_of_bound("from deque::const_iterator::operator++");
				b = b->nx;
			}
			else {
				++p;
			}
			return *this;
		}
		/**
		* TODO iter--
		*/
		const_iterator operator--(int) {
			const_iterator re(*this);
			--(*this);
			return re;
		}
		/**
		* TODO --iter
		*/
		const_iterator& operator--() {
			if (p == 0) {
				if (b->pr == NULL) throw index_out_of_bound("from deque::const_iterator::operator--");
				b = b->pr;
				p = b->s - 1;
			}
			else {
				--p;
			}
			return *this;
		}
		/**
		* TODO *it
		*/
		T& operator*() const { return (b->at(p)); }
		/**
		* TODO it->field
		*/
		T* operator->() const noexcept { return b->data[p]; }
		/**
		* a operator to check whether two iterators are same (pointing to the same memory).
		*/
		bool operator==(const iterator &rhs) const {
			return (b == rhs.b && p == rhs.p) ||
				(b->nx == NULL && rhs.b->nx == b && rhs.p == rhs.b->s) ||
				(rhs.b->nx == NULL && b->nx == rhs.b && p == b->s);
		}
		bool operator==(const const_iterator &rhs) const {
			return (b == rhs.b && p == rhs.p) ||
				(b->nx == NULL && rhs.b->nx == b && rhs.p == rhs.b->s) ||
				(rhs.b->nx == NULL && b->nx == rhs.b && p == b->s);
		}
		/**
		* some other operator for iterator.
		*/
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(*this == rhs);
		}
		// copy end
	};
	deque() :sizeOfBlock(B),s(0),head(NULL),tail(NULL) {
		head = new block(B);
		tail = new block(1);
		head->nx = tail;
		tail->pr = head;
	}
	deque(const deque &other) :sizeOfBlock(other.sizeOfBlock), s(other.s),head(NULL),tail(NULL) {
		head = new block(*other.head);
		tail = new block(1);
		block* cpy = head;
		for (block* ori = other.head->nx;ori != other.tail; ori = ori->nx) {
			cpy->nx = new block(*ori);
			cpy->nx->pr = cpy;
			cpy = cpy->nx;
		}
		cpy->nx = tail;
		tail->pr = cpy;
	}
	~deque() {
		clear();
		delete head;
		delete tail;
	}
	deque &operator=(const deque &other) {
		if (this == &other) return *this;
		clear();
		s = other.s;
		delete head;
		head = new block(*other.head);
		block* cpy = head;
		for (block* ori = other.head->nx; ori != other.tail; ori = ori->nx) {
			cpy->nx = new block(*ori);
			cpy->nx->pr = cpy;
			cpy = cpy->nx;
		}
		cpy->nx = tail;
		tail->pr = cpy;
		return *this;
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos) {
		block* nw = head;
		int num = pos;
		while (nw != tail && num >= nw->s) num -= nw->s, nw = nw->nx;
		if (nw == tail) throw index_out_of_bound("from deque::at");
		return nw->at(num);
	}
	const T & at(const size_t &pos) const {
		block* nw = head;
		int num = pos;
		while (nw != tail && num >= nw->s) num -= nw->s, nw = nw->nx;
		if (nw == tail) throw index_out_of_bound("from deque::at");
		return nw->at(num);
	}
	T & operator[](const size_t &pos) {
		block* nw = head;
		int num = pos;
		while (nw != tail && num >= nw->s) num -= nw->s, nw = nw->nx;
		if (nw == tail) throw index_out_of_bound("from deque::operator[]");
		return nw->at(num);
	}
	const T & operator[](const size_t &pos) const {
		return at(pos);
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
		if (s == 0) throw container_is_empty("from deque::front");
		return head->at(0);
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
		if (s == 0) throw container_is_empty("from deque::back");
		return tail->pr->at(tail->pr->s - 1);
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() { 
		if (s == 0) return end();
		return iterator(this, head, 0); 
	}
	const_iterator cbegin() const { 
		if (s == 0) return cend();
		return const_iterator(this, head, 0); 
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() { return iterator(this, tail, 0); }
	const_iterator cend() const { return const_iterator(this, tail, 0); }

	/**
	 * checks whether the container is empty.
	 */
	bool empty() const { return s == 0; }
	/**
	 * returns the number of elements
	 */
	size_t size() const { return s; }
	/**
	 * clears the contents
	 */
	void clear() {
		block* nw = head->nx;
		block* nx;
		delete head;
		head = new block(B);
		head->nx = tail;
		tail->pr = head;
		while (nw != tail) {
			nx = nw->nx;
			delete nw;
			nw = nx;
		}
		s = 0;
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {
	//	std::cout << "Insert" << std::endl;
		//if (pos == end()) pos = iterator(this, tail->pr, tail->pr->s);
		if (this != pos.deq) throw invalid_iterator("from deque::insert");
		if (pos.p == 0 && pos.b->pr != NULL) pos = iterator(this, pos.b->pr, pos.b->pr->s);
		++s;
		maintain(pos.b);
		while (pos.b->nx != NULL && pos.p > pos.b->s) pos.p -= pos.b->s, pos.b = pos.b->nx;
		if (pos.b->nx == NULL) throw index_out_of_bound("from deque::insert");
		pos.b->insert(pos.p, value);
		return pos;
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {
		if (this != pos.deq) throw invalid_iterator("from deque::erase");
		if (pos.b == tail) throw invalid_iterator("from deque::erase");
		--s;
		while (pos.p < 0) {
			pos.b = pos.b->pr;
			pos.p += pos.b->s;
		}
		if (pos.p == pos.b->s) {
			pos.p = 0;
			pos.b = pos.b->nx;
		}
		maintain(pos.b);
		while (pos.b->nx != NULL && pos.p >= pos.b->s) pos.p -= pos.b->s, pos.b = pos.b->nx;
		if (pos.b->nx == NULL) throw index_out_of_bound("from deque::erase");
		pos.b->erase(pos.p);
		if (pos.p == pos.b->s) {
			pos.p = 0;
			pos.b = pos.b->nx;
		}
		return pos;
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) { insert(iterator(this, tail->pr, tail->pr->s) , value); }
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() { if (s == 0) throw container_is_empty("from deque::pop_back"); erase(iterator(this, tail->pr, tail->pr->s - 1)); }
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) { insert(begin(), value); }
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() { if (s == 0) throw container_is_empty("from deque::pop_back"); erase(begin()); }
	size_t s;
private:
	int sizeOfBlock;
	void update_sizeOfBlock() { /*sizeOfBlock = 100 + (sqrt(s) / 500)) * 500;*/ }
	block* head;
	block* tail;
	void maintain(block*& t) {
		update_sizeOfBlock();
		if (t == tail) return;
		if (t->s < sizeOfBlock / 2) {
			while (t->nx->nx!=NULL && t->s + t->nx->s <= sizeOfBlock ) {
				while (t->nx->nx != NULL && t->s == 0) {
					block* old = t;
					t = t->nx;
					delete old;
				}
				block* nx = t->nx;
				for (int i = t->s; i < t->s + nx->s; i++) t->data[i] = nx->data[i - t->s];
				t->s += t->nx->s;
				nx->s = 0;
				t->nx = t->nx->nx;
				t->nx->pr = t;
				delete nx;
			}
			if (t->nx->nx != NULL && t->s < sizeOfBlock / 2) {
				int trans = (t->s + t->nx->s) / 2 - t->s;
				for (int i = 0; i < trans; i++) t->data[i + t->s] = t->nx->data[i];
				t->s += trans;
				int up = t->nx->s - trans;
				block* nx = t->nx;
				for (int i = 0; i < up; i++) {
					nx->data[i] = nx->data[i + trans];
				}
				nx->s -= trans;
			}
		}
		if (t->s >= sizeOfBlock) {
			block* nw = new block(t->c);
			nw->pr = t;
			nw->nx = t->nx;
			t->nx->pr = nw;
			t->nx = nw;
			int p = t->s / 2;
			for (int i = p; i < t->s; i++) nw->data[i - p] = t->data[i];
			nw->s = t->s - p;
			t->s = p;
		}
	}
};

}

#endif
