#include "serialize.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include <boost/tokenizer.hpp>

#include "../shader_core/config.h"
#include "../shader_core/rect.h"
#include "../shader_core/vector.h"

#include "curves.h"
#include "ext_base64.h"
#include "graph.h"
#include "node.h"
#include "node_enums.h"
#include "node_id.h"
#include "node_type.h"
#include "ramp.h"
#include "slot.h"
#include "slot_id.h"

static const char* const MAGIC_WORD{ "cycles_shader" };
static const char* const VERSION_INPUT{ "1" };
static const char* const VERSION_OUTPUT{ "1" };

static const char* const SECTION_CONNECTIONS{ "section_connections" };
static const char* const SECTION_NODES      { "section_nodes" };

static const char* const NODE_END{ "node_end" };

static const char* const NODE_ID_PREFIX{ "_nodeid_" };

static std::string name_from_node_id(const csg::NodeId node_id)
{
	const size_t size_src{ sizeof(csg::NodeId) };
	const size_t size_dest{ ext::base64::encoded_size(size_src) };
	std::array<char, 24> result_array;
	assert(size_dest < result_array.size());
	result_array.fill('\0');
	ext::base64::encode(result_array.data(), &node_id, size_src);
	return std::string{ NODE_ID_PREFIX } + result_array.data();
}

static std::string serialize_curve(const csg::Curve& curve)
{
	constexpr char SEPARATOR{ ',' };

	const auto interp_as_char = [](const csg::CurveInterp interp) -> char
	{
		switch (interp) {
		case csg::CurveInterp::LINEAR:
			return 'l';
		case csg::CurveInterp::CUBIC_HERMITE:
			return 'h';
		default:
			return '?';
		}
	};

	std::stringstream sstream;
	sstream << std::fixed << std::setprecision(SERIALIZED_GRAPH_PRECISION);

	bool write_opening_separator{ false };
	for (const csg::CurvePoint& point : curve.control_points()) {
		if (write_opening_separator) {
			sstream << SEPARATOR;
		}
		sstream << point.pos.x << SEPARATOR << point.pos.y << SEPARATOR << interp_as_char(point.interp);
		write_opening_separator = true;
	}
	return sstream.str();
}

