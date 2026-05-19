// Copyright 2025-2026, David H. Hovemeyer <david.hovemeyer@gmail.com>

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// “Software”), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "ds_list.h"

namespace dslib {

ListImpl::ListImpl( FreeNodeFn *free_node_fn, unsigned link_offset )
  : m_head( nullptr )
  , m_tail( nullptr )
  , m_free_node_fn( free_node_fn )
  , m_link_offset( link_offset ) {
}

ListImpl::~ListImpl() {
  if ( m_free_node_fn == nullptr )
    return;

  for ( auto p = m_head; p != nullptr; ) {
    auto n = next( p );
    m_free_node_fn( p );
    p = n;
  }
}

void ListImpl::append( void *n ) {
  Link *link = get_link( n );
  if ( m_head == nullptr ) {
    DS_ASSERT( m_tail == nullptr );
    m_head = m_tail = n;
    link->next = link->prev = nullptr;
  } else {
    Link *tail_link = get_link( m_tail );
    link->prev = m_tail;
    link->next = nullptr;
    DS_ASSERT( tail_link->next == nullptr );
    tail_link->next = n;
    m_tail = n;
  }
}

void ListImpl::prepend( void *n ) {
  Link *link = get_link( n );
  if ( m_tail == nullptr ) {
    DS_ASSERT( m_head == nullptr );
    m_head = m_tail = n;
    link->next = link->prev = nullptr;
  } else {
    Link *head_link = get_link( m_head );
    link->next = m_head;
    link->prev = nullptr;
    DS_ASSERT( head_link->prev == nullptr );
    head_link->prev = n;
    m_head = n;
  }
}

void ListImpl::insert_before( void *node_to_insert, void *existing ) {
  DS_ASSERT( existing != nullptr );

  if ( existing == m_head )
    prepend( node_to_insert );
  else {
    Link *link = get_link( node_to_insert );
    void *succ = existing;
    Link *succ_link = get_link( existing );
    void *pred = succ_link->prev;
    Link *pred_link = get_link( pred );

    link->prev = pred;
    link->next = succ;
    succ_link->prev = node_to_insert;
    pred_link->next = node_to_insert;
  }
}

void ListImpl::insert_after( void *node_to_insert, void *existing ) {
  DS_ASSERT( existing != nullptr );

  if ( existing == m_tail )
    append( node_to_insert );
  else {
    Link *link = get_link( node_to_insert );
    void *pred = existing;
    Link *pred_link = get_link( pred );
    void *succ = pred_link->next;
    Link *succ_link = get_link( succ );

    link->prev = pred;
    link->next = succ;
    succ_link->prev = node_to_insert;
    pred_link->next = node_to_insert;
  }
}

void ListImpl::remove( void *node_to_remove ) {
  if ( node_to_remove == m_head )
    remove_first();
  else if ( node_to_remove == m_tail )
    remove_last();
  else {
    Link *link = get_link( node_to_remove );
    void *pred = link->prev;
    DS_ASSERT( pred != nullptr );
    Link *pred_link = get_link( pred );
    void *succ = link->next;
    DS_ASSERT( succ != nullptr );
    Link *succ_link = get_link( succ );

    pred_link->next = succ;
    succ_link->prev = pred;
  }
}

void *ListImpl::remove_first() {
  DS_ASSERT( m_head != nullptr );

  void *removed = m_head;

  Link *head_link = get_link( m_head );
  if ( head_link->next == nullptr )
    // list becomes empty
    m_head = m_tail = nullptr;
  else {
    void *succ = head_link->next;
    Link *succ_link = get_link( succ );
    succ_link->prev = nullptr;
    m_head = succ;
  }

  return removed;
}

void *ListImpl::remove_last() {
  DS_ASSERT( m_tail != nullptr );

  void *removed = m_tail;

  Link *tail_link = get_link( m_tail );
  if ( tail_link->prev == nullptr )
    // list becomes empty
    m_head = m_tail = nullptr;
  else {
    void *pred = tail_link->prev;
    Link *pred_link = get_link( pred );
    pred_link->next = nullptr;
    m_tail = pred;
  }

  return removed;
}

unsigned ListImpl::get_size() const {
  unsigned count = 0;
  for ( auto p = get_first(); p != nullptr; p = next( p ) )
    ++count;
  return count;
}

} // end namespace dslib
