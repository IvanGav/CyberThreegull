#pragma once

#include "../Win32.h"

struct DirEntry;

typedef ArenaArrayList<ArenaArrayList<char>> File;

typedef ArenaArrayList<DirEntry> Dir;

struct DirEntry {
    bool isDir;
    StrA name;
    union {
        Dir* dir;
        File* file;
    };
};

struct Terminal {
    Dir root; // Root directory of this terminal
    File history; // Command history
};

enum TerminalMode {
    Cmd, // Terminal
    Editor, // Text file editing
};

// All terminals
Terminal ts[6];

// Current terminal state  
TerminalMode terminalMode; // Current terminal mode
Terminal* wt; // Working Terminal
Dir* wd; // Working Directory
File* wf; // Working File - the file that's

int curOffset; //will be -1 if none yet
int curCursorX;
int curCursorY;

int savedCursorX;
int editingHistory; // Which item in terminal history is being edited; will be at most curCursorY

/*
    Forward Declarations
*/

// From the outside of the file, use these functions
void terminalsInit();
void openTerminal(int terminal);
File& getTerminal();
int cursorX();
int cursorY();
bool typeChar(Win32::Key char_code, char c);
void clickAt(int x, int y);
void scrollInput(F32 scroll);
int getOffset(int height);

// These functions are internal
int seek(StrA str, int& from, int& len);
StrA vecToStrA(ArenaArrayList<char>& list);
StrA fileToStrA(File& f);
bool interpretCommand(StrA cmd);
int getLineLen();
void createFile(StrA name, bool isDir);
bool closeFile();
bool openFile(StrA file);
bool changeDir(StrA dir);
Dir* getDir(StrA path);
bool rightmost();
void makeRightmost();
void setCurTerminalLine();
void newCmdLine();
bool interpret_typed_character(Win32::Key charCode, char c);
//void tab_key();
void up_arrow();
void down_arrow();
void left_arrow();
void right_arrow();
void insert_char(char c);
void backspace_key();
void delete_key();
bool enter_key();

/*
    API
*/

// Call before using terminals
void terminalsInit() {
    //    MemoryArena& arena = get_scratch_arena();
    //    MEMORY_ARENA_FRAME(arena) {
    //        ArenaArrayList<char> arr {};

    //    }
    //    strafmt(arena, "THere are % number of ducks"a, 5);
    for (int i = 0; i < 6; i++) {
        ts[i].history = {};
        ts[i].root = {};
    }
}

// Open a terminal number 'terminal'
void openTerminal(int terminal) {
    terminalMode = TerminalMode::Cmd;
    wt = &ts[terminal];
    wf = &wt->history;
    curCursorX = 2;
    curCursorY = wf->size - 1;
    curOffset = -1;
    savedCursorX = 0;
    newCmdLine();
}

//get a pointer to a vector of strings to draw
File& getTerminal() {
    return *wf;
}

//return x position of cursor in an editor
int cursorX() {
    return curCursorX;
}

//return y position of cursor in an editor
int cursorY() {
    return curCursorY;
}

// Call for every button user presses
// If returns true, exit from the terminal
bool typeChar(Win32::Key char_code, char c) {
    return interpret_typed_character(char_code, c);
}

// When a terminal is clicked, call this function with coordinates of the character that has been clicked
void clickAt(int x, int y) {
    if (wf != &wt->history) {
        curCursorY = min(U32(y), wf->size - 1);
        curCursorX = min(U32(x), wf->data[curCursorY].size);
    } else if (y == curCursorY && x > 1 && x <= getLineLen()) {
        curCursorX = x;
        curCursorY = y;
    }
}

// Get offset based on current height
int getOffset(int height) {
    if (curOffset == -1) {
        if (wf == &wt->history) {
            curOffset = curCursorY;
        } else {
            curOffset = max(curCursorY - height + 1, 0);
        }
    } else {
        if (curOffset < curCursorY && curCursorY < curOffset + height) {
            //within bounds, do nothing
        } else if (curCursorY >= curOffset + height) {
            curOffset = max(0, curCursorY - height + 1);
        } else {
            curOffset = curCursorY;
        }
    }
    return curOffset;
}

void scrollInput(F32 scroll) {
    //assume -120 to 120
    curOffset = clamp(curOffset - signumf32(scroll), 0, curCursorY);
}

/*
    String manipulation
*/

// Set 'len' to how many characters were "read" until next space; return original 'from' value
int seek(StrA str, int& from, int& len) {
    int from_old = from;
    len = 0;
    while (from < str.length && str[from] != ' ') {
        from++;
        len++;
    }
    while (from < str.length && str[from] == ' ') from++;
    return from_old;
}

// Return a StrA slice of `list`
StrA vecToStrA(ArenaArrayList<char>& list) {
    ArenaArrayList<char> b = list;
    return { b.data, b.size };
}

// Return a StrA slice of `list`
StrA fileToStrA(File& f) {
    ArenaArrayList<char> buf{};
    for (int i = 0; i < f.size; i++) {
        buf.push_back_n(f.data[i].data, f.data[i].size);
    }
    return vecToStrA(buf);
}

