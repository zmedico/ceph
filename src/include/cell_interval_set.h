// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */


#ifndef CEPH_BLOCK_INTERVAL_SET_H
#define CEPH_BLOCK_INTERVAL_SET_H

#include <iterator>
#include <map>
#include <ostream>
#include <string.h>

#include "encoding.h"
#include "interval_set.h"
#include "array_interval_set.h"

#define MAX_INTERVALS_PER_ARRAY 1024

template<typename T>
class cell_interval_set {
 public:
  typedef std::pair<T,array_interval_set<T>> map_value_t;
  typedef std::map<T,array_interval_set<T>> map_t;
  cell_interval_set() : _size(0), _num_intervals(0) {}
  cell_interval_set(map_t& other) {
    m.swap(other);
    _size = 0;
    _num_intervals = 0;
    for (auto& i : m) {
      _size += i.second.get_length();
      _num_intervals += i.second.num_intervals();
    }
  }

  int num_intervals() const {
    return _num_intervals;
  }

  class const_iterator;

  class iterator : public std::iterator <std::forward_iterator_tag, T>
  {
    public:
        explicit iterator(typename map_t::iterator iter, const cell_interval_set<T> *parent)
          : _iter(iter), _parent(parent)
        {
          if (_iter != _parent->end())
            _interval_iter = _iter->second.begin();
        }

        iterator(const iterator &rhs): _iter(rhs._iter), _parent(rhs._parent) {
          if (_iter != _parent->end())
            _interval_iter = _iter->second.begin();
        }

        // For the copy constructor and assignment operator, the compiler-generated functions, which
        // perform simple bitwise copying, should be fine.

        bool operator==(const iterator& rhs) const {
          return (_iter == rhs._iter);
        }

        bool operator!=(const iterator& rhs) const {
          return (_iter != rhs._iter);
        }

        typename cell_interval_set::iterator end() const {
          assert(_parent != NULL);
          return _parent->end();
        }

        // Dereference this iterator to get a pair.
        std::pair < T, T > &operator*() {
          return *_interval_iter;
        }

        // Return the interval start.
        T get_start() const {
          T start = _interval_iter->get_start();
        }

        // Return the interval length.
        T get_len() const {
          if (_interval_iter == _interval_iter.end())
            return 0;
          return _interval_iter->get_len();
        }

        // Set the interval length.
        void set_len(T len) {
          _interval_iter->second = len;
        }

        // Preincrement
        iterator &operator++()
        {
          ++_interval_iter;
          if (_interval_iter == _interval_iter.end()) {
            ++_iter;
            if (_iter == _parent->end())
              _interval_iter = _interval_iter.end();
            else
              _interval_iter = _iter->second.begin();
          }
          return *this;
        }

        // Postincrement
        iterator operator++(int) {
          iterator prev(_iter, _parent);
          ++(*this);
          return prev;
        }

    friend class cell_interval_set<T>::const_iterator;

    protected:
        typename map_t::iterator _iter;
        cell_interval_set<T> *_parent;
        typename array_interval_set<T>::iterator _interval_iter;
        typename std::pair<T,T> _interval;
    friend class cell_interval_set<T>;
  };

  class const_iterator : public std::iterator <std::forward_iterator_tag, T>
  {
    public:
        explicit const_iterator(typename map_t::const_iterator iter,
            const cell_interval_set<T> *parent)
            : _iter(iter), _parent(parent) {
          if (_iter != _parent->m.end())
            _interval_iter = _iter->second.begin();
        }

        const_iterator(const iterator &i)
	  : _iter(i._iter), _parent(i._parent) {
          if (_iter != _parent->m.end())
            _interval_iter = _iter->second.begin();
        }

        // For the copy constructor and assignment operator, the compiler-generated functions, which
        // perform simple bitwise copying, should be fine.

        bool operator==(const const_iterator& rhs) const {
          return (_iter == rhs._iter);
        }

        bool operator!=(const const_iterator& rhs) const {
          return (_iter != rhs._iter);
        }

        typename cell_interval_set::const_iterator end() const {
          assert(_parent != NULL);
          return _parent->end();
        }

        // Dereference this iterator to get a pair.
        std::pair < T, T > operator*() const {
            return *_interval_iter;
        }

        // Return the interval start.
        T get_start() const {
          T start = (*_interval_iter).first;
          return start;
        }

        // Return the interval length.
        T get_len() const {
          return (*_interval_iter).second;
        }

        // Preincrement
        const_iterator &operator++() {
          ++_interval_iter;
          if (_interval_iter == _interval_iter.end()) {
            ++_iter;
            if (_iter != _parent->m.end())
              _interval_iter = _iter->second.begin();
          }
          return *this;
        }