static std::string serialize_slot_value(const csg::SlotValue& slot_value)
{
	switch (slot_value.type()) {
	case csg::SlotType::BOOL:
		if (slot_value.as<csg::BoolSlotValue>().has_value()) {
			const csg::BoolSlotValue bool_slot_value{ slot_value.as<csg::BoolSlotValue>().value() };
			if (bool_slot_value.get()) {
				return "1";
			}
			else {
				return "0";
			}
		}
		break;
	case csg::SlotType::COLOR:
		if (slot_value.as<csg::ColorSlotValue>().has_value()) {
			const csg::ColorSlotValue color_slot_value{ slot_value.as<csg::ColorSlotValue>().value() };
			const csc::Float3 color{ color_slot_value.get() };
			std::stringstream sstream;
			sstream << std::fixed << std::setprecision(SERIALIZED_GRAPH_PRECISION) << color.x << ',' << color.y << ',' << color.z;
			return sstream.str();
		}
		break;
	case csg::SlotType::ENUM:
		if (slot_value.as<csg::EnumSlotValue>().has_value()) {
			const csg::EnumSlotValue enum_slot_value{ slot_value.as<csg::EnumSlotValue>().value() };
			return std::string{ enum_slot_value.internal_name() };
		}
		break;
	case csg::SlotType::FLOAT:
		if (slot_value.as<csg::FloatSlotValue>().has_value()) {
			const csg::FloatSlotValue float_slot_value{ slot_value.as<csg::FloatSlotValue>().value() };
			std::stringstream sstream;
			sstream << std::fixed << std::setprecision(SERIALIZED_GRAPH_PRECISION) << float_slot_value.get();
			return sstream.str();
		}
		break;
	case csg::SlotType::INT:
		if (slot_value.as<csg::IntSlotValue>().has_value()) {
			const csg::IntSlotValue int_slot_value{ slot_value.as<csg::IntSlotValue>().value() };
			std::stringstream sstream;
			sstream << int_slot_value.get();
			return sstream.str();
		}
		break;
	case csg::SlotType::VECTOR:
		if (slot_value.as<csg::VectorSlotValue>().has_value()) {
			const csg::VectorSlotValue vec_slot_value{ slot_value.as<csg::VectorSlotValue>().value() };
			const csc::Float3 color{ vec_slot_value.get() };
			std::stringstream sstream;
			sstream << std::fixed << std::setprecision(SERIALIZED_GRAPH_PRECISION) << color.x << ',' << color.y << ',' << color.z;
			return sstream.str();
		}
		break;
	case csg::SlotType::CURVE_RGB:
		if (slot_value.as<csg::RGBCurveSlotValue>().has_value()) {
			constexpr char CURVE_SEPARATOR{ '/' };
			const csg::RGBCurveSlotValue rgb_slot_value{ slot_value.as<csg::RGBCurveSlotValue>().value() };
			std::stringstream sstream;
			sstream << std::fixed << std::setprecision(SERIALIZED_GRAPH_PRECISION);
			sstream << "curve_rgb_00" << CURVE_SEPARATOR << "00" << CURVE_SEPARATOR;
			sstream << serialize_curve(rgb_slot_value.get_all()) << CURVE_SEPARATOR;
			sstream << serialize_curve(rgb_slot_value.get_r()) << CURVE_SEPARATOR;
			sstream << serialize_curve(rgb_slot_value.get_g()) << CURVE_SEPARATOR;
			sstream << serialize_curve(rgb_slot_value.get_b());
			return sstream.str();
		}
		break;
	case csg::SlotType::CURVE_VECTOR:
		if (slot_value.as<csg::VectorCurveSlotValue>().has_value()) {
			constexpr char CURVE_SEPARATOR{ '/' };
			const csg::VectorCurveSlotValue curve_slot_value{ slot_value.as<csg::VectorCurveSlotValue>().value() };
			std::stringstream sstream;
			sstream << std::fixed << std::setprecision(SERIALIZED_GRAPH_PRECISION);
			sstream << "curve_vec_00" << CURVE_SEPARATOR << "00" << CURVE_SEPARATOR;
			sstream << curve_slot_value.get_min().x << "," << curve_slot_value.get_min().y << CURVE_SEPARATOR;
			sstream << curve_slot_value.get_max().x << "," << curve_slot_value.get_max().y << CURVE_SEPARATOR;
			sstream << serialize_curve(curve_slot_value.get_x()) << CURVE_SEPARATOR;
			sstream << serialize_curve(curve_slot_value.get_y()) << CURVE_SEPARATOR;
			sstream << serialize_curve(curve_slot_value.get_z());
			return sstream.str();
		}
		break;
	case csg::SlotType::COLOR_RAMP:
		if (slot_value.as<csg::ColorRampSlotValue>().has_value()) {
			constexpr char RAMP_SEPARATOR{ ',' };
			const csg::ColorRampSlotValue ramp_slot_value{ slot_value.as<csg::ColorRampSlotValue>().value() };
			std::stringstream sstream;
			sstream << std::fixed << std::setprecision(SERIALIZED_GRAPH_PRECISION);
			sstream << "ramp00";
			for (const auto& this_point : ramp_slot_value.get().get()) {
				const float a = this_point.pos;
				const float b = this_point.color.x;
				const float c = this_point.color.y;
				const float d = this_point.color.z;
				const float e = this_point.alpha;
				sstream << RAMP_SEPARATOR << a << RAMP_SEPARATOR << b << RAMP_SEPARATOR << c << RAMP_SEPARATOR << d << RAMP_SEPARATOR << e;
			}
			return sstream.str();
		}
		break;
    case csg::SlotType::IMAGE:
        if (slot_value.as<csg::ImageSlotValue>().has_value()) {
            const csg::ImageSlotValue image_slot_value{ slot_value.as<csg::ImageSlotValue>().value() };
            std::stringstream sstream;
            sstream << image_slot_value.get();
            return sstream.str();
        }
        break;
	default:
		assert(false);
		break;
	}

	return "ERROR";
}

