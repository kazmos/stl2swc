#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Mean_curvature_flow_skeletonization.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/split_graph_into_polylines.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/IO/STL_reader.h>
#include <CGAL/subdivision_method_3.h>
#include <fstream>
#include <iostream>
typedef CGAL::Simple_cartesian<double>                        Kernel;
typedef Kernel::Point_3                                       Point;
typedef CGAL::Polyhedron_3<Kernel>                            Polyhedron;
typedef CGAL::Surface_mesh<Point>                             Triangle_mesh;
typedef boost::graph_traits<Polyhedron>::vertex_descriptor    vertex_descriptor;
typedef CGAL::Mean_curvature_flow_skeletonization<Polyhedron> Skeletonization;
typedef CGAL::Mean_curvature_flow_skeletonization<Triangle_mesh> Mcfskel;
typedef Skeletonization::Skeleton                             Skeleton;
typedef Skeleton::vertex_descriptor                           Skeleton_vertex;
typedef Skeleton::edge_descriptor                             Skeleton_edge;

//only needed for the display of the skeleton as maximal polylines
struct Display_polylines{
  const Skeleton& skeleton;
  std::ofstream& out;
  int polyline_size;
  std::stringstream sstr;
  Display_polylines(const Skeleton& skeleton, std::ofstream& out)
    : skeleton(skeleton), out(out)
  {}
  void start_new_polyline(){
    polyline_size=0;
    sstr.str("");
    sstr.clear();
  }
  void add_node(Skeleton_vertex v){
    ++polyline_size;
    sstr << " " << skeleton[v].point;
  }
  void end_polyline()
  {
    out << polyline_size << sstr.str() << "\n";
  }
};

// This example extracts a medially centered skeleton from a given mesh.
int main(int argc, char* argv[])
{
  if (argc<2) {
    std::cout << "Usage: stl2swc <input-file> [<num-subdiv-steps> [<quality-speed-tradeoff [<medially-centered-speed-tradeoff>]]]" << std::endl;
    return EXIT_FAILURE;
  }
  std::ifstream input(argv[1]);

  int num_subdiv = 0;
  if (argc > 2) { 
    num_subdiv = std::stoi(argv[2]);
  }

  double quality_speed_tradeoff = 0.1;
  if (argc > 3) {
    quality_speed_tradeoff = std::stod(argv[3]);
  }

  double medially_centered_speed_tradeoff = 0.2;
  if (argc > 4) {
    medially_centered_speed_tradeoff = std::stod(argv[4]);
  }

  std::vector<Point> points;
  std::vector<std::vector<std::size_t> > triangles;

  bool read_success = CGAL::read_STL(input, points, triangles);
  if (!read_success) {
    std::cout << "Could not read STL file." << std::endl;
    return EXIT_FAILURE;
  }
  input.close();

  Polyhedron mesh;
  CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, triangles, mesh);
  if (mesh.empty()) {
    std::cerr << "Input geometry is empty" << std::endl;
    return EXIT_FAILURE;
  }
  if (!CGAL::is_triangle_mesh(mesh))
  {
    std::cout << "Input geometry is not triangulated." << std::endl;
    return EXIT_FAILURE;
  }


  // Optionally, subdivide using Loop
  if (num_subdiv > 0) {
    std::cout << "Applying " << num_subdiv << " rounds of Loop subdivision" << std::endl;
    CGAL::Subdivision_method_3::Loop_subdivision(mesh,
        CGAL::parameters::number_of_iterations(num_subdiv));
  }

  //Mcfskel mcfs(mesh);
  Skeletonization mcfs(mesh);

  mcfs.set_quality_speed_tradeoff(quality_speed_tradeoff);
  mcfs.set_medially_centered_speed_tradeoff(medially_centered_speed_tradeoff);

  Skeleton skeleton;
  mcfs(skeleton);

  std::cout << "Number of vertices of the skeleton: " << boost::num_vertices(skeleton) << "\n";
  std::cout << "Number of edges of the skeleton: " << boost::num_edges(skeleton) << "\n";

  int node_id = 0;
  std::map<std::string, Point> node_map;

  for(Skeleton_vertex v : CGAL::make_range(vertices(skeleton))) {
    for(vertex_descriptor vd : skeleton[v].vertices) {
      std::stringstream ss;
      ss << skeleton[v].point;
      node_map[ss.str()] = get(CGAL::vertex_point, mesh, vd);
    }
  }

  std::ofstream output("test.edges");
  for(Skeleton_edge e : CGAL::make_range(edges(skeleton)))
  {
    Point& s = skeleton[source(e, skeleton)].point;
    Point& t = skeleton[target(e, skeleton)].point;

    // SWC format:
    // ID, Type (0), X, Y, Z, Radius, Parent ID
    std::stringstream ss;
    ss << s;
    output << s << ' ' << t << ' ' << node_map[ss.str()] << "\n";
  }
  output.close();

  std::cout << "Done" << std::endl;

  /*
  // Output all the edges of the skeleton.
  std::ofstream output("skel-poly.cgal");
  Display_polylines display(skeleton,output);
  CGAL::split_graph_into_polylines(skeleton, display);
  output.close();


  std::ofstream output("skel-sm.cgal");
  for(Skeleton_edge e : CGAL::make_range(edges(skeleton)))
  {
    const Point& s = skeleton[source(e, skeleton)].point;
    const Point& t = skeleton[target(e, skeleton)].point;
    output << "2 " << s << " " << t << "\n";
  }
  output.close();

  // Output skeleton points and the corresponding surface points
  output.open("correspondance-poly.cgal");
  for(Skeleton_vertex v : CGAL::make_range(vertices(skeleton)))
    for(vertex_descriptor vd : skeleton[v].vertices)
      output << "2 " << skeleton[v].point << " "
                     << get(CGAL::vertex_point, mesh, vd)  << "\n";
  */

  return EXIT_SUCCESS;
}

