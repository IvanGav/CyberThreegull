#pragma once

#include <string>
#include <vector>
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
    Normal, // Terminal
    Input, // Text file editing
};

// All terminals
Terminal ts[4];

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
bool interpretCommand(StrA cmd);
int getLineLen();
void createFile(StrA name);

/*
    API
*/

// Open a terminal number 'terminal'
void openTerminal(int terminal) {
    terminalMode = TerminalMode::Normal;
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

/*
    Internal
*/

// Given a command, interpret it. Return false.
bool interpretCommand(StrA cmd) {
    int from = 0;
    int cmdsize;
    seek(cmd, from, cmdsize);
    if (terminalMode == TerminalMode::Normal) {}
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

std::string get_prompt() {
    if (terminalMode == TerminalMode::Normal)
        return "> ";
    else if (terminalMode == TerminalMode::Input)
        return ": ";
    else
        return std::to_string(connectedCamNumber) + ">";
}

void change_file(int file) {
    curFile = file;
    curCursorY = get_cur_file().size() - 1;
    if (file == 0) {
        curCursorX = 2;
    }
    else {
        curCursorX = get_line_len();
    }
    curOffset = -1;
}
//return true if cursor is currently the rightmost of the line
bool rightmost() {
    return get_line_len() == curCursorX;
}

//when in a terminal, set the latest line to be the correct selected (history) command
void setCurTerminalLine() {
    get_cur_file()[curCursorY] = get_prompt() + ts[curTerminal].history[editingHistory];
}

void newCmdLine() {
    wf->push_back(get_prompt());
    wt->history.push_back("");
    //if the last history line is empty, don't make a new empty line, reuse
    if (ts[curTerminal].history[ts[curTerminal].history.size() - 1] != "") ts[curTerminal].history.push_back("");

    curCursorY = get_cur_file().size() - 1;
    curCursorX = 2;
    editingHistory = ts[curTerminal].history.size() - 1;
}

/*
    Interpret typed character
*/

//typing prefixes the selected character

bool interpret_typed_character(Win32::Key charCode, char c) {
    if (charCode == Win32::KEY_UP) {
        up_arrow();
    }
    else if (charCode == Win32::KEY_DOWN) {
        down_arrow();
    }
    else if (charCode == Win32::KEY_LEFT) {
        left_arrow();
    }
    else if (charCode == Win32::KEY_RIGHT) {
        right_arrow();
    }
    else if (charCode == Win32::KEY_BACKSPACE) {
        backspace_key();
    }
    else if (charCode == Win32::KEY_DELETE) {
        delete_key();
    }
    else if (charCode == Win32::KEY_RETURN) {
        return enter_key();
    }
    else if (charCode == Win32::KEY_ESC) {
        if (curFile != 0) {
            change_file(0);
        }
        else {
            return true;
        }
    }
    else if (c != '\0') {
        insert_char(c);
    }
    return false;
}

void up_arrow() {
    if (curFile == 0) {
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