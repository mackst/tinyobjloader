#include <pybind11\pybind11.h>
#include <pybind11\stl.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


namespace py = pybind11;


py::tuple loadModel(std::string modelfile) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelfile.c_str())) {
		std::exception(err);
	}

	py::dict uniqueVertices;
	py::list vertices;
	py::list indices;

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			py::tuple vertex = py::make_tuple(
				// vertex position
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2],
				// color
				1.0, 1.0, 1.0,
				// texcoord
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			);

			if (!uniqueVertices.contains(vertex)) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.append(vertex);
			}

			indices.append(uniqueVertices[vertex]);
		}
	}

	return py::make_tuple(vertices, indices);
}




PYBIND11_MODULE(tol, m) {
	m.doc() = "tinyobjloader for python(for vulkan tutorial model loading demo)";

	m.def("loadModel", &loadModel, "load an obj model.");
}


