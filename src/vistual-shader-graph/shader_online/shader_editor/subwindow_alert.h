#pragma once

#include <string>

#include "event.h"

namespace cse {
	class AlertSubwindow {
	public:
		void set_message(const std::string& message);

		InterfaceEventArray run() const;

	private:
		std::string message{ "[No message]" };
	};
}
