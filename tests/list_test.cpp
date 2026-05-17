#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "tctest.h"
#include "ds_list.h"

////////////////////////////////////////////////////////////////////////
// Integer list for testing
////////////////////////////////////////////////////////////////////////

class IntListNode {
private:
  int m_val;
  dslib::Link link;

  NO_VALUE_SEMANTICS( IntListNode );

public:
  IntListNode( int val ) : m_val( val ) { }

  int get_val() const { return m_val; }
  void set_val( int val ) { m_val = val; }

  static void free_int_list_node( void *node );

  static constexpr unsigned link_offset() {
    return offsetof( IntListNode, link );
  }
};

void IntListNode::free_int_list_node( void *node ) {
  delete static_cast< IntListNode* >( node );
}

void check_list_contents(
  const std::vector<int> &expected,
  const dslib::List< IntListNode, IntListNode::link_offset() > &list ) {
  ASSERT( expected.size() == list.get_size() );

  // check in forward direction
  auto p = list.get_first();
  for ( auto i = expected.begin(); i != expected.end(); ++i ) {
    ASSERT( p != nullptr );
    ASSERT( *i == p->get_val() );
    p = list.next( p );
  }
  ASSERT( p == nullptr );

  // check in backward direction
  auto q = list.get_last();
  for ( auto i = expected.rbegin(); i != expected.rend(); ++i ) {
    ASSERT( q != nullptr );
    ASSERT( *i == q->get_val() );
    q = list.prev( q );
  }
  ASSERT( q == nullptr );
}

////////////////////////////////////////////////////////////////////////
// Turn dslib assertions into tctest test failures
////////////////////////////////////////////////////////////////////////

namespace dslib {

void assert_fail( const char *msg, const char *filename, int line ) {
  std::stringstream ss;
  ss << filename << ":" << line << ": " << msg;
  FAIL( ss.str().c_str() ); // uses siglongjmp to "throw" to tctest failure handling code
  for (;;);
}

} // end namespace dslib

////////////////////////////////////////////////////////////////////////
// Test fixture
////////////////////////////////////////////////////////////////////////

struct TestObjs {
  dslib::List< IntListNode, IntListNode::link_offset() > ilist;

  TestObjs() : ilist( &IntListNode::free_int_list_node ) {

  }
};

////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////

// test fixture setup and cleanup
TestObjs *setup();
void cleanup( TestObjs *objs );
// test functions
void test_empty_list( TestObjs *objs );
void test_append( TestObjs *objs );
void test_prepend( TestObjs *objs );
void test_insert_before( TestObjs *objs );
void test_insert_after( TestObjs *objs );
void test_remove( TestObjs *objs );
void test_remove_first( TestObjs *objs );
void test_remove_last( TestObjs *objs );

////////////////////////////////////////////////////////////////////////
// Test program
////////////////////////////////////////////////////////////////////////

