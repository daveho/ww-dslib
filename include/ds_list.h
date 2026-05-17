// Copyright 2025, David H. Hovemeyer <david.hovemeyer@gmail.com>

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

#ifndef DS_LIST_H
#define DS_LIST_H

#include "ds_util.h"

namespace dslib {

//! The node type should contain one instance of Link for
//! each kind of list the node can be a member of.
struct Link {
  void *prev, *next;
};

//! Low-level list implementation class.
//! Don't use this directly: use List, parametized with the
//! node type and the link offset.
class ListImpl {
public:
public:
  //! Node free function type.
  typedef void FreeNodeFn( void *n );

private:
  void *m_head, *m_tail;
  FreeNodeFn *m_free_node_fn;
  unsigned m_link_offset;

  NO_VALUE_SEMANTICS(ListImpl);

public:
  ListImpl( FreeNodeFn *free_node_fn, unsigned link_offset );
  ~ListImpl();

  void *get_first() const { return m_head; }
  void *get_last() const { return m_tail; }

  void append( void *n );
  void prepend( void *n );

  void insert_before( void *node_to_insert, void *existing );
  void insert_after( void *node_to_insert, void *existing );

private:
  Link *get_link( void *n ) const {
    char *p = static_cast< char * >( n );
    p += m_link_offset;
    return reinterpret_cast< Link* >( p );
  }

  void *next( void *n ) const {
    DS_ASSERT( n != nullptr );
    return get_link( n )->next;
  }

  void *prev( void *n ) const {
    DS_ASSERT( n != nullptr );
    return get_link( n )->prev;
  }
};

#if 0
class ListImpl;

//! Intrusive list node base class.
class ListNode {
private:
  ListNode *m_prev, *m_next;

  NO_VALUE_SEMANTICS( ListNode );

public:
  //! Constructor.
  ListNode() : m_prev( nullptr ), m_next( nullptr ) { }

  //! Destructor.
  ~ListNode() {}

  friend class ListImpl;

private:
  //! @return pointer to previous node (nullptr if there is no previous node)
  ListNode *get_prev() const { return m_prev; }

  //! Set the previous node pointer.
  //! @param prev previous node pointer to set
  void set_prev( ListNode *prev ) { m_prev = prev; }

  //! @return pointer to next node (nullptr if there is no next node)
  ListNode *get_next() const { return m_next; }
  
  //! Set the next node pointer.
  //! @param next next node pointer to set
  void set_next( ListNode *next ) { m_next = next; }
};

//! List header implementation class.
//! Don't use this directly: instead, use List, parametized with the
//! actual list node type, which should derive from ListNode.
class ListImpl {
public:
  //! Node free function type.
  typedef void FreeNodeFn( ListNode *node );

private:
  FreeNodeFn *m_free_node_fn;
  // Fake head and tail nodes: same trick as lists in Pintos,
  // this eliminates special cases in insertions and deletions
  ListNode m_head, m_tail;

  NO_VALUE_SEMANTICS( ListImpl );

public:
  ListImpl( FreeNodeFn *free_node_fn );
  ~ListImpl();

  bool is_empty() const;
  ListNode *get_first() const;
  ListNode *get_last() const;
  void append( ListNode *node );
  void prepend( ListNode *node );
  void insert_before( ListNode *node_to_insert, ListNode *existing );
  void insert_after( ListNode *node_to_insert, ListNode *existing );
  void remove( ListNode *node_to_remove );
  ListNode *remove_first();
  ListNode *remove_last();
  unsigned get_size() const;

  ListNode *next( ListNode *node ) const;
  ListNode *prev( ListNode *node ) const;
};

//! List class, storing a sequence of nodes.
//! @tparam ActualNodeType the list node type, which should derive
//!         from ListNode
template< typename ActualNodeType >
class List {
private:
  ListImpl m_impl;

  NO_VALUE_SEMANTICS( List );

public:
  //! Constructor.
  //! @param free_node_fn function to free a list node (called from
  //                      the destructor if the list is non-empty to
  //                      to free all remaining nodes)
  List( ListImpl::FreeNodeFn *free_node_fn )
    : m_impl( free_node_fn ) { }
  
  //! Destructor.
  //! Uses the list's free node function to delete any remaining
  //! nodes.
  ~List() { }

  //! Get the list node that follows the given one.
  //! @return the next list node, or nullptr if the given list
  //!         node is the tail of the list
  ActualNodeType *next( ActualNodeType *node ) const {
    return static_cast< ActualNodeType* >( m_impl.next( node ) );
  }

  //! Get the list node that follows the given one.
  //! @return the previous list node, or nullptr if the given list
  //!         node is the head of the list
  ActualNodeType *prev( ActualNodeType *node ) const {
    return static_cast< ActualNodeType *>( m_impl.prev( node ) );
  }

  //! @return true if list is empty, false if not
  bool is_empty() const { return m_impl.is_empty(); }

  //! @return pointer to the first node in the list
  ActualNodeType *get_first() const {
    return static_cast< ActualNodeType* >( m_impl.get_first() );
  }

  //! @return pointer to the last node in the list
  ActualNodeType *get_last() const {
    return static_cast< ActualNodeType* >( m_impl.get_last() );
  }

  //! Append given node to the list.
  //! The node becomes owned by the list (and will be
  //! freed by the list's destructor.)
  //! @param node the node to append
  void append( ActualNodeType *node ) {
    m_impl.append( node );
  }

  //! Prepend given node to the list.
  //! The node becomes owned by the list (and will be
  //! freed by the list's destructor.)
  //! @param node the node to append
  void prepend( ListNode *node ) {
    m_impl.prepend( node );
  }

  //! Insert a new node before an existing node.
  //! @param node_to_insert the node to insert
  //! @param existing an existing list node
  void insert_before( ListNode *node_to_insert, ListNode *existing ) {
    m_impl.insert_before( node_to_insert, existing );
  }

  //! Insert a new node after an existing node.
  //! @param node_to_insert the node to insert
  //! @param existing an existing list node
  void insert_after( ListNode *node_to_insert, ListNode *existing ) {
    m_impl.insert_after( node_to_insert, existing );
  }

  //! Remove a list node.
  //! The List gives up ownership of the removed node,
  //! so it's the caller's responsibility to free it.
  //! @param existing an existing list node
  void remove( ListNode *node_to_remove ) {
    m_impl.remove( node_to_remove );
  }

  //! Remove the first node in the list.
  //! The list must be nonempty. The list gives up ownership
  //! of the returned node, so it's the caller's responsibility
  //! to free it.
  //! @return the removed first list node
  ActualNodeType *remove_first() {
    return static_cast< ActualNodeType* >( m_impl.remove_first() );
  }

  //! Remove the last node in the list.
  //! The list must be nonempty. The list gives up ownership
  //! of the returned node, so it's the caller's responsibility
  //! to free it.
  //! @return the removed last list node
  ActualNodeType *remove_last() {
    return static_cast< ActualNodeType* >( m_impl.remove_last() );
  }

  //! @return the number of nodes in the list (note that this involves
  //           an O(N) traversal of the list nodes)
  unsigned get_size() const { return m_impl.get_size(); }
};
#endif

} // end namespace dslib

#endif // DS_LIST_H