        // Postincrement
        const_iterator operator++(int) {
          const_iterator prev(_iter, _parent);
          ++(*this);
          return prev;
        }

    protected:
        typename map_t::const_iterator _iter;
        const cell_interval_set<T> *_parent;
        typename array_interval_set<T>::const_iterator _interval_iter;
        typename std::pair<T,T> _interval;
  };

  typename cell_interval_set<T>::iterator begin() {
    return typename cell_interval_set<T>::iterator(m.begin(), this);
  }

  typename cell_interval_set<T>::iterator lower_bound(T start) {
    return typename cell_interval_set<T>::iterator(find_inc_m(start), this);
  }

  typename cell_interval_set<T>::iterator end() {
    return typename cell_interval_set<T>::iterator(m.end(), this);
  }

  typename cell_interval_set<T>::const_iterator begin() const {
    return typename cell_interval_set<T>::const_iterator(m.begin(), this);
  }

  typename cell_interval_set<T>::const_iterator lower_bound(T start) const {
    return typename cell_interval_set<T>::const_iterator(find_inc(start), this);
  }

  typename cell_interval_set<T>::const_iterator end() const {
    return typename cell_interval_set<T>::const_iterator(m.end(), this);
  }

  // helpers
 private:
  typename map_t::const_iterator find_inc(T start) const {
    typename map_t::const_iterator p = m.lower_bound(start);  // p->first >= start
    if (p != m.begin() &&
        (p == m.end() || p->first > start)) {
      p--;   // might overlap?
      if (p->second.range_end() <= start)
        p++; // it doesn't.
    }
    return p;
  }
  
  typename map_t::iterator find_inc_m(T start) {
    typename map_t::iterator p = m.lower_bound(start);
    if (p != m.begin() &&
        (p == m.end() || p->first > start)) {
      p--;   // might overlap?
      if (p->second.range_end() <= start)
        p++; // it doesn't.
    }
    return p;
  }
  
  typename map_t::const_iterator find_adj(T start) const {
    typename map_t::const_iterator p = m.lower_bound(start);
    if (p != m.begin() &&
        (p == m.end() || p->first > start)) {
      p--;   // might touch?
      if (p->second.range_end() < start)
        p++; // it doesn't.
    }
    return p;
  }
  
  typename map_t::iterator find_adj_m(T start) {
    typename map_t::iterator p = m.lower_bound(start);
    if (p != m.begin() &&
        (p == m.end() || p->first > start)) {
      p--;   // might touch?
      if (p->second.range_end() < start)
        p++; // it doesn't.
    }
    return p;
  }
  
 public:
  bool operator==(const cell_interval_set& other) const {
    if (_size != other._size ||
      _num_intervals != other._num_intervals)
      return false;
    // TODO: optimize with range comparison
    typename cell_interval_set<T>::const_iterator pa = begin(), pb = begin();
    while (pa != end() && pb != end()) {
      if (pa.get_start() != pb.get_start() ||
          pa.get_len() != pb.get_len())
        return false;
      ++pa;
      ++pb;
    }
    return true;
  }

  int64_t size() const {
    return _size;
  }

  void bound_encode(size_t& p) const {
    denc_traits<map_t>::bound_encode(m, p);
  }
  void encode(bufferlist::contiguous_appender& p) const {
    denc(m, p);
  }
  void decode(bufferptr::iterator& p) {
    denc(m, p);
    _size = 0;
    _num_intervals = 0;

    for (const auto& i : m) {
      _size += i.second.get_length();
      _num_intervals += i.second.num_intervals();
    }
  }
  void decode(bufferlist::iterator& p) {
    denc(m, p);
    _size = 0;
    _num_intervals = 0;
    for (const auto& i : m) {
      _size += i.second.get_length();
      _num_intervals += i.second.num_intervals();
    }
  }

  void encode_nohead(bufferlist::contiguous_appender& p) const {
    denc_traits<map_t>::encode_nohead(m, p);
  }
  void decode_nohead(int n, bufferptr::iterator& p) {
    denc_traits<map_t>::decode_nohead(n, m, p);
    _size = 0;
    _num_intervals = 0;
    for (const auto& i : m) {
      _size += i.second.get_length();
      _num_intervals += i.second.num_intervals();
    }
  }

  void clear() {
    m.clear();
    _size = 0;
    _num_intervals = 0;
  }