int main( int argc, char **argv ) {
  if ( argc > 1 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  TEST( test_empty_list );
  TEST( test_append );
  TEST( test_prepend );
  TEST( test_insert_before );
  TEST( test_insert_after );
  TEST( test_remove );
  TEST( test_remove_first );
  TEST( test_remove_last );

  TEST_FINI();
}

////////////////////////////////////////////////////////////////////////
// Function implementations
////////////////////////////////////////////////////////////////////////

TestObjs *setup() {
  TestObjs *objs = new TestObjs;
  return objs;
}

void cleanup( TestObjs *objs ) {
  delete objs;
}

void test_empty_list( TestObjs *objs ) {
  auto &ilist = objs->ilist;
  
  ASSERT( ilist.get_size() == 0 );
}

void test_append( TestObjs *objs ) {
  auto &ilist = objs->ilist;

  // Append nodes to list
  ilist.append( new IntListNode( 9 ) );
  ASSERT( ilist.get_size() == 1 );
  ilist.append( new IntListNode( 0 ) );
  ASSERT( ilist.get_size() == 2 );
  ilist.append( new IntListNode( 1 ) );
  ASSERT( ilist.get_size() == 3 );
  ilist.append( new IntListNode( 2 ) );
  ASSERT( ilist.get_size() == 4 );
  ilist.append( new IntListNode( 5 ) );
  ASSERT( ilist.get_size() == 5 );

  // verify contents
  check_list_contents( { 9, 0, 1, 2, 5 }, ilist );
}

void test_prepend( TestObjs *objs ) {
  auto &ilist = objs->ilist;

  ilist.prepend( new IntListNode( 5 ) );
  ASSERT( ilist.get_size() == 1 );
  ilist.prepend( new IntListNode( 2 ) );
  ASSERT( ilist.get_size() == 2 );
  ilist.prepend( new IntListNode( 1 ) );
  ASSERT( ilist.get_size() == 3 );
  ilist.prepend( new IntListNode( 0 ) );
  ASSERT( ilist.get_size() == 4 );
  ilist.prepend( new IntListNode( 9 ) );
  ASSERT( ilist.get_size() == 5 );

  // verify contents
  check_list_contents( { 9, 0, 1, 2, 5 }, ilist );
}

void test_insert_before( TestObjs *objs ) {
  auto &ilist = objs->ilist;

  auto p1 = new IntListNode( 0 );
  ilist.append( p1 );
  ilist.append( new IntListNode( 1 ) );
  auto p2 = new IntListNode( 5 );
  ilist.append( p2 );

  ASSERT( ilist.get_size() == 3 );

  ilist.insert_before( new IntListNode( 9 ), p1 );
  ASSERT( ilist.get_size() == 4 );
  check_list_contents( { 9, 0, 1, 5 }, ilist );

  ilist.insert_before( new IntListNode( 2 ), p2 );
  ASSERT( ilist.get_size() == 5 );
  check_list_contents( { 9, 0, 1, 2, 5 }, ilist );
}

void test_insert_after( TestObjs *objs ) {
  auto &ilist = objs->ilist;

  ilist.append( new IntListNode( 9 ) );
  auto p1 = new IntListNode( 0 );
  ilist.append( p1 );
  auto p2 = new IntListNode( 2 );
  ilist.append( p2 );

  ASSERT( ilist.get_size() == 3 );

  ilist.insert_after( new IntListNode( 1 ), p1 );
  ASSERT( ilist.get_size() == 4 );
  check_list_contents( { 9, 0, 1, 2 }, ilist );

  ilist.insert_after( new IntListNode( 5 ), p2 );
  ASSERT( ilist.get_size() == 5 );
  check_list_contents( { 9, 0, 1, 2, 5 }, ilist );
}

void test_remove( TestObjs *objs ) {
  auto &ilist = objs->ilist;

  auto first = new IntListNode( 9 );
  ilist.append( first );
  ilist.append( new IntListNode( 0 ) );
  auto middle = new IntListNode( 1 );
  ilist.append( middle );
  ilist.append( new IntListNode( 2 ) );
  auto last = new IntListNode( 5 );
  ilist.append( last );

  ilist.remove( first );
  ASSERT( ilist.get_size() == 4 );
  check_list_contents( { 0, 1, 2, 5 }, ilist );

  ilist.remove( last );
  ASSERT( ilist.get_size() == 3 );
  check_list_contents( { 0, 1, 2 }, ilist );

  ilist.remove( middle );
  ASSERT( ilist.get_size() == 2 );
  check_list_contents( { 0, 2 }, ilist );

  delete first;
  delete last;
  delete middle;
}

void test_remove_first( TestObjs *objs ) {
  const std::vector<int> vals = { 9, 0, 1, 2, 5 };

  auto &ilist = objs->ilist;
  for ( auto i = vals.begin(); i != vals.end(); ++i )
    ilist.append( new IntListNode( *i ) );
  
  for ( auto i = vals.begin(); i != vals.end(); ++i ) {
    ASSERT( !ilist.is_empty() );
    IntListNode *n = ilist.remove_first();
    ASSERT( n->get_val() == *i );
    delete n;
  }

  ASSERT( ilist.is_empty() );
}

void test_remove_last( TestObjs *objs ) {
  const std::vector<int> vals = { 9, 0, 1, 2, 5 };

  auto &ilist = objs->ilist;
  for ( auto i = vals.begin(); i != vals.end(); ++i )
    ilist.append( new IntListNode( *i ) );
  
  for ( auto i = vals.rbegin(); i != vals.rend(); ++i ) {
    ASSERT( !ilist.is_empty() );
    IntListNode *n = ilist.remove_last();
    ASSERT( n->get_val() == *i );
    delete n;
  }

  ASSERT( ilist.is_empty() );
}