/*
    Internal
*/

// Given a command, interpret it. Return false.
bool interpretCommand(StrA cmd) {
    int from = 0;
    int cmdsize;
    seek(cmd, from, cmdsize);
    if (terminalMode == TerminalMode::Cmd) {}
    newCmdLine();
    return false;
}

int getLineLen() {
    return wf->data[curCursorY].size;
}

void createFile(StrA name, bool isDir) {
    if (isDir) {
        Dir* d = globalArena.zalloc<Dir>(1);
        DirEntry e = DirEntry { true, name };
        e.dir = d;
        wd->push_back(e);
    } else {
        File* f = globalArena.zalloc<File>(1);
        f->push_back(ArenaArrayList<char>{});
        DirEntry e = DirEntry { false, name };
        e.file = f;
        wd->push_back(e);
    }
}

// Return true if successful
bool closeFile() {
    if (wf == &wt->history) {
        return false;
    }
    wf = &wt->history;
    curCursorX = 2;
    curCursorY = wf->size - 1;
    curOffset = -1;
    terminalMode = TerminalMode::Cmd;
    return true;
}

// Open a file with a given name. Return true if the file was found (and opened).
bool openFile(StrA file) {
    for (DirEntry& e : *wd) {
        if (e.name == file && !e.isDir) {
            wf = e.file;
            curCursorX = getLineLen();
            curOffset = -1;
            terminalMode = TerminalMode::Editor;
            return true;
        }
    }
    return false;
}

// Change dir. Return true if the dir was found (and changed to).
bool changeDir(StrA dir) {
    for (DirEntry& e : *wd) {
        if (e.name == dir && e.isDir) {
            wd = e.dir;
            return true;
        }
    }
    return false;
}

// Get a directory that's indicated by the path. Return nullptr if it doesn't exist.
Dir* getDir(StrA path) {
    Dir* cur_dir = wd;
    MemoryArena& arena = get_scratch_arena();
    MEMORY_ARENA_FRAME(arena) {
        U64 num;
        StrA* path_split = path.split(arena, &num, "/"a);

        // for every dir in path specified
        for (int i = 0; i < num; i++) {
            // for every file in the dir
            for (DirEntry& e : *cur_dir) {
                if (e.name == path_split[i] && e.isDir) {
                    cur_dir = e.dir;
                    goto next;
                }
            }
            cur_dir = nullptr; // the loop ended without switching to a new dir - not found
            goto ret;
            next:;
        }
        ret:;
    }
    return cur_dir;
}

//return true if cursor is currently the rightmost of the line
bool rightmost() {
    return getLineLen() == curCursorX;
}

// make cursor rightmost in current line
void makeRightmost() {
    curCursorX = getLineLen();
}

//when in a terminal, set the latest line to be the correct selected (`editingHistory`) command
void setCurTerminalLine() {
    wt->history.back().clear();
    ArenaArrayList data = wt->history.data[editingHistory];
    wt->history.back().push_back_n(data.data, data.size);
}

// assuming that `wt` is history, put a blank cmd line
void newCmdLine() {
    //if the last history line is empty, don't make a new empty line
    if (wt->history.empty() || wt->history.back().size > 2) {
        wt->history.push_back(ArenaArrayList<char>{});
    }
    wt->history.back().push_back(' ', '>');
    curCursorX = 2;
    curCursorY = wt->history.size - 1;
    editingHistory = wt->history.size - 1;
    setCurTerminalLine();
}

/*
    Interpret typed character
*/

//typing prefixes the selected character

bool interpret_typed_character(Win32::Key charCode, char c) {
    if (charCode == Win32::KEY_UP) {
        up_arrow();
    } else if (charCode == Win32::KEY_DOWN) {
        down_arrow();
    } else if (charCode == Win32::KEY_LEFT) {
        left_arrow();
    } else if (charCode == Win32::KEY_RIGHT) {
        right_arrow();
    } else if (charCode == Win32::KEY_BACKSPACE) {
        backspace_key();
    } else if (charCode == Win32::KEY_DELETE) {
        delete_key();
    } else if (charCode == Win32::KEY_TAB) {
        //tab_key();
    } else if (charCode == Win32::KEY_RETURN) {
        return enter_key();
    } else if (charCode == Win32::KEY_ESC) {
        if (terminalMode == TerminalMode::Cmd) {
            return true;
        } else {
            closeFile();
        }
    } else if (c != '\0') {
        insert_char(c);
    }
    return false;
}

