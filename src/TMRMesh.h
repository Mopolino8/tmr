/*
  This file is part of the package TMR for adaptive mesh refinement.

  Copyright (C) 2015 Georgia Tech Research Corporation.
  Additional copyright (C) 2015 Graeme Kennedy.
  All rights reserved.

  TMR is licensed under the Apache License, Version 2.0 (the "License");
  you may not use this software except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifndef TMR_MESH_H
#define TMR_MESH_H

#include "TMRBase.h"
#include "TMRGeometry.h"
#include "TMRTopology.h"

/*
  The type of face mesh algorithm to apply
*/
enum TMRFaceMeshType { TMR_NO_MESH, 
                       TMR_STRUCTURED, 
                       TMR_UNSTRUCTURED,
                       TMR_TRIANGLE };

/*
  Global options for meshing
*/
class TMRMeshOptions {
 public:
  enum TriangleSmoothingType { TMR_LAPLACIAN, TMR_SPRING };

  /*
    Create the mesh options objects with the default settings
  */
  TMRMeshOptions(){
    // Set the default print level
    triangularize_print_level = 0;
    triangularize_print_iter = 1000;
    write_mesh_quality_histogram = 0;
    
    // Set the default meshing options
    mesh_type_default = TMR_STRUCTURED;
    num_smoothing_steps = 10;
    tri_smoothing_type = TMR_LAPLACIAN;
    frontal_quality_factor = 1.5;

    // By default, write nothing to any files
    write_init_domain_triangle = 0;
    write_triangularize_intermediate = 0;
    write_pre_smooth_triangle = 0;
    write_post_smooth_triangle = 0;
    write_dual_recombine = 0;
    write_pre_smooth_quad = 0;
    write_post_smooth_quad = 0;
    write_quad_dual = 0;
  }

  // Set the print level for the triangularize code
  int triangularize_print_level;
  int triangularize_print_iter;

  // Set the write level for the quality histogram
  int write_mesh_quality_histogram;

  // Options to control the meshing algorithm
  TMRFaceMeshType mesh_type_default;
  int num_smoothing_steps;
  TriangleSmoothingType tri_smoothing_type;
  double frontal_quality_factor;

  // Write intermediate surface meshes to file
  int write_init_domain_triangle;
  int write_triangularize_intermediate;
  int write_pre_smooth_triangle;
  int write_post_smooth_triangle;
  int write_dual_recombine;
  int write_pre_smooth_quad;
  int write_post_smooth_quad;
  int write_quad_dual;
};

/*
  The element feature size class
*/
class TMRElementFeatureSize : public TMREntity {
 public:
  TMRElementFeatureSize( double _hmin );
  virtual ~TMRElementFeatureSize();
  virtual double getFeatureSize( TMRPoint pt );

 protected:
  // The min local feature size
  double hmin;
};

/*
  Set a min/max element feature size
*/
class TMRLinearElementSize : public TMRElementFeatureSize {
 public:
  TMRLinearElementSize( double _hmin, double _hmax,
                        double c, double _ax, double _ay, double _az );
  ~TMRLinearElementSize();
  double getFeatureSize( TMRPoint pt );

 private:
  double hmax;
  double c, ax, ay, az;
};

/*
  Set a min/max feature size and feature sizes dictated within boxes
*/
class TMRBoxFeatureSize : public TMRElementFeatureSize {
 public:
  TMRBoxFeatureSize( TMRPoint p1, TMRPoint p2,
                     double _hmin, double _hmax );
  ~TMRBoxFeatureSize();
  void addBox( TMRPoint p1, TMRPoint p2, double h );
  double getFeatureSize( TMRPoint pt );

 private:
  // Maximum feature size
  double hmax;

  // Store the information about the points
  class BoxSize {
   public:
    // Check whether the box contains a point
    int contains( TMRPoint p );

    // Data for the box and its location
    TMRPoint m; // Center of the box
    TMRPoint d; // Half-edge length of each box
    double h; // Mesh size within the box
  };
  
  // The list of boxes that are stored
  int num_boxes;

  // A list object
  static const int MAX_LIST_BOXES = 256;
  class BoxList {
   public:
    BoxList(){ next = NULL; }
    BoxSize boxes[MAX_LIST_BOXES];
    BoxList *next;
  } *list_root, *list_current;

