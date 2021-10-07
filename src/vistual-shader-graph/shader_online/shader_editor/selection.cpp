#include "selection.h"

void cse::NodeSelection::select(const SelectMode mode, const csg::NodeId id)
{
	switch (mode) {
		case SelectMode::EXCLUSIVE:
			if (_selected.count(id) == 0) {
				// Ignore exclusive selects when the new node is already selected
				// This allows click+drag to work without resetting the selection
				_selected.clear();
				_selected.insert(id);
			}
			break;
		case SelectMode::ADD:
			_selected.insert(id);
			break;
		case SelectMode::REMOVE:
			_selected.erase(id);
			break;
		case SelectMode::TOGGLE:
			if (_selected.count(id) > 0) {
				_selected.erase(id);
			}
			else {
				_selected.insert(id);
			}
			break;
	}
}