/*void tab_key() {
	if (terminalMode == TerminalMode::Editor) {
		return;
	}

	Dir* currDir = wd; // current Directory

    // Getting the users typed line
    StrA fullLine = vecToStrA(wf->back());
	StrA prompt = fullLine.slice(0, 2); // get the prompt
    StrA currentLine = (fullLine++)++; // removes the prompt
    ArenaArrayList<char>* updatedLine = wf->back();

	if (currentLine.is_empty()) {
		// line empty
		return;
	}
    
	// Finding the section of the line that needs to be autocompleted(tabbed)
    int indexLastSpace = currentLine.skip(currentLine.find(" ")); // get index of last space, -1 if none
    StrA inputToTab = (indexLastSpace == -1) ? currentLine : currentLine.slice(indexLastSpace + 1); // actual input to tab
	int inputToTabSize = inputToTab.size; // size of the input to tab
    StrA tabbedInput; 
	
    ArenaArrayList<StrA> Files{}; // List of matching files
    for (ArenaArrayList<char>* currFileRaw : File) {
        StrA currFile = vecToStrA(*currFileRaw); // current file name
        // Check if the current file name starts with the input
        if (currFile.starts_with(inputToTab)) {
            // Found a file that matches the input
            Files.push_back(currFile); 
        }
    }

    // Check if we have matches
	if (Files.size > 0) {
		// We have matchee
		if (Files.size == 1) {
			// if there is only one match just update the  updatedLine
			tabbedInput = Files[0].slice(inputToTabSize); // get the rest of the file name
		}
		else {
			// More than one match
            // IMPLEMENT TODO 
            tabbedInput = Files[0].slice(inputToTabSize); // get the rest of the file name
		}
	}
    else {
        // no matches, dont change anything
        tabbedInput = ""a;
    }

	updatedLine->push_back(tabbedInput); // add the tabbed input to the line
	// Update the cursor position
    curCursorX += tabbedInput.size;
    makeRightmost();

}
*/

void up_arrow() {
    if (terminalMode == TerminalMode::Cmd) {
        //in terminal
        if (editingHistory == 0) return;
        editingHistory--;
        setCurTerminalLine();
        makeRightmost();
    } else {
        //in editor
        if (curCursorY == 0) {
            savedCursorX = max(curCursorX, savedCursorX);
            curCursorX = 0;
            return;
        }
        curCursorY--;
        curCursorX = max(curCursorX, savedCursorX);
        if (getLineLen() < curCursorX) {
            //bad, save
            savedCursorX = max(curCursorX, savedCursorX);
            makeRightmost();
        }
    }
}

void down_arrow() {
    if (terminalMode == TerminalMode::Cmd) {
        //in terminal
        if (editingHistory == wt->history.size - 1) return;
        editingHistory++;
        setCurTerminalLine();
        makeRightmost();
    } else {
        //in editor
        if (curCursorY == wf->size - 1) { makeRightmost(); return; }
        curCursorY++;
        curCursorX = max(curCursorX, savedCursorX);
        if (getLineLen() < curCursorX) {
            //bad, save
            savedCursorX = max(curCursorX, savedCursorX);
            makeRightmost();
        }
    }
}

//
void left_arrow() {
    if (curCursorX > (terminalMode == TerminalMode::Cmd ? 2 : 0)) { //because you don't want to be able to edit " >" in the terminal
        curCursorX--;
    }
    savedCursorX = 0;
}

//go right until to the right of the rightmost character; reset savedCursorX
void right_arrow() {
    if (curCursorX < getLineLen()) {
        curCursorX++;
    }
    savedCursorX = 0;
}

void insert_char(char c) {
    if (rightmost()) {
        wf->data[curCursorY].push_back(c);
    }
    else {
        wf->data[curCursorY].insert(c, curCursorX);
    }
    right_arrow();
}

void backspace_key() {
    if (terminalMode == TerminalMode::Cmd) {
        //terminal
        if (curCursorX > 2) {
            wf->data[curCursorY].erase(curCursorX - 1);
            left_arrow();
        }
    } else {
        //editor
        if (curCursorX != 0) {
            wf->data[curCursorY].erase(curCursorX - 1);
            left_arrow();
        } else {
            if (curCursorY == 0) return;
            curCursorY--;
            makeRightmost();
            ArenaArrayList<char>& r = wf->data[curCursorY + 1];
            wf->data[curCursorY].push_back_n(r.data, r.size);
            wf->erase(curCursorY + 1);
        }
    }
}

void delete_key() {
    if (terminalMode == TerminalMode::Cmd) {
        //terminal
        if (rightmost()) return;
        wf->data[curCursorY].erase(curCursorX);
    }
    else {
        //editor
        if (!rightmost()) {
            wf->data[curCursorY].erase(curCursorX);
        } else {
            if (curCursorY == wf->size - 1) return;
            ArenaArrayList<char>& r = wf->data[curCursorY + 1];
            wf->data[curCursorY].push_back_n(r.data, r.size);
            wf->erase(curCursorY + 1);
        }
    }
}

bool enter_key() {
    if (terminalMode == TerminalMode::Cmd) {
        bool ret = interpretCommand(StrA{ wf->back().data, wf->back().size });
        newCmdLine();
        return ret;
    } else {
        if (rightmost()) {
            //newline
            wf->insert(ArenaArrayList<char>{}, curCursorY + 1);
        } else {
            //split
            ArenaArrayList<char>& curLine = wf->data[curCursorY];
            wf->insert(curLine.subarray(curCursorX, curLine.size-curCursorX), curCursorY + 1);
            curLine = curLine.subarray(0, curCursorX);
        }
        down_arrow();
        curCursorX = 0;
        savedCursorX = 0;
        return false;
    }
}
