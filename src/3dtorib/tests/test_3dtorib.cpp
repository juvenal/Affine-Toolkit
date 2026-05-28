#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "../3dtorib.hpp"

using namespace threedtorib;

TEST_CASE("Geometry Transformations: RHS Y-up to LHS Y-up", "[transform]") {
    Mesh mesh;
    mesh.points = {{1.0f, 2.0f, 3.0f}};
    mesh.faces = {{{0, 1, 2}}}; // Dummy indices for winding test

    transformToLHSYUp(mesh, Orientation::RHS_Y_UP);

    // X and Y stay same, Z is negated
    REQUIRE(mesh.points[0].x == 1.0f);
    REQUIRE(mesh.points[0].y == 2.0f);
    REQUIRE(mesh.points[0].z == -3.0f);

    // Winding order reversed
    REQUIRE(mesh.faces[0].indices[0] == 0);
    REQUIRE(mesh.faces[0].indices[1] == 2);
    REQUIRE(mesh.faces[0].indices[2] == 1);
}

TEST_CASE("Geometry Transformations: RHS Z-up to LHS Y-up", "[transform]") {
    Mesh mesh;
    // In RHS Z-up: X=right, Y=forward, Z=up
    // To LHS Y-up: X=right, Y=up, Z=forward (but Z negated for LHS?)
    // Actually standard RenderMan is LHS Y-up.
    // RHS Z-up: (x, y, z) -> RHS Y-up: (x, z, -y) -> LHS Y-up: (x, z, y) ?
    // Let's define the conversion logic clearly.
    // If we want LHS Y-up:
    // RHS Y-up: (x, y, z) -> (x, y, -z) [and flip winding]
    // RHS Z-up: (x, y, z) -> (x, z, y)  [and flip winding?]
    
    mesh.points = {{1.0f, 2.0f, 3.0f}}; // X=1, Y=2 (forward), Z=3 (up)
    mesh.faces = {{{0, 1, 2}}};

    transformToLHSYUp(mesh, Orientation::RHS_Z_UP);

    // Expected: X=1, Y=3 (up), Z=2 (forward)
    // We also need to check winding order.
    REQUIRE(mesh.points[0].x == 1.0f);
    REQUIRE(mesh.points[0].y == 3.0f);
    REQUIRE(mesh.points[0].z == 2.0f);
    
    // For RHS Z-up to LHS Y-up, we'll see if winding needs flip.
    // Standard conversion involves an odd number of reflections usually.
}

#include <fstream>
#include <cstdio>

TEST_CASE("STL Parser: ASCII", "[stl]") {
    const std::string filename = "test_ascii.stl";
    std::ofstream ofs(filename);
    ofs << "solid test\n"
        << "  facet normal 0 0 1\n"
        << "    outer loop\n"
        << "      vertex 0 0 0\n"
        << "      vertex 1 0 0\n"
        << "      vertex 0 1 0\n"
        << "    endloop\n"
        << "  endfacet\n"
        << "endsolid test\n";
    ofs.close();

    Mesh mesh = parseSTL(filename);
    REQUIRE(mesh.points.size() == 3);
    REQUIRE(mesh.faces.size() == 1);
    REQUIRE(mesh.faces[0].indices.size() == 3);
    
    // Check points
    REQUIRE(mesh.points[0].x == 0.0f);
    REQUIRE(mesh.points[1].x == 1.0f);
    REQUIRE(mesh.points[2].y == 1.0f);

    std::remove(filename.c_str());
}

TEST_CASE("STL Parser: Binary", "[stl]") {
    const std::string filename = "test_binary.stl";
    std::ofstream ofs(filename, std::ios::binary);
    
    char header[80] = {0};
    ofs.write(header, 80);
    
    uint32_t num_facets = 1;
    ofs.write(reinterpret_cast<const char*>(&num_facets), 4);
    
    float normal[3] = {0, 0, 1};
    float v1[3] = {0, 0, 0};
    float v2[3] = {1, 0, 0};
    float v3[3] = {0, 1, 0};
    uint16_t attribute_byte_count = 0;
    
    ofs.write(reinterpret_cast<const char*>(normal), 12);
    ofs.write(reinterpret_cast<const char*>(v1), 12);
    ofs.write(reinterpret_cast<const char*>(v2), 12);
    ofs.write(reinterpret_cast<const char*>(v3), 12);
    ofs.write(reinterpret_cast<const char*>(&attribute_byte_count), 2);
    
    ofs.close();

    Mesh mesh = parseSTL(filename);
    REQUIRE(mesh.points.size() == 3);
    REQUIRE(mesh.faces.size() == 1);
    
    std::remove(filename.c_str());
}

TEST_CASE("OBJ Parser", "[obj]") {
    const std::string filename = "test.obj";
    std::ofstream ofs(filename);
    ofs << "v 0 0 0\n"
        << "v 1 0 0\n"
        << "v 0 1 0\n"
        << "f 1 2 3\n";
    ofs.close();

    Mesh mesh = parseOBJ(filename);
    REQUIRE(mesh.points.size() == 3);
    REQUIRE(mesh.faces.size() == 1);
    REQUIRE(mesh.faces[0].indices.size() == 3);
    REQUIRE(mesh.faces[0].indices[0] == 0);
    REQUIRE(mesh.faces[0].indices[1] == 1);
    REQUIRE(mesh.faces[0].indices[2] == 2);

    std::remove(filename.c_str());
}

TEST_CASE("RIB Generation", "[rib]") {
    Mesh mesh;
    mesh.points = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
    mesh.faces = {{{0, 1, 2}}};

    std::string rib = generateRIB(mesh, "test_mesh");
    
    REQUIRE(rib.find("AttributeBegin") != std::string::npos);
    REQUIRE(rib.find("Attribute \"identifier\" \"string name\" [\"test_mesh\"]") != std::string::npos);
    REQUIRE(rib.find("PointsPolygons") != std::string::npos);
    REQUIRE(rib.find("[3]") != std::string::npos); // nvertices
    REQUIRE(rib.find("[0 1 2]") != std::string::npos); // vertices
    REQUIRE(rib.find("\"P\" [0 0 0 1 0 0 0 1 0]") != std::string::npos);
    REQUIRE(rib.find("AttributeEnd") != std::string::npos);
}