std::string csg::serialize_graph(const Graph& graph)
{
	// Make a local sorted copy of all nodes and connections
	const auto& graph_connections = graph.connections();
	std::vector<Connection> connections{ graph_connections.begin(), graph_connections.end() };
	std::sort(connections.begin(), connections.end());

	std::vector<Node> nodes;
	for (const auto& node : graph.nodes()) {
		nodes.push_back(*node);
	}
	std::sort(nodes.begin(), nodes.end(),
		[](const Node& a, const Node& b) {
			return a.id() < b.id();
		}
	);

	std::stringstream result_stream;

	// Header
	result_stream << MAGIC_WORD << "|" << VERSION_OUTPUT << "|";

	// Node section
	result_stream << SECTION_NODES << "|";
	std::map<NodeId, std::string> names_by_id;
	for (const Node& node : nodes) {
		const std::string node_name{ name_from_node_id(node.id()) };
		names_by_id[node.id()] = node_name;
		const boost::optional<NodeTypeInfo> info{ NodeTypeInfo::from(node.type()) };
		if (info.has_value()) {
			result_stream << info->name() << "|" << node_name << "|";
			result_stream << node.position.x << "|" << node.position.y << "|";
			for (const Slot& slot : node.slots()) {
				if (slot.dir() == SlotDirection::INPUT && slot.value.has_value()) {
					result_stream << slot.name() << "|" << serialize_slot_value(slot.value.value()) << "|";
				}
			}
			result_stream << "node_end|";
		}
		else {
			continue;
		}
	}

	// Connection section
	result_stream << SECTION_CONNECTIONS << "|";

	for (const Connection& connection : connections) {
		const csg::NodeId id_src{ connection.source().node_id() };
		const csg::NodeId id_dest{ connection.dest().node_id() };
		if (names_by_id.count(id_src) == 0 || names_by_id.count(id_dest) == 0) {
			// Either source or dest node does not exist, ignore this connection
			continue;
		}
		const std::string name_src{ names_by_id[id_src] };
		const std::string name_dest{ names_by_id[id_dest] };
		const auto opt_node_src{ graph.get(id_src) };
		const auto opt_node_dest{ graph.get(id_dest) };

		// We already checked that these nodes exist, so the optionals here should always hold a value
		assert(opt_node_src.use_count() > 0);
		assert(opt_node_dest.use_count() > 0);

		// Now we need to get the slots to find the slot names
		const auto opt_slot_src{ opt_node_src->slot(connection.source().index()) };
		const auto opt_slot_dest{ opt_node_dest->slot(connection.dest().index()) };
		if (opt_slot_src.has_value() == false || opt_slot_dest.has_value() == false) {
			// One of the slots is not real, ignore this connection
			continue;
		}

		result_stream << name_src << "|" << opt_slot_src->disp_name() << "|";
		result_stream << name_dest << "|" << opt_slot_dest->disp_name() << "|";
	}

	return result_stream.str();
}

// Check whether iter is able to successfully increment count times without becoming equal to end
template<typename T>
static bool iter_has_contents(T iter, const T end, size_t count)
{
	while (count > 0) {
		if (iter == end) {
			return false;
		}
		count--;
		iter++;
	}
	return true;
}

// Normal stoi/stof will use system locale which may be a problem, this way uses default locale

static int my_stoi(const std::string& input)
{
	std::stringstream stream{ input };
	int result{ 0 };
	stream >> result;
	return result;
}

static float my_stof(const std::string& input)
{
	std::stringstream stream{ input };
	float result{ 0.0f };
	stream >> result;
	return result;
}

static csc::Float2 my_stof2(const std::string& input)
{
	const boost::char_separator<char> sep{ "," };
	const boost::tokenizer<boost::char_separator<char>> tokenizer{ input, sep };

	auto token_iter = tokenizer.begin();
	if (iter_has_contents(token_iter, tokenizer.end(), 2) == false) {
		// Did not find the two expected comma-separated floats
		return csc::Float2{};
	}

	csc::Float2 result;
	result.x = my_stof(*token_iter++);
	result.y = my_stof(*token_iter++);
	return result;
}