  bool contains(T i, T *pstart=0, T *plen=0) const {
    typename map_t::const_iterator p = find_inc(i);
    if (p != m.end())
      return p->second.contains(i, pstart, plen);
    return false;
  }
  bool contains(T start, T len) const {
    typename map_t::const_iterator p = find_inc(start);
    if (p != m.end())
      return p->second.contains(start, len);
    return false;
  }
  bool intersects(T start, T len) const {
    typename map_t::const_iterator p = find_inc(start);
    if (p != m.end())
      return p->second.intersects(start, len);
    return false;
  }

  // outer range of set
  bool empty() const {
    return m.empty();
  }
  T range_start() const {
    assert(!empty());
    typename map_t::const_iterator p = m.begin();
    return p->first;
  }
  T range_end() const {
    assert(!empty());
    typename map_t::const_iterator p = m.end();
    p--;
    return p->second.range_end();
  }

  // interval start after p (where p not in set)
  bool starts_after(T i) const {
    assert(!contains(i));
    typename map_t::const_iterator p = find_inc(i);
    if (p == m.end()) return false;
    return true;
  }
  T start_after(T i) const {
    assert(!contains(i));
    typename map_t::const_iterator p = find_inc(i);
    return p->first;
  }

  // interval end that contains start
  T end_after(T start) const {
    assert(contains(start));
    typename map_t::const_iterator p = find_inc(start);
    return p->first+p->second.get_length();
  }
  
  void insert(T val) {
    insert(val, 1);
  }

  void insert(T start, T len, T *pstart=0, T *plen=0) {
    assert(len > 0);
    _size += len;
    typename map_t::iterator p = find_adj_m(start);
    // TODO: join neighbors
    if (p == m.end()) {
      m.emplace(std::piecewise_construct, // new interval
        std::make_tuple(start),
        std::make_tuple(start, len));
      _num_intervals += 1;
      if (pstart)
        *pstart = start;
      if (plen)
        *plen = len;
    } else {
      T _pstart, _plen;
      if (!pstart)
        pstart = &_pstart;
      if (!plen)
        plen = &_plen;
      T pnum = p->second.num_intervals();
      T oldstart = p->second.range_start();
      p->second.insert(start, len, pstart, plen);
      _num_intervals += p->second.num_intervals() - pnum;
      if (*pstart < oldstart) {
        map_value_t value = std::make_pair(*pstart, std::move(p->second));
        typename map_t::iterator n = p;
        p = m.insert(p, std::move(value));
        m.erase(n);
      }

      typename map_t::iterator n = p;
      n++;
      if (n != m.end() &&
          n->second.range_start() == p->second.range_end()) {
        // combine with next, too!
        --_num_intervals;
        p->second.erase(*pstart, *plen);
        if (p->second.empty())
          m.erase(p);
        n->second.insert(*pstart, *plen, pstart, plen);
        m[*pstart] = std::move(n->second);
        m.erase(n);
      }
    }
  }

  void swap(cell_interval_set<T>& other) {
    m.swap(other.m);
    std::swap(_size, other._size);
    std::swap(_num_intervals, other._num_intervals);
  }    

  void erase(T val) {
    erase(val, 1);
  }

  void erase(T start, T len) {
    typename map_t::iterator p = find_inc_m(start);

    _size -= len;
    assert(_size >= 0);

    assert(p != m.end());
    assert(p->first <= start);
    T pnum = p->second.num_intervals();
    p->second.erase(start, len);
    _num_intervals += p->second.num_intervals() - pnum;
    if (p->second.empty())
      m.erase(p);
    else {
      T nstart = p->second.range_start();
      if (p->first != nstart) {
        map_value_t value = std::make_pair(nstart, std::move(p->second));
        m.insert(p, std::move(value));
        m.erase(p);
      }
    }
  }

  void erase(const array_interval_set<T> &a) {
    // TODO: optimize
    for (typename array_interval_set<T>::const_iterator i = a.begin();
        i != a.end();
        ++i)
      erase(i.get_start(), i.get_len());
  }

  void subtract(const cell_interval_set &a) {
    for (typename map_t::const_iterator p = a.m.begin();
         p != a.m.end();
         p++) {
      erase(p->second);
    }
  }

  void insert(const cell_interval_set &a) {
    for (typename map_t::const_iterator p = a.m.begin();
         p != a.m.end();
         p++) {
      typename array_interval_set<T>::const_iterator i = p->second.begin();
      while (i != p->second.end()) {
        // TODO: optimize
        insert(i.get_start(), i.get_len());
        ++i;
      }
    }
  }


