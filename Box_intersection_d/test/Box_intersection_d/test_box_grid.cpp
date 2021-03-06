// file: test/Box_intersection_d/box_grid.C
// similar to examples/Box_intersection_d/box_grid.C but stricter in checking
// and more extensive in what is tested.
#include <CGAL/box_intersection_d.h>
#include <cassert>
#include <vector>
#include <algorithm>
#include <iterator>

typedef CGAL::Box_intersection_d::Box_d<int,2> Box;

// coordinates for 9 boxes of a grid
int p[9*4]   = { 0,0,1,1,  1,0,2,1,  2,0,3,1, // lower
                 0,1,1,2,  1,1,2,2,  2,1,3,2, // middle
                 0,2,1,3,  1,2,2,3,  2,2,3,3};// upper
// 9 boxes + 2 selected boxes as query; center and upper right
Box init_boxes[11] = { Box( p,    p+ 2),  Box( p+ 4, p+ 6),  Box( p+ 8, p+10),
                       Box( p+12, p+14),  Box( p+16, p+18),  Box( p+20, p+22),
                       Box( p+24, p+26),  Box( p+28, p+30),  Box( p+32, p+34),
                       Box( p+16, p+18),  Box( p+32, p+34)};
Box  boxes[11];
Box* query = boxes+9; 

void init() {
    for ( int i = 0; i < 11; ++i)
        boxes[i] = init_boxes[i];
}

void check_result( const char* text, std::vector<std::size_t>& result, 
                   const std::size_t* check, std::size_t size) {
    // sort, show, and check result
    std::sort( result.begin(), result.end());
    std::cout << text << ": got " << result.size() << " elements, expected "
              << size << " elements\n    got     : ";
    std::copy( result.begin(), result.end(), 
               std::ostream_iterator<std::size_t>( std::cout, ","));
    std::cout << "\n    expected: ";
    std::copy( check, check+size,
               std::ostream_iterator<std::size_t>( std::cout, ","));
    std::cout << '\n' << std::endl;
    assert( result.size() == size 
            && std::equal( check, check+size, result.begin()));
}

// callback function object writing results to an output iterator
template <class OutputIterator>
struct Report {
    OutputIterator it;
    Report( OutputIterator i) : it(i) {} // store iterator in object
    // We encode both id-numbers in a single number, a.id() + 100 * b.id(),
    // and write that number to the output iterator.
    void operator()( const Box& a, const Box& b) { *it++ = a.id()+100*b.id(); }
};
template <class Iter> // helper function to create the function object
Report<Iter> report( Iter it) { return Report<Iter>(it); }

// ---------------------------------------------------------------------
// box_intersection_d
// run the intersection algorithms and store results in a vector
// ---------------------------------------------------------------------
void test_box_intersection() {
    // intersect 3x3 with 2 query boxes, closed boxes
    init();
    std::vector<std::size_t> result;
    CGAL::box_intersection_d( boxes, boxes+9, query, query+2, 
                              report( std::back_inserter( result)));
    std::size_t check1[13] = {900,901,902,903,904,905,906,907,908,
                              1004,1005,1007,1008};
    check_result( "Box inters. 3x3, 2, closed", result, check1, 13);

    // intersect 3x3 with 2 query boxes, half-open boxes and changed cutoff
    init();
    result.clear();
    CGAL::box_intersection_d( boxes, boxes+9, query, query+2, 
                              report( std::back_inserter( result)),
                              std::ptrdiff_t(1), 
                              CGAL::Box_intersection_d::HALF_OPEN);
    std::size_t check2[2]  = {904,1008};
    check_result( "Box inters. 3x3, 2, half-open", result, check2, 2);

    // self intersect 3x2, closed boxes
    init();
    result.clear();
    CGAL::box_self_intersection_d( boxes, boxes+6, 
                                   report( std::back_inserter( result)));
    std::size_t check3[11] = {1,3,4,102,103,104,105,204,205,304,405};
    check_result( "Box self inters. 3x2, closed", result, check3, 11);

    // self intersect 3x2, half-open boxes
    init();
    result.clear();
    CGAL::box_self_intersection_d( boxes, boxes+6, 
                                   report( std::back_inserter( result)),
                                   std::ptrdiff_t(1), 
                                   CGAL::Box_intersection_d::HALF_OPEN);
    std::size_t check4[1] = {9999};
    check_result( "Box self inters. 3x2, half-open", result, check4, 0);

    // self intersect 3x3+2 query boxes, half-open boxes
    init();
    result.clear();
    CGAL::box_self_intersection_d( boxes, boxes+11, 
                                   report( std::back_inserter( result)),
                                   std::ptrdiff_t(1), 
                                   CGAL::Box_intersection_d::HALF_OPEN);
    std::size_t check5[2] = {409,810};
    check_result( "Box self inters. 3x3+2, half-open", result, check5, 2);

    // self intersect 3x3+2 query boxes, half-open boxes, full function interf.
    // tests also mixed types for the two iterator ranges
    init();
    result.clear();
    std::vector<Box> boxes2( boxes, boxes+11);
    CGAL::box_intersection_d( boxes, boxes+11, boxes2.begin(), boxes2.end(), 
                              report( std::back_inserter( result)),
                              std::ptrdiff_t(1), 
                              CGAL::Box_intersection_d::HALF_OPEN,
                              CGAL::Box_intersection_d::COMPLETE);
    check_result( "Box inters. 3x3+2, half-open", result, check5, 2);
    
    // compare this with the bipartite case
    // self intersect 3x3+2 query boxes, half-open boxes
    // tests also mixed types for the two iterator ranges
    init();
    result.clear();
    boxes2 = std::vector<Box>( boxes, boxes+11);
    CGAL::box_intersection_d( boxes, boxes+11, boxes2.begin(), boxes2.end(), 
                              report( std::back_inserter( result)),
                              std::ptrdiff_t(20), 
                              CGAL::Box_intersection_d::HALF_OPEN,
                              CGAL::Box_intersection_d::BIPARTITE);
    std::size_t check6[4] = {409,810,904,1008};
    check_result( "Box inters. 3x3+2, half-open", result, check6, 4);
}