static csc::Float3 my_stof3(const std::string& input)
{
	const boost::char_separator<char> sep{ "," };
	const boost::tokenizer<boost::char_separator<char>> tokenizer{ input, sep };

	auto token_iter = tokenizer.begin();
	if (iter_has_contents(token_iter, tokenizer.end(), 3) == false) {
		// Did not find the three expected comma-separated floats
		return csc::Float3{};
	}

	csc::Float3 result;
	result.x = my_stof(*token_iter++);
	result.y = my_stof(*token_iter++);
	result.z = my_stof(*token_iter++);
	return result;
}

static boost::optional<csg::NodeId> node_id_from_name(const std::string& node_name)
{
	if (node_name.find(NODE_ID_PREFIX) != 0) {
		// This name does not have the right prefix
		return boost::none;
	}

	const size_t prefix_size = strlen(NODE_ID_PREFIX);
	const std::string id_encoded{ node_name.substr(prefix_size, node_name.size() - prefix_size) };
	std::array<char, 32> buffer;
	buffer.fill('\0');
	const std::pair<size_t, size_t> decode_result = ext::base64::decode(buffer.data(), id_encoded.c_str(), std::min(static_cast<size_t>(16), id_encoded.size()));
	if (decode_result.first < sizeof(csg::NodeId)) {
		// ID was not long enough
		return boost::none;
	}

	const csg::NodeId result{ *( reinterpret_cast<csg::NodeId*>( buffer.data() ) ) };

	return result;
}

static boost::optional<csg::NodeType> get_type_from_name(const std::string& type_name)
{
	const boost::optional<csg::NodeTypeInfo> opt_type_info{ csg::NodeTypeInfo::from(type_name.c_str()) };
	if (opt_type_info.has_value()) {
		return opt_type_info->type();
	}
	else {
		return boost::none;
	}
}

static csg::CurveInterp get_interp(const std::string symbol)
{
	if (symbol == "l") {
		return csg::CurveInterp::LINEAR;
	}
	else {
		return csg::CurveInterp::CUBIC_HERMITE;
	}
}

static boost::optional<csg::Curve> deserialize_curve(const std::string& curve, const csc::Float2 min, const csc::Float2 max)
{
	const boost::char_separator<char> sep{ "," };
	const boost::tokenizer<boost::char_separator<char>> tokenizer{ curve, sep };

	auto token_iter = tokenizer.begin();

	std::vector<csg::CurvePoint> points;
	while (iter_has_contents(token_iter, tokenizer.end(), 3)) {
		const float x{ my_stof(*token_iter++) };
		const float y{ my_stof(*token_iter++) };
		const csg::CurveInterp interp{ get_interp(*token_iter++) };
		points.push_back(csg::CurvePoint{ csc::Float2{ x, y }, interp });
	}

	if (points.size() < 2) {
		return boost::none;
	}
	else {
		return csg::Curve{ min, max, points };
	}
}

static boost::optional<csg::RGBCurveSlotValue> deserialize_rgb_curve(const std::string& rgb_curve)
{
	const boost::char_separator<char> sep{ "/" };
	const boost::tokenizer<boost::char_separator<char>> tokenizer{ rgb_curve, sep };

	auto token_iter = tokenizer.begin();

	if (iter_has_contents(token_iter, tokenizer.end(), 6) == false) {
		return boost::none;
	}

	const std::string curve_type{ *token_iter++ };
	const std::string curve_version{ *token_iter++ };

	const csc::Float2 min{ 0.0f, 0.0f };
	const csc::Float2 max{ 1.0f, 1.0f };

	const boost::optional<csg::Curve> opt_all{ deserialize_curve(*token_iter++, min, max) };
	const boost::optional<csg::Curve> opt_r{ deserialize_curve(*token_iter++, min, max) };
	const boost::optional<csg::Curve> opt_g{ deserialize_curve(*token_iter++, min, max) };
	const boost::optional<csg::Curve> opt_b{ deserialize_curve(*token_iter++, min, max) };

	csg::RGBCurveSlotValue result{};
	if (opt_all) {
		result.set_all(*opt_all);
	}
	if (opt_r) {
		result.set_r(*opt_r);
	}
	if (opt_g) {
		result.set_g(*opt_g);
	}
	if (opt_b) {
		result.set_b(*opt_b);
	}

	return result;
}