  void intersection_of(const cell_interval_set &a, const cell_interval_set &b) {
    assert(&a != this);
    assert(&b != this);
    clear();

    typename map_t::const_iterator pa = a.m.begin();
    typename map_t::const_iterator pb = b.m.begin();
    array_interval_set<T> i;

    while (pa != a.m.end() && pb != b.m.end()) {
      // passing?
      if (pa->second.range_end() <= pb->first)
        { pa++;  continue; }
      if (pb->second.range_end() <= pa->first)
        { pb++;  continue; }
      // TODO: optimize by by using array comparison
      // for intersecting ranges
      i.intersection_of(pa->second, pb->second);
      for (typename array_interval_set<T>::const_iterator it = i.begin();
          it != i.end();
          ++it)
        insert(it.get_start(), it.get_len());
      if (pa->second.range_end() > pb->second.range_end())
        pb++;
      else
        pa++; 
    }
  }
  void intersection_of(const cell_interval_set& b) {
    cell_interval_set a;
    swap(a);
    intersection_of(a, b);
  }

  void union_of(const cell_interval_set &a, const cell_interval_set &b) {
    assert(&a != this);
    assert(&b != this);
    clear();
    
    //cout << "union_of" << endl;

    // a
    m = a.m;
    _size = a._size;
    _num_intervals = a._num_intervals;

    // - (a*b)
    cell_interval_set ab;
    ab.intersection_of(a, b);
    subtract(ab);

    // + b
    insert(b);
    return;
  }
  void union_of(const cell_interval_set &b) {
    cell_interval_set a;
    swap(a);    
    union_of(a, b);
  }
  void union_insert(T off, T len) {
    cell_interval_set a;
    a.insert(off, len);
    union_of(a);
  }

  bool subset_of(const cell_interval_set &big) const {
    // TODO: optimize
    for (typename cell_interval_set::const_iterator i = begin();
         i != end();
         i++) 
      if (!big.contains(i.get_start(), i.get_len()))
        return false;
    return true;
  }

  /*
   * build a subset of @other, starting at or after @start, and including
   * @len worth of values, skipping holes.  e.g.,
   *  span_of([5~10,20~5], 8, 5) -> [8~2,20~3]
   */
  void span_of(const cell_interval_set &other, T start, T len) {
    clear();
    T offset = start;

    while (len) {
      typename map_t::const_iterator p = other.find_inc(offset);
      if (p == other.m.end())
        return;
      array_interval_set<T> s;
      s.span_of(p->second, offset, len);
      if (!s.empty()) {
        m[s.range_start()] = std::move(s);
        _num_intervals += s.num_intervals();
        _size += s.size();
        len -= s.size();
      }
      offset = p->second.range_end();
    }
  }

  /*
   * Move contents of m into another map_t. Use that instead of
   * encoding cell_interval_set into bufferlist then decoding it back into std::map.
   */
  void move_into(map_t& other) {
    other = std::move(m);
  }

private:
  // data
  int64_t _size;
  T _num_intervals;
  map_t m;   // map start -> len
};

// declare traits explicitly because (1) it's templatized, and (2) we
// want to include _nohead variants.
template<typename T>
struct denc_traits<cell_interval_set<T>> {
  static constexpr bool supported = true;
  static constexpr bool bounded = false;
  static constexpr bool featured = false;
  static constexpr bool need_contiguous = denc_traits<T>::need_contiguous;
  static void bound_encode(const cell_interval_set<T>& v, size_t& p) {
    v.bound_encode(p);
  }
  static void encode(const cell_interval_set<T>& v,
		     bufferlist::contiguous_appender& p) {
    v.encode(p);
  }
  static void decode(cell_interval_set<T>& v, bufferptr::iterator& p) {
    v.decode(p);
  }
  template<typename U=T>
    static typename std::enable_if<sizeof(U) && !need_contiguous>::type
    decode(cell_interval_set<T>& v, bufferlist::iterator& p) {
    v.decode(p);
  }
  static void encode_nohead(const cell_interval_set<T>& v,
			    bufferlist::contiguous_appender& p) {
    v.encode_nohead(p);
  }
  static void decode_nohead(size_t n, cell_interval_set<T>& v,
			    bufferptr::iterator& p) {
    v.decode_nohead(n, p);
  }
};


template<class T>
inline std::ostream& operator<<(std::ostream& out, const cell_interval_set<T> &s) {
  out << "[";
  const char *prequel = "";
  for (typename cell_interval_set<T>::const_iterator i = s.begin();
       i != s.end();
       ++i)
  {
    out << prequel << i.get_start() << "~" << i.get_len() << std::flush;
    prequel = ",";
  }
  out << "]";
  return out;
}


#endif
