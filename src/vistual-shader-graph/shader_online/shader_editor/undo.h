#pragma once

#include <list>

#include "../shader_graph/graph.h"

namespace cse {
	class UndoStack {
	public:
		UndoStack(const csg::Graph& graph);

		void clear(const csg::Graph& graph);

		bool push_undo(const csg::Graph& graph);

		csg::Graph pop_undo(const csg::Graph& for_redo);
		csg::Graph pop_redo(const csg::Graph& for_undo);

		bool undo_available() const { return undo_state.size() > 0; }
		bool redo_available() const { return redo_state.size() > 0; }

	private:
		csg::Graph next_undo_graph;

		std::list<csg::Graph> undo_state;
		std::list<csg::Graph> redo_state;
	};
}