static boost::optional<csg::VectorCurveSlotValue> deserialize_vector_curve(const std::string& vector_curve)
{
	const boost::char_separator<char> sep{ "/" };
	const boost::tokenizer<boost::char_separator<char>> tokenizer{ vector_curve, sep };

	auto token_iter = tokenizer.begin();

	if (iter_has_contents(token_iter, tokenizer.end(), 7) == false) {
		return boost::none;
	}

	const std::string curve_type{ *token_iter++ };
	const std::string curve_version{ *token_iter++ };
	const csc::Float2 min{ my_stof2(*token_iter++) };
	const csc::Float2 max{ my_stof2(*token_iter++) };

	// Check validity of min and max before proceeding
	if (min.x >= max.x || min.y >= max.y) {
		return boost::none;
	}

	const boost::optional<csg::Curve> opt_x{ deserialize_curve(*token_iter++, min, max) };
	const boost::optional<csg::Curve> opt_y{ deserialize_curve(*token_iter++, min, max) };
	const boost::optional<csg::Curve> opt_z{ deserialize_curve(*token_iter++, min, max) };

	csg::VectorCurveSlotValue result{ min, max };
	if (opt_x) {
		result.set_x(*opt_x);
	}
	if (opt_y) {
		result.set_y(*opt_y);
	}
	if (opt_z) {
		result.set_z(*opt_z);
	}

	return result;
}

// For deserializing the old curve format
// This program can only read this format, not write it
static boost::optional<csg::Curve> deserialize_legacy_curve(const std::string& curve_string)
{
	const boost::char_separator<char> sep{ "," };
	const boost::tokenizer<boost::char_separator<char>> tokenizer{ curve_string, sep };

	auto token_iter{ tokenizer.begin() };

	if (iter_has_contents(token_iter, tokenizer.end(), 3) == false) {
		return boost::none;
	}

	const std::string identifier = *token_iter++;
	const std::string interpolation_str = *token_iter++;
	const std::string control_point_count_str = *token_iter++;

	if (identifier != "curve00") {
		return boost::none;
	}

	const auto get_interp_type = [](const std::string& name) -> csg::CurveInterp
	{
		if (name == "cubic_hermite") {
			return csg::CurveInterp::CUBIC_HERMITE;
		}
		else {
			return csg::CurveInterp::LINEAR;
		}
	};
	const csg::CurveInterp point_interp{ get_interp_type(interpolation_str) };

	const size_t point_count{ static_cast<size_t>(my_stoi(control_point_count_str)) };

	if (iter_has_contents(token_iter, tokenizer.end(), point_count * 2) == false) {
		return boost::none;
	}

	const csc::FloatRect valid_rect{ csc::Float2{ 0.0f, 0.0f }, csc::Float2{ 1.0f, 1.0f} };
	std::vector<csg::CurvePoint> points;
	for (size_t i = 0; i < point_count; i++) {
		const float x{ my_stof(*token_iter++) };
		const float y{ my_stof(*token_iter++) };
		const csc::Float2 pos{ x, y };
		if (valid_rect.contains(pos)) {
			points.push_back(csg::CurvePoint{ pos, point_interp });
		}
	}

	// Valid curves must have at least 2 points
	if (points.size() < 2) {
		return boost::none;
	}

	return csg::Curve{ valid_rect.begin(), valid_rect.end(), points };
}