// ---------------------------------------------------------------------
// box_intersection_all_pairs_d
// run the intersection algorithms and store results in a vector
// ---------------------------------------------------------------------
void test_box_intersection_all_pairs() {
    // intersect 3x3 with 2 query boxes, closed boxes
    init();
    std::vector<std::size_t> result;
    CGAL::box_intersection_all_pairs_d( boxes, boxes+9, query, query+2, 
                                        report( std::back_inserter( result)));
    std::size_t check1[13] = {900,901,902,903,904,905,906,907,908,
                              1004,1005,1007,1008};
    check_result( "All-pairs inters. 3x3, 2, closed", result, check1, 13);

    // intersect 3x3 with 2 query boxes, half-open boxes
    init();
    result.clear();
    CGAL::box_intersection_all_pairs_d( boxes, boxes+9, query, query+2, 
                                        report( std::back_inserter( result)),
                                        CGAL::Box_intersection_d::HALF_OPEN);
    std::size_t check2[2]  = {904,1008};
    check_result( "All-pairs inters. 3x3, 2, half-open", result, check2, 2);

    // self intersect 3x2, closed boxes
    init();
    result.clear();
    CGAL::box_self_intersection_all_pairs_d( boxes, boxes+6, 
                                   report( std::back_inserter( result)));
    std::size_t check3[11] = {100,201,300,301,400,401,402,403,501,502,504};
    check_result( "All-pairs self inters. 3x2, closed", result, check3, 11);

    // self intersect 3x2, half-open boxes
    init();
    result.clear();
    CGAL::box_self_intersection_all_pairs_d( boxes, boxes+6, 
                                   report( std::back_inserter( result)),
                                   CGAL::Box_intersection_d::HALF_OPEN);
    std::size_t check4[1] = {9999};
    check_result( "All-pairs self inters. 3x2, half-open", result, check4, 0);

    // self intersect 3x3+2 query boxes, half-open boxes
    init();
    result.clear();
    CGAL::box_self_intersection_all_pairs_d( boxes, boxes+11, 
                                   report( std::back_inserter( result)),
                                   CGAL::Box_intersection_d::HALF_OPEN);
    std::size_t check5[2] = {904,1008};
    check_result( "All-pairs self inters. 3x3+2, half-open", result, check5,2);

    // self intersect 3x3+2 query boxes, half-open boxes, full function interf.
    // tests also mixed types for the two iterator ranges
    init();
    result.clear();
    std::vector<Box> boxes2( boxes, boxes+11);
    CGAL::box_intersection_all_pairs_d( boxes, boxes+11, 
                                        boxes2.begin(), boxes2.end(), 
                              report( std::back_inserter( result)),
                              CGAL::Box_intersection_d::HALF_OPEN,
                              CGAL::Box_intersection_d::COMPLETE);
    check_result( "All-pairs self inters. 3x3+2, half-open", result, check5,2);
    
    // compare this with the bipartite case
    // self intersect 3x3+2 query boxes, half-open boxes
    // tests also mixed types for the two iterator ranges
    init();
    result.clear();
    boxes2 = std::vector<Box>( boxes, boxes+11);
    CGAL::box_intersection_all_pairs_d( boxes, boxes+11, 
                                        boxes2.begin(), boxes2.end(), 
                              report( std::back_inserter( result)),
                              CGAL::Box_intersection_d::HALF_OPEN,
                              CGAL::Box_intersection_d::BIPARTITE);
    std::size_t check6[4] = {409,810,904,1008};
    check_result( "All-pairs inters. 3x3+2, half-open", result, check6, 4);
}


int main() {
    test_box_intersection();
    test_box_intersection_all_pairs();
    return 0;
}
