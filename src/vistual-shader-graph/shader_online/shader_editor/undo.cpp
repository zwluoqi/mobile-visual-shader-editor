#include "undo.h"

#include <cstddef>

static constexpr size_t UNDO_LIMIT = 50;

cse::UndoStack::UndoStack(const csg::Graph& graph) :
	next_undo_graph{ graph }
{

}

void cse::UndoStack::clear(const csg::Graph& graph)
{
	undo_state.clear();
	redo_state.clear();
	next_undo_graph = graph;
}

bool cse::UndoStack::push_undo(const csg::Graph& graph)
{
	if (next_undo_graph != graph) {
		redo_state.clear();
		undo_state.push_front(next_undo_graph);
		next_undo_graph = graph;
		while (undo_state.size() > UNDO_LIMIT) {
			undo_state.pop_back();
		}
		return true;
	}
	else {
		return false;
	}
}

csg::Graph cse::UndoStack::pop_undo(const csg::Graph& for_redo)
{
	if (undo_state.size() == 0) {
		return for_redo;
	}
	redo_state.push_front(for_redo);
	csg::Graph result = undo_state.front();
	undo_state.pop_front();
	next_undo_graph = result;
	return result;
}

csg::Graph cse::UndoStack::pop_redo(const csg::Graph& for_undo)
{
	if (redo_state.size() == 0) {
		return for_undo;
	}
	undo_state.push_front(for_undo);
	csg::Graph result = redo_state.front();
	redo_state.pop_front();
	next_undo_graph = result;
	return result;
}
