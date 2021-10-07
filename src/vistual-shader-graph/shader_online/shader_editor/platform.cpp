#include "platform.h"
#include "../common/entry/input.h"

bool cse::Platform::save_graph_dialog()
{
    char tmp[bx::kMaxFilePath];
    entry::postDialogSaveEventToMainThread(tmp);
	return true;
}

void cse::Platform::load_graph_dialog()
{
	char tmp[bx::kMaxFilePath];
    tmp[0]='n';
    entry::postDialogOpenEventToMainThread(tmp);
}

void cse::Platform::open_image_dialog(){
    char tmp[bx::kMaxFilePath];
    tmp[0]='i';
    entry::postDialogOpenEventToMainThread(tmp);
}