  // Store the information about the size
  class BoxNode {
   public:
    static const int MAX_NUM_BOXES = 10;

    BoxNode( BoxSize *cover, TMRPoint m, TMRPoint d );
    ~BoxNode();

    // Add a box
    void addBox( BoxSize *ptr );

    // Add a box to the data structure
    void getSize( TMRPoint pt, double *h );
    
    // The mid-point of the node and the distance from the mid-point
    // to the box sides
    TMRPoint m, d;

    // The children from this node
    BoxNode *c[8];

    // The number of boxes and the pointers to them
    int num_boxes;
    BoxSize **boxes;
  } *root;
};

/*
  The mesh for a geometric curve
*/
class TMREdgeMesh : public TMREntity {
 public:
  TMREdgeMesh( MPI_Comm _comm, TMREdge *edge );
  ~TMREdgeMesh();

  // Is this edge mesh degenerate
  int isDegenerate(){ return edge->isDegenerate(); }

  // Retrieve the underlying curve
  void getEdge( TMREdge **_edge );

  // Mesh the geometric object
  void mesh( TMRMeshOptions options, 
             TMRElementFeatureSize *fs );

  // Order the mesh points uniquely
  int setNodeNums( int *num );
  int getNodeNums( const int **_vars );

  // Retrieve the mesh points
  void getMeshPoints( int *_npts, const double **_pts, TMRPoint **X );

 private:
  MPI_Comm comm;
  TMREdge *edge;

  // The parametric locations of the points that are obtained from
  // meshing the curve
  int npts; // number of points along the curve
  double *pts; // Parametric node locations
  TMRPoint *X; // Physical node locations
  int *vars; // Global node variable numbers
};

/*
  Triangle info class

  This class is used to generate a triangular mesh
*/
class TMRFaceMesh : public TMREntity {
 public:
  TMRFaceMesh( MPI_Comm _comm, TMRFace *face );
  ~TMRFaceMesh();

  // Retrieve the underlying surface
  void getFace( TMRFace **_surface );
  
  // Mesh the underlying geometric object
  void mesh( TMRMeshOptions options, 
             TMRElementFeatureSize *fs );

  // Return the type of the underlying mesh
  TMRFaceMeshType getMeshType(){
    return mesh_type;
  }

  // Retrieve the mesh points
  void getMeshPoints( int *_npts, const double **_pts, TMRPoint **X );

  // Order the mesh points uniquely
  int setNodeNums( int *num );
  int getNodeNums( const int **_vars );
  int getNumFixedPoints();

  // Retrieve the local connectivity from this surface mesh
  int getQuadConnectivity( const int **_quads );
  int getTriConnectivity( const int **_tris );

  // Write the quadrilateral mesh to a VTK format
  void writeToVTK( const char *filename );

  // Print the quadrilateral quality
  void addMeshQuality( int nbins, int count[] );
  void printMeshQuality();

 private:
  // Write the segments to a VTK file in parameter space
  void writeSegmentsToVTK( const char *filename, 
                           int npts, const double *params, 
                           int nsegs, const int segs[] );

  // Print the triangle quality
  void printTriQuality( int ntris, const int tris[] );
  void writeTrisToVTK( const char *filename,
                       int ntris, const int tris[] );
  
  // Write the dual mesh - used for recombination - to a file
  void writeDualToVTK( const char *filename, int nodes_per_elem, 
                       int nelems, const int elems[],
                       int num_dual_edges, const int dual_edges[],
                       const TMRPoint *p );

  // Recombine the mesh to a quadrilateral mesh based on the
  // quad-Blossom algorithm
  void recombine( int ntris, const int tris[], const int tri_neighbors[],
                  const int node_to_tri_ptr[], const int node_to_tris[],
                  int num_edges, const int dual_edges[],
                  int *_num_quads, int **_new_quads,
                  TMRMeshOptions options );

  // Simplify the quadrilateral mesh to remove points that make
  // for a poor quadrilateral mesh
  void simplifyQuads();

  // Compute recombined triangle information
  int getRecombinedQuad( const int tris[], const int trineighbors[],
                         int t1, int t2, int quad[] );
  double computeRecombinedQuality( const int tris[], 
                                   const int trineighbors[],
                                   int t1, int t2, const TMRPoint *p ); 

  // Compute the quadrilateral quality
  double computeQuadQuality( const int *quad, const TMRPoint *p );
  double computeTriQuality( const int *tri, const TMRPoint *p );

