#include "drawable.h"

std::tuple<int, int, int> parse_face(const std::string& face, const size_t& v_size, const size_t& vt_size, const size_t& vn_size);
inline uint64_t make_vertex_key(const int& v, const int& vt, const int& vn) {
	// make a unique key from the v, vt, vn values such that we can avoid duplicates.
	uint64_t key = 0;
	key |= (uint64_t(v) & 0xFFFFF);
	key |= (uint64_t(vt & 0xFFFFF) << 20);
	key |= (uint64_t(vn & 0xFFFFF) << 40);
	return key;
}

Drawable import_from_obj(const std::string& filepath) {
	Drawable out;
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Error: couldn't open file " << filepath << std::endl;
		return out;
	}

	std::vector<Vec3> positions;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvs;

	// Maps every v/vt/vn combo to a unique index such that we do not end up with duplicates.
	std::unordered_map<uint64_t, uint32_t> vertex_index_map;
	
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream stream(line);
		std::string type;

		stream >> type;

		if (type.empty() || type == "#") continue;

		if (type == "v") {
			Vec3 v;
			stream >> v.x >> v.y >> v.z;
			positions.push_back(v);
		}
		else if (type == "vn") {
			Vec3 n;
			stream >> n.x >> n.y >> n.z;
			normals.push_back(n);
		}
		else if (type == "vt") {
			Vec2 uv;
			stream >> uv.x >> uv.y;
			uvs.push_back(uv);
		}
		else if (type == "o") {
			stream >> out.name;
		}
		else if (type == "f") {
			std::string vertex_string;
			std::vector<std::tuple<int, int, int>> face_vertices;
			int v_index, vt_index, vn_index;
			bool valid = true;
			while (stream >> vertex_string) {
				std::tie(v_index, vt_index, vn_index) = parse_face(vertex_string, positions.size(), uvs.size(), normals.size());

				if (v_index <= 0 || v_index > positions.size()) {
					valid = false;
					break;
				}
				if (vt_index != 0 && (vt_index <= 0 || vt_index > static_cast<int>(uvs.size()))) {
					valid = false;
					break;
				}
				if (vn_index != 0 && (vn_index <= 0 || vn_index > static_cast<int>(normals.size()))) {
					valid = false;
					break;
				}

				face_vertices.push_back({ v_index, vt_index, vn_index });
			}

			if (valid) {
				std::vector<uint32_t> indices;
				for (const auto& vertex_tuple : face_vertices) {
					int v_index = std::get<0>(vertex_tuple);
					int vt_index = std::get<1>(vertex_tuple);
					int vn_index = std::get<2>(vertex_tuple);

					uint64_t key = make_vertex_key(v_index, vt_index, vn_index);
					uint32_t index;

					auto it = vertex_index_map.find(key);
					if (it != vertex_index_map.end()) {
						index = it->second; // we use the already existing vertex rather than adding another.
					}
					else {
						// The vertex didn't exist so let's add it.
						Vertex vertex;
						vertex.pos = positions[v_index - 1];
						if (vt_index > 0) vertex.uv = uvs[vt_index - 1];
						if (vn_index > 0) vertex.normal = normals[vn_index - 1];

						index = static_cast<uint32_t>(out.vertices.size());
						out.vertices.push_back(vertex);
						vertex_index_map[key] = index;
					}
					indices.push_back(index);
				}

				for (size_t i = 1; i + 1 < indices.size(); i++) {
					Triangle t;
					t.a = indices[0];
					t.b = indices[i];
					t.c = indices[i + 1];
					out.triangles.push_back(t);
				}
			}
		}
	}
	file.close();
	return out;
}

std::tuple<int, int, int> parse_face(const std::string& face, const size_t& v_size, const size_t& vt_size, const size_t& vn_size) {
	int v = 0, vt = 0, vn = 0;
	const char* cursor = face.c_str();
	char* endptr = nullptr;
	// strtol will parse the string until it hits a character invalid for
	// base 10 (which will be the /)
	long val = std::strtol(cursor, &endptr, 10);
	if (endptr != cursor) v = static_cast<int>(val);

	if (*endptr == '/') {
		cursor = endptr + 1;
		if (*cursor == '/') {
			// v//n (vt missing)
			cursor++;
			val = std::strtol(cursor, &endptr, 10);
			if (endptr != cursor) vn = static_cast<int>(val);
		}
		else {
			val = std::strtol(cursor, &endptr, 10);
			if (endptr != cursor) vt = static_cast<int>(val);
			if (*endptr == '/') {
				cursor = endptr + 1;
				val = std::strtol(cursor, &endptr, 10);
				if (endptr != cursor) vn = static_cast<int>(val);
			}
		}
	}

	// indexes can be negative in obj files so -1 = most recent/last vertex, -2 = 2nd most...
	// so adjust them to reflect their actual index
	if (v < 0) v = static_cast<int>(v_size + v + 1);
	if (vt < 0) vt = static_cast<int>(vt_size + vt + 1);
	if (vn < 0) vn = static_cast<int>(vn_size + vn + 1);

	return { v, vt, vn };
}