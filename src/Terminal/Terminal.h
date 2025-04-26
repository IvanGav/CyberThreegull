#pragma once

#include "../Win32.h"

#define PROMPT "> "a
#define PROMPT_LEN 2

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
int getOffset(int height);
bool typeChar(Win32::Key char_code, char c);
void scrollInput(F32 scroll);

// These functions are internal
int seek(StrA str, int& from, int& len);
StrA vecToStrA(ArenaArrayList<char> list);
bool interpretCommand(StrA cmd);
int getLineLen();
void createFile(StrA name, bool isDir);
bool rightmost();
void setCurTerminalLine();

/*
    API
*/

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

// Call before using terminals
void terminalsInit() {
//    MemoryArena& arena = get_scratch_arena();
//    MEMORY_ARENA_FRAME(arena) {
//        ArenaArrayList<char> arr {};

//    }
//    strafmt(arena, "THere are % number of ducks"a, 5);
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
StrA vecToStrA(ArenaArrayList<char> list) {
    ArenaArrayList<char> b = list;
    return { b.data, b.size };
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
        f->push_back("");
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

//when in a terminal, set the latest line to be the correct selected (history) command
void setCurTerminalLine() {
    wt->history.back().clear();
    ArenaArrayList data = wt->history.data[editingHistory];
    wt->history.back().push_back_n(data.data, data.size);
}

// assuming that `wt` is history, put a blank cmd line
void newCmdLine() {
    //if the last history line is empty, don't make a new empty line
    if (wt->history.back().size > 2) {
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
        tab_key();
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

ArenaArrayList<StrA> find_files(StrA match) {
	ArenaArrayList<StrA> files{}; // List of matching files
	for (int i = 0; i < wd->size; i++) {
		StrA currFile = wd->data[i].name; // current file name
		// Check if the current file name starts with the input
		if (currFile.starts_with(match)) {
			// Found a file that matches the input
			files.push_back(currFile);
		}
	}
	return files;
}

StrA findInputToTab() {
	// Find the last space in the line
	StrA fullLine = vecToStrA(wf->back()); // get the full line
    StrA currentLine = (fullLine++)++; // removes the prompt
	int indexLastSpace = currentLine.find(" "a); // get index of last space, -1 if none
	StrA inputToTab = (indexLastSpace == -1) ? currentLine : currentLine.skip(indexLastSpace + 1); // actual input to tab
	return inputToTab;
}


void tab_key() {
	if (terminalMode == TerminalMode::Editor) {
		return;
	}
	if (wf->back().size > PROMPT_LEN) {
		// line empty
		return;
	}
    Dir* currDir = wd; // current Directory
    ArenaArrayList<char>* updatedLine = &wf->back();

	// Finding the section of the line that needs to be autocompleted(tabbed)
    StrA inputToTab = findInputToTab();
	int inputToTabSize = inputToTab.length; // size of the input to tab
    StrA tabbedInput = ""a;
	
    // Find matching files to the tabbed input
	ArenaArrayList<StrA> files = find_files(inputToTab);
    // Check if we have matches
    if (files.size == 0)
        return;

	// We have match
	if (files.size == 1) {
		// if there is only one match just update the  updatedLine
		tabbedInput = files[0].skip(inputToTabSize); // get the rest of the file name
        updatedLine->push_back(tabbedInput); // add the tabbed input to the line

        // Update the cursor position
        curCursorX += tabbedInput.length;
        makeRightmost();
        return;
	}
			// More than one match
            // IMPLEMENT TODO 
    while (1) {
        struct Prefix {
            StrA prefix;
            int count;
        };

        int lineAdjust = 0;

        // Find the longest common prefix
        Prefix mostCommonPrefix = { ""a, -1 };
        Prefix currCommonPrefix = { ""a, -1 };
        for (int i = 0; i < files.size; i++) {
            StrA currPrefix = files[i].slice(inputToTabSize + 1 + lineAdjust, inputToTabSize + 1 + lineAdjust);
            currCommonPrefix = { currPrefix, 1 };

            // find the first character of a file name with greater matches to other files
            for (int j = i + 1; j < files.size; j++) {
                StrA compPrefix = files[j].slice(inputToTabSize + 1 + lineAdjust, inputToTabSize + 1 + lineAdjust);
                if (currPrefix.starts_with(compPrefix)) {
                    currCommonPrefix.count++;
                }
            }
            if (mostCommonPrefix.count == -1 || currCommonPrefix.count > mostCommonPrefix.count) {
                mostCommonPrefix = currCommonPrefix;
            }

        }

		// Check if we have a common prefix
		if (mostCommonPrefix.count == 1) {
			// No common prefix, just print the files
			break;
		}

        updatedLine->push_back(mostCommonPrefix.prefix); // add the common prefix to the line
        curCursorX++;
        lineAdjust++;
        makeRightmost();

    }
     

}

void up_arrow() {
    if (terminalMode == TerminalMode::Cmd) {
        //in terminal
        if (editingHistory == 0) return;
        editingHistory--;
        setCurTerminalLine();
        curCursorX = get_line_len();
    }
    else {
        //in editor
        if (curCursorY == 0) {
            savedCursorX = max(curCursorX, savedCursorX);
            curCursorX = 0;
            return;
        }
        curCursorY--;
        curCursorX = max(curCursorX, savedCursorX);
        if (get_line_len() < curCursorX) {
            //bad, save
            savedCursorX = max(curCursorX, savedCursorX);
            curCursorX = get_line_len();
        }
    }
}

void down_arrow() {
    if (curFile == 0) {
        //in terminal
        if (editingHistory == ts[curTerminal].history.size() - 1) return;
        editingHistory++;
        setCurTerminalLine();
        curCursorX = get_line_len();
    }
    else {
        //in editor
        if (curCursorY == get_cur_file().size() - 1) { curCursorX = get_line_len(); return; }
        curCursorY++;
        curCursorX = max(curCursorX, savedCursorX);
        if (get_line_len() < curCursorX) {
            //bad, save
            savedCursorX = max(curCursorX, savedCursorX);
            curCursorX = get_line_len();
        }
    }
}

//
void left_arrow() {
    if (curCursorX > (curFile == 0 ? 2 : 0)) { //because you don't want to be able to edit "> " in the terminal
        curCursorX--;
    }
    savedCursorX = 0;
}

//go right until to the right of the rightmost character; reset savedCursorX
void right_arrow() {
    if (curCursorX < get_line_len()) {
        curCursorX++;
    }
    savedCursorX = 0;
}

void insert_char(char c) {
    if (curFile == 0) {
        //terminal
        if (rightmost()) {
            ts[curTerminal].history[editingHistory].push_back(c);
        }
        else {
            ts[curTerminal].history[editingHistory].insert(curCursorX - 2, 1, c);
        }
        setCurTerminalLine();
    }
    else {
        //editor
        if (rightmost()) {
            get_cur_file()[curCursorY].push_back(c);
        }
        else {
            get_cur_file()[curCursorY].insert(curCursorX, 1, c);
        }
    }
    right_arrow();
}

void backspace_key() {
	if (curFile == 0) {
        //terminal
        file& f = get_cur_file();
        if (curCursorX > 2) {
            ts[curTerminal].history[editingHistory].erase(curCursorX - 3, 1);
            left_arrow();
        }
        setCurTerminalLine();
    }
    else {
        //editor
        file& f = get_cur_file();
        if (curCursorX != 0) {
            f[curCursorY].erase(curCursorX - 1, 1);
            left_arrow();
        }
        else {
            if (curCursorY == 0) return;
            curCursorY--;
            curCursorX = get_line_len();
            f[curCursorY].append(f[curCursorY + 1]);
            f.erase(f.begin() + curCursorY + 1);
        }
    }
}

void delete_key() {
    if (curFile == 0) {
        //terminal
        file& f = get_cur_file();
        if (rightmost()) return;
        ts[curTerminal].history[editingHistory].erase(curCursorX - 2, 1);
        setCurTerminalLine();
    }
    else {
        //editor
        file& f = get_cur_file();
        if (!rightmost()) {
            f[curCursorY].erase(curCursorX, 1);
        }
        else {
            if (curCursorY == get_cur_file().size() - 1) return;
            f[curCursorY].append(f[curCursorY + 1]);
            f.erase(f.begin() + curCursorY + 1);
        }
    }
}

bool enter_key() {
    if (curFile == 0) {
        //copy current history item to latest history line
        int last = ts[curTerminal].history.size() - 1;
        ts[curTerminal].history[last] = ts[curTerminal].history[editingHistory];
        editingHistory = last;
        return interpret_command(ts[curTerminal].history[last]);
    }
    else {
        file& f = get_cur_file();
        if (rightmost()) {
            //newline
            f.insert(f.begin() + curCursorY + 1, "");
        }
        else {
            //split
            f.insert(f.begin() + curCursorY + 1, (*(f.begin() + curCursorY)).substr(curCursorX));
            f[curCursorY].erase(curCursorX);
        }
        down_arrow();
        curCursorX = 0;
        savedCursorX = 0;
        return false;
    }
}