  // The underlying surface
  MPI_Comm comm;
  TMRFace *face;

  // The actual type of mesh used to mesh the structure
  TMRFaceMeshType mesh_type;

  // Points in the mesh
  int num_fixed_pts; // number of fixed points
  int num_points; // The number of point locations
  double *pts; // The parametric node locations
  TMRPoint *X; // The physical node locations
  int *vars; // The global variable numbers

  // Quadrilateral surface mesh information
  int num_quads;
  int *quads;

  // Triangle mesh surface information
  int num_tris;
  int *tris;
};

/*
  TMRVolumeMesh class

  This is the class that contains the volume mesh. This mesh 
  takes in the arguments needed to form a volume mesh
*/
class TMRVolumeMesh : public TMREntity {
 public:
  TMRVolumeMesh( MPI_Comm _comm, TMRVolume *volume );
  ~TMRVolumeMesh();
  
  // Create the volume mesh
  int mesh( TMRMeshOptions options );

  // Retrieve the mesh points
  void getMeshPoints( int *_npts, TMRPoint **X );

  // Retrieve the local connectivity from this volume mesh
  int getHexConnectivity( const int **hex );
  int getTetConnectivity( const int **tets );

  // Order the mesh points uniquely
  int setNodeNums( int *num );
  int getNodeNums( const int **_vars );

  // Write the volume mesh to a VTK file
  void writeToVTK( const char *filename );

 private:
  // Create a tetrahedral mesh (if possible)
  int tetMesh( TMRMeshOptions options );

  // The underlying volume
  MPI_Comm comm;
  TMRVolume *volume;

  // Set the number of face loops
  int num_face_loops;
  int *face_loop_ptr;
  TMRFace **face_loops;
  int *face_loop_dir;
  int *face_loop_edge_count;

  // Number of points through-thickness
  int num_depth_pts;

  // Keep the bottom/top surfaces (master/target) in the mesh for
  // future reference
  TMRFace *target, *source;
  int target_dir, source_dir;

  int num_points; // The number of points
  TMRPoint *X; // The physical node locations
  int *vars; // The global variable numbers

  // Hexahedral mesh information
  int num_hex;
  int *hex;

  // Tetrahedral mesh information
  int num_tet;
  int *tet;
};

/*
  Mesh the geometry model.

  This class handles the meshing for surface objects without any
  additional information. For hexahedral meshes, the model must
*/
class TMRMesh : public TMREntity {
 public:
  static const int TMR_QUAD = 1;
  static const int TMR_HEX = 2;

  TMRMesh( MPI_Comm _comm, TMRModel *_geo );
  ~TMRMesh();

  // Mesh the underlying geometry
  void mesh( TMRMeshOptions options, double htarget );
  void mesh( TMRMeshOptions options, 
             TMRElementFeatureSize *fs );

  // Write the mesh to a VTK file
  void writeToVTK( const char *filename, 
                   int flag=(TMRMesh::TMR_QUAD | TMRMesh::TMR_HEX) );

  // Write the mesh to a BDF file
  void writeToBDF( const char *filename,
                   int flag=(TMRMesh::TMR_QUAD | TMRMesh::TMR_HEX) );

  // Retrieve the mesh components
  int getMeshPoints( TMRPoint **_X );
  void getQuadConnectivity( int *_nquads, const int **_quads );
  void getTriConnectivity( int *_ntris, const int **_tris );
  void getHexConnectivity( int *_nhex, const int **_hex );
  // void getQuadConnectivity( int *_nquads, const int **_quads );

  // Create a topology object (with underlying mesh geometry)
  TMRModel* createModelFromMesh();

 private:
  // Allocate and initialize the underlying mesh
  void initMesh( int count_nodes=0 );

  // Reset the mesh
  void resetMesh();

  // The underlying geometry object
  MPI_Comm comm;
  TMRModel *geo;

  // The number of nodes/positions in the mesh
  int num_nodes;
  TMRPoint *X;  

  // The number of quads
  int num_quads;
  int *quads;

  // The number of triangles
  int num_tris;
  int *tris;

  // The number of hexahedral elements in the mesh
  int num_hex;
  int *hex;

  // The number of tetrahedral elements
  int num_tet;
  int *tet;
};

#endif // TMR_MESH_H
