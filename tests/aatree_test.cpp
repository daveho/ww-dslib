#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <random>
#include "tctest.h"
#include "ds_aatree.h"
#include "ds_aatreeprint.h"

////////////////////////////////////////////////////////////////////////
// Integer tree node type for testing
////////////////////////////////////////////////////////////////////////

class IntAATreeNode : public dslib::AATreeNode {
private:
  int m_val;

  DS_NO_VALUE_SEMANTICS( IntAATreeNode );

public:
  IntAATreeNode( int val = 0 ) : m_val( val ) { }
  ~IntAATreeNode() { }

  void set_val( int val ) { m_val = val; }
  int get_val() const { return m_val; }

  static void free_node_fn( dslib::AATreeNode *node );
  static void copy_node_fn( dslib::AATreeNode *from, dslib::AATreeNode *to );
  static bool less_than_fn( const dslib::AATreeNode *left, const dslib::AATreeNode *right );
};

void IntAATreeNode::free_node_fn( dslib::AATreeNode *node ) {
  delete static_cast< IntAATreeNode* >( node );
}

void IntAATreeNode::copy_node_fn( dslib::AATreeNode *from_, dslib::AATreeNode *to_ ) {
  IntAATreeNode *from = static_cast< IntAATreeNode* >( from_ );
  IntAATreeNode *to = static_cast< IntAATreeNode* >( to_ );

  to->set_val( from->get_val() );
}

bool IntAATreeNode::less_than_fn( const dslib::AATreeNode *left_, const dslib::AATreeNode *right_ ) {
  const IntAATreeNode *left = static_cast< const IntAATreeNode* >( left_ );
  const IntAATreeNode *right = static_cast< const IntAATreeNode* >( right_ );
  return left->m_val < right->m_val;
}

////////////////////////////////////////////////////////////////////////
// Support for printing the test AATree
////////////////////////////////////////////////////////////////////////

class IntAATreePrint : public dslib::AATreePrint {
public:
  IntAATreePrint();
  virtual ~IntAATreePrint();

  virtual std::string node_contents_to_str( dslib::AATreeNode *t ) const;
};

IntAATreePrint::IntAATreePrint() {

}

IntAATreePrint::~IntAATreePrint() {

}

std::string IntAATreePrint::node_contents_to_str( dslib::AATreeNode *t_ ) const {
  IntAATreeNode *t = static_cast< IntAATreeNode* >( t_ );
  return std::to_string( t->get_val() );
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
  dslib::AATree< IntAATreeNode > itree;

  TestObjs()
    : itree( &IntAATreeNode::less_than_fn, &IntAATreeNode::copy_node_fn, &IntAATreeNode::free_node_fn )
  { }
};

// some arbitrary test data
const std::vector< int > TEST_VALS = { 16, 53, 3, 98, 79, 80, 17, 11, 42, 86 };

// how many nodes to insert for the "many" tests
constexpr const int MANY = 100000;

////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////

// test fixture setup and cleanup
TestObjs *setup();
void cleanup( TestObjs *objs );
// test functions
void test_insert( TestObjs *objs );
void test_insert_many( TestObjs *objs );
void test_remove_one( TestObjs *objs );
void test_remove( TestObjs *objs );
void test_remove_many( TestObjs *objs );
void test_iterator_empty( TestObjs *objs );
void test_iterator( TestObjs *objs );
void test_postfix_iterator( TestObjs *objs );

////////////////////////////////////////////////////////////////////////
// Test program
////////////////////////////////////////////////////////////////////////