static boost::optional<csg::ColorRamp> deserialize_ramp(const std::string& ramp_string)
{
	using namespace csg;

	const boost::char_separator<char> sep{ "," };
	const boost::tokenizer<boost::char_separator<char>> tokenizer{ ramp_string, sep };

	auto token_iter{ tokenizer.begin() };

	if (iter_has_contents(token_iter, tokenizer.end(), 1) == false) {
		return boost::none;
	}

	const std::string identifier = *token_iter++;
	if (identifier != "ramp00") {
		return boost::none;
	}

	std::vector<ColorRampPoint> ramp_points;
	while (iter_has_contents(token_iter, tokenizer.end(), 5)) {
		const float pos{ my_stof(*token_iter++) };
		const float r{ my_stof(*token_iter++) };
		const float g{ my_stof(*token_iter++) };
		const float b{ my_stof(*token_iter++) };
		const float a{ my_stof(*token_iter++) };
		const ColorRampPoint this_point{ pos, csc::Float3{ r, g, b }, a };
		ramp_points.push_back(this_point);
	}

	if (ramp_points.size() < 2) {
		return boost::none;
	}

	return ColorRamp{ ramp_points };
}

boost::optional<csg::Graph> csg::deserialize_graph(const std::string& graph_string)
{
	const boost::char_separator<char> sep{ "|" };
	const boost::tokenizer<boost::char_separator<char>> tokenizer{ graph_string, sep };

	auto token_iter = tokenizer.begin();

	if (token_iter == tokenizer.end() || *token_iter != MAGIC_WORD) {
		return boost::none;
	}
	else {
		++token_iter;
	}

	if (token_iter == tokenizer.end() || *token_iter != VERSION_INPUT) {
		return boost::none;
	}
	else {
		++token_iter;
	}

	csg::Graph result{ GraphType::EMPTY };

	// Advance iterator until we find the start of the node section
	while (token_iter != tokenizer.end() && *token_iter != SECTION_NODES) {
		token_iter++;
	}

	if (token_iter == tokenizer.end()) {
		// No nodes or connection section in the input, return empty graph
		return result;
	}

	// Map to let us look up node ids by name
	// This will be needed for building connections later in this function
	std::map<std::string, NodeId> ids_by_name;

	// Advance token to the start of the first node
	token_iter++;

	// Loop adding nodes until we see the connections header
	while (token_iter != tokenizer.end() && *token_iter != SECTION_CONNECTIONS) {
		constexpr size_t NODE_MIN_TOKENS{ 5 }; // type, name, x, y, node_end
		if (iter_has_contents(token_iter, tokenizer.end(), NODE_MIN_TOKENS) == false) {
			// Not enough tokens exist to form a node, end here
			return result;
		}
		assert(token_iter != tokenizer.end());
		const std::string type_code{ *token_iter++ };
		assert(token_iter != tokenizer.end());
		const std::string node_name{ *token_iter++ };
		assert(token_iter != tokenizer.end());
		const int x{ my_stoi(*token_iter++) };
		assert(token_iter != tokenizer.end());
		const int y{ my_stoi(*token_iter++) };

		const boost::optional<NodeType> opt_node_type{ get_type_from_name(type_code) };
		if (opt_node_type.has_value() == false) {
			// We do not recognize this type code
			// Advance the iterator past this node and continue
			while (token_iter != tokenizer.end() && *token_iter != NODE_END) {
				token_iter++;
			}
			if (*token_iter == NODE_END) {
				token_iter++;
			}
			continue;
		}

		const boost::optional<NodeId> opt_node_id{ node_id_from_name(node_name) };
		NodeId node_id;
		if (opt_node_id.has_value()) {
			node_id = opt_node_id.value();
			if (result.add(opt_node_type.value(), csc::Int2{ x, y }, node_id) == false) {
				// This node was not added because it is a duplicate id
				// This can randomly happen but should be very rare (birthday problem with a 64 bit number)
				// except in malformed graphs
				return boost::none;
			}
		}
		else {
			node_id = result.add(opt_node_type.value(), csc::Int2{ x, y });
		}
		
		if (ids_by_name.count(node_name) != 0) {
			// This name has already been used
			// The graph is invalid, abort processing here
			return boost::none;
		}
		ids_by_name[node_name] = node_id;

		// Load in all input/value pairs
		while (token_iter != tokenizer.end() && *token_iter != NODE_END && iter_has_contents(token_iter, tokenizer.end(), 2)) {
			assert(token_iter != tokenizer.end());
			const std::string input_name{ *token_iter++ };
			assert(token_iter != tokenizer.end());
			const std::string input_value{ *token_iter++ };

			const std::shared_ptr<const Node> node{ result.get(node_id) };
			assert(node.use_count() > 0);

			const boost::optional<size_t> opt_slot_index{ node->slot_index(SlotDirection::INPUT, input_name) };
			if (opt_slot_index.has_value()) {
				const boost::optional<Slot> opt_slot = node->slot(*opt_slot_index);
				assert(opt_slot.has_value());
				if (opt_slot->value.has_value()) {
					const SlotId slot_id{ node->id(), *opt_slot_index };
					// Choose how we interpret 'input_value' based on the slot type
					switch (opt_slot.value().type()) {
					case SlotType::BOOL:
					{
						const bool bool_value{ static_cast<bool>(my_stoi(input_value)) };
						result.set_bool(slot_id, bool_value);
						break;
					}
					case SlotType::COLOR:
					{
						const csc::Float3 float3_value{ my_stof3(input_value) };
						result.set_color(slot_id, float3_value);
						break;
					}
					case SlotType::ENUM:
					{
						const boost::optional<EnumSlotValue> slot_value{ opt_slot->value->as<EnumSlotValue>() };
						if (slot_value) {
							const NodeMetaEnum meta_enum{ slot_value->get_meta() };
							assert(NodeEnumInfo::from(meta_enum).has_value());
							const NodeEnumInfo enum_info{ NodeEnumInfo::from(meta_enum).value() };
							// Loop though all available internal names to look for a match
							for (size_t i = 0; i < enum_info.count(); i++) {
								assert(NodeEnumOptionInfo::from(meta_enum, i).has_value());
								const NodeEnumOptionInfo option_info{ NodeEnumOptionInfo::from(meta_enum, i).value() };
								if (input_value == option_info.internal_name()) {
									// Match the regular internal name
									result.set_enum(slot_id, i);
									break;
								}
								else if (option_info.alt_name() && input_value == option_info.alt_name()) {
									// Match the alternate name if it exists
									result.set_enum(slot_id, i);
									break;
								}
							}
						}
						break;
					}
					case SlotType::FLOAT:
					{
						const float float_value{ my_stof(input_value) };
						result.set_float(slot_id, float_value);
						break;
					}
					case SlotType::INT:
					{
						const int int_value{ my_stoi(input_value) };
						result.set_int(slot_id, int_value);
						break;
					}
					case SlotType::VECTOR:
					{
						const csc::Float3 float3_value{ my_stof3(input_value) };
						result.set_vector(slot_id, float3_value);
						break;
					}
					case SlotType::CURVE_RGB:
					{
						const boost::optional<csg::RGBCurveSlotValue> opt_curve_value{ deserialize_rgb_curve(input_value) };
						if (opt_curve_value) {
							result.set_curve_rgb(slot_id, *opt_curve_value);
						}
						break;
					}
					case SlotType::CURVE_VECTOR:
					{
						const boost::optional<csg::VectorCurveSlotValue> opt_curve_value{ deserialize_vector_curve(input_value) };
						if (opt_curve_value) {
							result.set_curve_vec(slot_id, *opt_curve_value);
						}
						break;
					}
					case SlotType::COLOR_RAMP:
					{
						const boost::optional<csg::ColorRamp> opt_ramp_value{ deserialize_ramp(input_value) };
						if (opt_ramp_value) {
							result.set_color_ramp(slot_id, *opt_ramp_value);
						}
						break;
					}
                    case SlotType::IMAGE:
                    {
                        const boost::optional<csg::ImageSlotValue> opt_image_value{ csg::ImageSlotValue(input_value.c_str()) };
                        result.set_image_value(slot_id, *opt_image_value);
                        break;
                    }
					default:
						// Not a type we know how to parse from a string, do nothing
						break;
					}
				}
			}
			else {
				// Here we can handle old parameters that don't exist anymore and translate them if possible
				if (opt_node_type == csg::NodeType::RGB_CURVES) {
					const boost::optional<csg::Curve> new_curve{ deserialize_legacy_curve(input_value) };
					if (new_curve) {
						const boost::optional<size_t> slot_index{ node->slot_index(csg::SlotDirection::INPUT, "curves") };
						if (slot_index) {
							const boost::optional<csg::Slot> slot{ node->slot(*slot_index) };
							if (slot && slot->type() == csg::SlotType::CURVE_RGB) {
								const boost::optional<csg::RGBCurveSlotValue> opt_curve{ slot->value->as<csg::RGBCurveSlotValue>() };
								if (opt_curve) {
									csg::RGBCurveSlotValue rgb_curve{ *opt_curve };
									if (input_name == "rgb_curve") {
										rgb_curve.set_all(*new_curve);
										result.set_curve_rgb(csg::SlotId{ node_id, *slot_index }, rgb_curve);
									}
									else if (input_name == "r_curve") {
										rgb_curve.set_r(*new_curve);
										result.set_curve_rgb(csg::SlotId{ node_id, *slot_index }, rgb_curve);
									}
									else if (input_name == "g_curve") {
										rgb_curve.set_g(*new_curve);
										result.set_curve_rgb(csg::SlotId{ node_id, *slot_index }, rgb_curve);
									}
									else if (input_name == "b_curve") {
										rgb_curve.set_b(*new_curve);
										result.set_curve_rgb(csg::SlotId{ node_id, *slot_index }, rgb_curve);
									}
								}
							}
						}
					}
				}
			}
		}

		// Advance to one past the next NODE_END and continue loop
		while (token_iter != tokenizer.end() && *token_iter != NODE_END) {
			token_iter++;
		}
		if (*token_iter == NODE_END) {
			token_iter++;
		}
	}

	// Advance iterator until we find the start of the connection section
	while (token_iter != tokenizer.end() && *token_iter != SECTION_CONNECTIONS) {
		token_iter++;
	}

	if (token_iter == tokenizer.end()) {
		// No connections, return graph so far
		return result;
	}

	// Advance token to the start of the first connection
	token_iter++;

	while (token_iter != tokenizer.end()) {
		constexpr size_t CONNECTION_TOKENS{ 4 };
		if (iter_has_contents(token_iter, tokenizer.end(), CONNECTION_TOKENS) == false) {
			// Not enough tokens left for a full connection, end early
			break;
		}

		const std::string name_src{ *(token_iter++) };
		const std::string slot_src{ *(token_iter++) };
		const std::string name_dst{ *(token_iter++) };
		const std::string slot_dst{ *(token_iter++) };
		if (ids_by_name.count(name_src) == 0 || ids_by_name.count(name_dst) == 0) {
			// Name does not reference a real node, skip this connection
			continue;
		}
		const NodeId id_src{ ids_by_name[name_src] };
		const NodeId id_dst{ ids_by_name[name_dst] };

		// Find source slot
		const auto node_src{ result.get(id_src) };
		assert(node_src.use_count() > 0);
		boost::optional<size_t> slot_index_src;
		{
			size_t current_index{ 0 };
			for (const Slot& this_slot : node_src->slots()) {
				if (this_slot.dir() == csg::SlotDirection::OUTPUT && slot_src == this_slot.disp_name()) {
					slot_index_src = current_index;
					break;
				}
				current_index++;
			}
		}

		// Find Dest slot
		const auto node_dst{ result.get(id_dst) };
		assert(node_dst.use_count() > 0);
		boost::optional<size_t> slot_index_dst;
		{
			size_t current_index{ 0 };
			for (const Slot& this_slot : node_dst->slots()) {
				if (this_slot.dir() == csg::SlotDirection::INPUT && slot_dst == this_slot.disp_name()) {
					slot_index_dst = current_index;
					break;
				}
				current_index++;
			}
		}

		if (slot_index_src.has_value() == false || slot_index_dst.has_value() == false) {
			continue;
		}

		const SlotId src_slot_id{ id_src, slot_index_src.value() };
		const SlotId dst_slot_id{ id_dst, slot_index_dst.value() };

		result.add_connection(src_slot_id, dst_slot_id);
	}

	return result;
}
