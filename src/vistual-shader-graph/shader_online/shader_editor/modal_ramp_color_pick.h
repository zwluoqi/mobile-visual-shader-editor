#pragma once

#include <array>
#include <cstddef>

#include <boost/optional.hpp>

#include "event.h"

namespace cse {
	class ModalRampColorPicker {
	public:

		void attach(csg::SlotId slot_id, size_t index, csc::Float4 rgba);

		InterfaceEventArray run() const;

		void do_event(const InterfaceEvent& event);

	private:
		struct ColorLocation {
			ColorLocation(csg::SlotId slot_id, size_t index) : slot_id{ slot_id }, index{ index } {}

			csg::SlotId slot_id;
			size_t index;
		};
		std::array<float, 4> rgba;
		boost::optional<ColorLocation> attached_location;
	};
}