int main( int argc, char **argv ) {
  if ( argc > 1 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  TEST( test_insert );
  TEST( test_insert_many );
  TEST( test_remove_one );
  TEST( test_remove );
  TEST( test_remove_many );
  TEST( test_iterator_empty );
  TEST( test_iterator );
  TEST( test_postfix_iterator );

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

void test_insert( TestObjs *objs ) {
  auto &itree = objs->itree;

  for ( auto i = TEST_VALS.begin(); i != TEST_VALS.end(); ++i ) {
    itree.insert( new IntAATreeNode( *i ) );
    ASSERT( itree.is_valid() );
  }
  
  for ( int i = 0; i < 100; ++i ) {
    if ( std::find( TEST_VALS.begin(), TEST_VALS.end(), i ) != TEST_VALS.end() )
      ASSERT( itree.contains( i ) );
    else
      ASSERT( !itree.contains( i ) );
  }
}

void test_insert_many( TestObjs *objs ) {
  auto rng = std::default_random_engine();
  std::vector<int> vals;
  for ( int i = 0; i < MANY; ++i )
    vals.push_back( i );
  std::shuffle( vals.begin(), vals.end(), rng );

  auto &itree = objs->itree;

  for ( auto i = vals.begin(); i != vals.end(); ++i )
    itree.insert( new IntAATreeNode( *i ) );
  ASSERT( itree.is_valid() );
/*
  int height = itree.get_height();
  std::cout << "height=" << height;
*/

  for ( auto i = vals.begin(); i != vals.end(); ++i )
    ASSERT( itree.contains( IntAATreeNode( *i ) ) );
}

void test_remove_one( TestObjs *objs ) {
  auto &itree = objs->itree;
  
  itree.insert( new IntAATreeNode( 42 ) );
  ASSERT( itree.contains( IntAATreeNode( 42 ) ) );
  bool removed = itree.remove( IntAATreeNode( 42 ) );
  ASSERT( removed );
}

void test_remove( TestObjs *objs ) {
  auto &itree = objs->itree;
  
  for ( auto i = TEST_VALS.begin(); i != TEST_VALS.end(); ++i )
    itree.insert( new IntAATreeNode( *i ) );

  for ( auto i = TEST_VALS.begin(); i != TEST_VALS.end(); ++i ) {
    bool removed = itree.remove( IntAATreeNode( *i ) );
    ASSERT( removed );
    ASSERT( !itree.contains( IntAATreeNode( *i ) ) );
    ASSERT( itree.is_valid() );
  }
}

void test_remove_many( TestObjs *objs ) {
  auto rng = std::default_random_engine();
  std::vector<int> vals;
  for ( int i = 0; i < MANY; ++i )
    vals.push_back( i );
  std::shuffle( vals.begin(), vals.end(), rng );

  auto &itree = objs->itree;

  // Insert the values in shufled order
  for ( auto i = vals.begin(); i != vals.end(); ++i )
    itree.insert( new IntAATreeNode( *i ) );
  ASSERT( itree.is_valid() );

  // Shuffle the values again
  std::shuffle( vals.begin(), vals.end(), rng );

  // Remove all of the values
  for ( auto i = vals.begin(); i != vals.end(); ++i ) {
    bool removed;
    removed = itree.remove( IntAATreeNode( *i ) );
    ASSERT( removed );
  }

  ASSERT( itree.is_valid() );
  ASSERT( itree.is_empty() );
}

void test_iterator_empty( TestObjs *objs ) {
  auto &itree = objs->itree;

  auto it = itree.iterator();

  // The tree is empty
  ASSERT( !it.has_next() );
}

void test_iterator( TestObjs *objs ) {
  auto &itree = objs->itree;
  
  for ( auto i = TEST_VALS.begin(); i != TEST_VALS.end(); ++i )
    itree.insert( new IntAATreeNode( *i ) );

  std::vector< int > test_vals_sorted = TEST_VALS;
  std::sort( test_vals_sorted.begin(), test_vals_sorted.end(), std::less<int>() );

  // The iterator should give us the test values in sorted order
  auto it = itree.iterator();

  for ( auto i = test_vals_sorted.begin(); i != test_vals_sorted.end(); ++i ) {
    ASSERT( it.has_next() );
    IntAATreeNode *n = it.next();
    ASSERT( *i == n->get_val() );
  }

  // Should be at end of collection now
  ASSERT( !it.has_next() );
}

void test_postfix_iterator( TestObjs *objs ) {
  auto &itree = objs->itree;

  for ( auto i = TEST_VALS.begin(); i != TEST_VALS.end(); ++i )
    itree.insert( new IntAATreeNode( *i ) );

  // Test to make sure all nodes are traversed using the postfix iterator
  std::set< int > seen;
  auto it = itree.postfix_iterator();
  while ( it.has_next() ) {
    IntAATreeNode *n = it.next();
    seen.insert( n->get_val() );
  }

  // seen should contain the same set of values as TEST_VALS
  ASSERT( seen.size() == TEST_VALS.size() );
  for ( auto i = TEST_VALS.begin(); i != TEST_VALS.end(); ++i )
    ASSERT( seen.find( *i ) != seen.end() );
}
