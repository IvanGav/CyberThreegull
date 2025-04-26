#pragma once

#include "../Win32.h"
#include "asm.h"
#include "Files.h"

#define PROMPT " >"a
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
    File io; // IO history
};

enum TerminalMode {
    Cmd, // Terminal
    Editor, // Text file editing
};

struct TabInfo {
    StrA portion;
    int index;
};

// All terminals
Terminal ts[6];

// Current terminal state  
TerminalMode terminalMode; // Current terminal mode
Terminal* wt; // Working Terminal
Dir* wd; // Working Directory
File* wf; // Working File - the file that's

ArenaArrayList<StrA> pwd; // keep the current dir tree from root

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
void io_print(File& f, StrA s);
ArenaArrayList<char> strAToVec(StrA s);
bool interpretCommand(StrA cmd);
void print_help();
void vim();
void mkdir();
void checkConditions();
void cat();
void exec(StrA source, StrA out, StrA error);
void touch();
int getLineLen();
void create_file(StrA name, bool isDir);
bool close_file();
bool open_file(StrA file);
bool change_dir(StrA dir);
Dir* get_dir(StrA path);
bool rightmost();
void make_rightmost();
void set_cur_terminal_line();
void new_cmd_line();
bool interpret_typed_character(Win32::Key charCode, char c);
TabInfo find_input_to_tab();
ArenaArrayList<StrA> find_files(StrA match);
StrA add_two_stra(StrA str1, StrA str2);
void tab_key();
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
    init_asm();
    for (int i = 0; i < 6; i++) {
        ts[i].history = {};
        ts[i].root = {};
    }

    //put files in all terminal root directors,
    // ts[0] = depression
    // ts[1] = incel
    // ts[2] = adhd
    // ts[3] = brainrot
    // ts[4] = insecurity
    // ts[5] = anxiety

    File* depressionFile = globalArena.zalloc<File>(1);
	depression(*depressionFile);
	ts[0].root.push_back(DirEntry{ false, "depression"a });
    ts[0].root.back().file = depressionFile;

	File* depressionFactsFile = globalArena.zalloc<File>(1);
	depresssion_facts(*depressionFactsFile);
	ts[0].root.push_back(DirEntry{ false, "depression facts"a });
    ts[0].root.back().file = depressionFactsFile;

	File* incelFile = globalArena.zalloc<File>(1);
	incel_facts(*incelFile);
	ts[1].root.push_back(DirEntry{ false, "incel"a });
	ts[1].root.back().file = incelFile;

	File* incelFactsFile = globalArena.zalloc<File>(1);
	incel_facts(*incelFactsFile);
	ts[1].root.push_back(DirEntry{ false, "incel facts"a });
	ts[1].root.back().file = incelFactsFile;

	File* adhdFile = globalArena.zalloc<File>(1);
	adhd(*adhdFile);
	ts[2].root.push_back(DirEntry{ false, "adhd"a });
	ts[2].root.back().file = adhdFile;

	File* adhdFactsFile = globalArena.zalloc<File>(1);
	adhd_facts(*adhdFactsFile);
	ts[2].root.push_back(DirEntry{ false, "adhd facts"a });
	ts[2].root.back().file = adhdFactsFile;

	File* insecurityFile = globalArena.zalloc<File>(1);
	insecurity(*insecurityFile);
	ts[4].root.push_back(DirEntry{ false, "insecurity"a });
	ts[4].root.back().file = insecurityFile;

	File* insecurityFactsFile = globalArena.zalloc<File>(1);
	insecurity_facts(*insecurityFactsFile);
	ts[4].root.push_back(DirEntry{ false, "insecurity facts"a });
	ts[4].root.back().file = insecurityFactsFile;

	File* anxietyFile = globalArena.zalloc<File>(1);
    anxiety(*anxietyFile);
	ts[5].root.push_back(DirEntry{ false, "anxiety"a });
	ts[5].root.back().file = anxietyFile;

	File* anxietyFactsFile = globalArena.zalloc<File>(1);
	anxiety_facts(*anxietyFactsFile);
	ts[5].root.push_back(DirEntry{ false, "anxiety facts"a });
    ts[5].root.back().file = anxietyFactsFile;



}
    

// Open a terminal number 'terminal'
void openTerminal(int terminal) {
    terminalMode = TerminalMode::Cmd;
    wt = &ts[terminal];
    wd = &wt->root;
    wf = &wt->io;
    new_cmd_line();
    //curCursorX = 2;
    //curCursorY = wf->size - 1;
    curOffset = -1;
    savedCursorX = 0;
}

void checkConditions() {
    int terminalIndex = wt - ts; // Calculate the index of the current terminal
    switch (terminalIndex) {
        case 0:
            for (DirEntry& e : *wd) {
				if (!e.isDir) {
					if (e.name == "depression"a) {
						// Check if happiness what changed
                        // Loop trough file to make sure "-5" isnt in the file
						for (int i = 0; i < e.file->size; i++) {
							if (e.file->data[i].data[0] == '-' && e.file->data[i].data[1] == '5') {
								// Found -5, do nothing
								return;
							}

						}
						// Found value isnt -5, so we congratulate the user
						File& io = wt->io;
						io_print(io, "Congratulations! You have cured the depression!"a);
						io_print(io, "ChatGPT is now happy!"a);							
					}
				}
            }
        break;
        // Add cases for other terminals if needed
    default:
        break;
    }
}


//get a pointer to a vector of strings to draw
File& getTerminal() {
    if (terminalMode == TerminalMode::Cmd) {
        return wt->io;
    }
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
    if (terminalMode == TerminalMode::Editor) {
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
        if (terminalMode == TerminalMode::Cmd) {
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
        buf.push_back('\n');
    }
    return vecToStrA(buf);
}

// do not modify the returned arrays FOR THE LOVE OF GOD
ArenaArrayList<char> strAToVec(StrA s) {
    ArenaArrayList<char> b{};
    b.data = const_cast<char*>(s.str);
    b.size = s.length;
    return b;
}

void io_print(File& f, StrA s) {
    f.push_back(strAToVec(s));
}

void io_print(File& f, StrA s, StrA s2) {
    f.push_back(strAToVec(s));
    f.back().push_back_n(s2.str, s2.length);
}

/*
    Internal
*/

// Given a command, interpret it. Return false.
bool interpretCommand(StrA cmd) {
    int from = 0;
    int cmdsize;
    while (cmd.front() == ' ') cmd++;
    seek(cmd, from, cmdsize);

    File& io = wt->io;
    
    if (cmd.substr(0, cmdsize) == "dir"a || cmd.substr(0, cmdsize) == "l"a) {
        if (wd->size == 0) {
            io_print(io, "No files in this directory."a);
        } else {
            for (DirEntry& e : *wd) {
                if (e.isDir) {
                    io_print(io, e.name, "/"a);
                }
                else {
                    io_print(io, e.name);
                }
            }
        }
    } else if (cmd.substr(0, cmdsize) == "help"a) {
        print_help();
    } else if (cmd.substr(0, cmdsize) == "vim"a) {
        int filename_from = seek(cmd, from, cmdsize);
        StrA file = cmd.substr(filename_from, cmdsize);

        if (!open_file(file)) {
            io_print(io, "File not found."a);
        }
    } else if (cmd.substr(0, cmdsize) == "mkdir"a) {
        int dirname_from = seek(cmd, from, cmdsize);
        StrA dir = cmd.substr(dirname_from, cmdsize);

        if (dir.is_empty()) {
            io_print(io, "No directory name specified."a);
            goto skip_mkdir;
        }
        for (DirEntry& e : *wd) {
            if (e.name == dir && e.isDir) {
                io_print(io, "Directory already exists."a);
                goto skip_mkdir;
            }
        }
        create_file(dir, true);
        skip_mkdir:;
    } else if (cmd.substr(0, cmdsize) == "cat"a) {
        int filename_from = seek(cmd, from, cmdsize);
        StrA file = cmd.substr(filename_from, cmdsize);

        if (file.is_empty()) {
            io_print(io, "No file name specified."a);
        } else {
            for (DirEntry& e : *wd) {
                if (e.name == file && !e.isDir) {
                    for (int i = 0; i < e.file->size; i++) {
                        io_print(io, vecToStrA(e.file->data[i]));
                    }
                    goto skip_cat;
                }
            }
            io_print(io, "File not found."a);
            skip_cat:;
        }
    } else if (cmd.substr(0, cmdsize) == "exit"a) {
        return true;
    } else if (cmd.substr(0, cmdsize) == "pwd"a) {
        
    } else if (cmd.substr(0, cmdsize) == "cd"a) {
        int dirname_from = seek(cmd, from, cmdsize);
        StrA dir = cmd.substr(dirname_from, cmdsize);

        change_dir(dir);
    } else if (cmd.substr(0, cmdsize) == "exec"a) {
        int source_from = seek(cmd, from, cmdsize);
        StrA source = cmd.substr(source_from, cmdsize);
        int out_from = seek(cmd, from, cmdsize);
        StrA out = cmd.substr(out_from, cmdsize);
        int error_from = seek(cmd, from, cmdsize);
        StrA error = cmd.substr(error_from, cmdsize);

        exec(source, out, error);
    } else if (cmd.substr(0, cmdsize) == "touch"a) {
        int filename_from = seek(cmd, from, cmdsize);
        StrA file = cmd.substr(filename_from, cmdsize);

        if (file.is_empty()) {
            io_print(io, "No file name specified."a);
            goto skip_touch;
        }
        for (DirEntry& e : *wd) {
            if (e.name == file && !e.isDir) {
                io_print(io, "File already exists."a);
                goto skip_touch;
            }
        }
        create_file(file, false);
        skip_touch:;
    } else if (cmd.substr(0, cmdsize) == "clear"a) {
        File& io = wt->io;
        io.clear();
    } else {
        io_print(io, "No such command exists."a);
    }

    new_cmd_line();
    return false;
}

void print_help() {
    File& io = wt->io;
    io_print(io, " >help"a);
    io_print(io, "  Display this message"a);

    io_print(io, " >clear"a);
    io_print(io, "  Clear the terminal"a);

    io_print(io, " >dir/l [dir]"a);
    io_print(io, "  List all files in a directory"a);

    io_print(io, " >cat <file>"a);
    io_print(io, "  Print file's contents"a);

    io_print(io, " >vim <file>"a);
    io_print(io, "  Open a file in an editor"a);

    io_print(io, " >touch <file>"a);
    io_print(io, "  Create a file"a);

    io_print(io, " >mkdir <dir>"a);
    io_print(io, "  Create a directory"a);

    io_print(io, " >pwd"a);
    io_print(io, "  Print the working directory"a);

    io_print(io, " >exec <file>"a);
    io_print(io, "  Execute a file"a);

    io_print(io, " >exit"a);
    io_print(io, "  Exit"a);
}

void vim() {
    
}

void mkdir() {
}


// Print out file 
void cat() {
}

File* getFile(StrA name) {
    for (DirEntry& e : *wd) {
        if (e.name == name && !e.isDir) {
            return e.file;
        }
    }
    return nullptr;
}

void exec(StrA source_filename, StrA out_filename, StrA error_filename) {
    //TODO: check for bad filenames
    StrA prog{};
    File& source = *getFile(source_filename);
    File& out = *getFile(out_filename);
    File& error = *getFile(error_filename);

    //for (int i = 0; i < source.size; i++) {
    //    strafmt()
    //}

    //run_program(StrA prog, [](U32 n) { io_print(out, strafmt(globalArena, "%", n)); }, [](StrA str) { io_print(str); });
}

int getLineLen() {
    return wf->data[curCursorY].size;
}

void create_file(StrA name, bool isDir) {
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
bool close_file() {

    if (terminalMode == TerminalMode::Cmd) {
        return false;
    }
    wf = &wt->io;
    curCursorX = 2;
    curCursorY = wf->size - 1;
    curOffset = -1;
    terminalMode = TerminalMode::Cmd;

    checkConditions(); // JANG

    return true;
}

// Open a file with a given name. Return true if the file was found (and opened).
bool open_file(StrA file) {
    for (DirEntry& e : *wd) {
        if (e.name == file && !e.isDir) {
            if (e.name == file && !e.isDir) {
                wf = e.file;
                curCursorX = getLineLen();
                curCursorY = 0;
                curOffset = -1;
                terminalMode = TerminalMode::Editor;
                return true;
            }
        }
    }
    return false;
}

// Change dir. Return true if the dir was found (and changed to).
bool change_dir(StrA dir) {
    for (DirEntry& e : *wd) {
        if (e.name == dir && e.isDir) {
            wd = e.dir;
            return true;
        }
    }
    return false;
}

// Get a directory that's indicated by the path. Return nullptr if it doesn't exist.
Dir* get_dir(StrA path) {
    Dir* cur_dir = wd;
    MemoryArena& arena = get_scratch_arena();
    MEMORY_ARENA_FRAME(arena) {
        ArenaArrayList<StrA> pwd_local{}; pwd_local.copy_from(pwd);
        U64 num;
        StrA* path_split = path.split(arena, &num, "/"a);

        // for every dir in path specified
        for (int i = 0; i < num; i++) {
            // for every file in the dir
            if (path_split[i] == ".."a) {
                if (pwd_local.size == 0) {
                    return nullptr;
                }
                pwd_local.pop_back();
            }
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
void make_rightmost() {
    curCursorX = getLineLen();
}

//when in a terminal, set the latest line to be the correct selected (`editingHistory`) command
void set_cur_terminal_line() {
    wt->io.back().clear();
    ArenaArrayList data = wt->history.data[editingHistory];
    wt->io.back().push_back_n(PROMPT.str, PROMPT.length);
    wt->io.back().push_back_n(data.data, data.size);

    curCursorY = wt->io.size - 1;
    make_rightmost();
}

// assuming that `wt` is history, put a blank cmd line
void new_cmd_line() {
    if (terminalMode != TerminalMode::Cmd) return;
    wt->history.push_back(ArenaArrayList<char>{});
    wt->io.push_back(ArenaArrayList<char>{});
    editingHistory = wt->history.size - 1;
    set_cur_terminal_line();
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
            close_file();
            new_cmd_line();
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


TabInfo find_input_to_tab() {
	// Find the last space in the line
	StrA fullLine = vecToStrA(wf->back()); // get the full line
    StrA currentLine = (fullLine++)++; // removes the prompt
	int indexNextSpace = currentLine.find(" "a); // get index of last space, -1 if none
	indexNextSpace = (indexNextSpace == -1) ? 0 : indexNextSpace; // if no space, set to 0
	StrA inputToTab = currentLine.slice(indexNextSpace, currentLine.length);

    // Find the next space of current inputToTab to make segment wante 
	int indexNextSpace2 = inputToTab.find(" "a); // get index of last space, -1 if none
	if (indexNextSpace2 != -1) {
		inputToTab = inputToTab.slice(0, indexNextSpace2 - 1); // get the input to tab
	}

	TabInfo tabInfo = { inputToTab, indexNextSpace + 2 }; // +2 to account for the prompt

    return tabInfo; 
}

StrA add_two_stra(StrA str1, StrA str2) {
	MemoryArena& arena = get_scratch_arena(); // Get a memory arena for temporary allocations
	StrA concatenated = StrA{
		arena.alloc<char>(str1.length + str2.length), // Allocate memory
		str1.length + str2.length
	};
	// Copy the first string
	memcpy((void*)concatenated.str, str1.str, str1.length);
	// Copy the second string
	memcpy((void*)(concatenated.str + str1.length), str2.str, str2.length);
	return concatenated;
}


void tab_key() {

	if (terminalMode == TerminalMode::Editor) {
		return;
	}
	if (wf->back().size <= PROMPT_LEN) {
		// line empty
		return;
	}
    Dir* currDir = wd; // current Directory
    ArenaArrayList<char>* updatedLine = &wf->back();

	// Finding the section of the line that needs to be autocompleted(tabbed)
	TabInfo tabInfo = find_input_to_tab();
	StrA inputToTab = tabInfo.portion; // get the input to tab
	int inputToTabSize = inputToTab.length; // size of the input to tab
    StrA tabbedInput = ""a;
	
    // Find matching files to the tabbed input
	ArenaArrayList<StrA> files = find_files(inputToTab);
    // Check if we have matches
    if (files.size == 0)
        return;

	// We have match
	if (files.size == 1) {
		tabbedInput = files[0].skip(inputToTabSize); // get the rest of the file name
        goto onematch;
	}
    // There is more than one match
    while(true){
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

        tabbedInput = add_two_stra(tabbedInput, mostCommonPrefix.prefix);
        lineAdjust++;

    }
	
    onematch:; // goto label for one match 

    updatedLine->insert(tabbedInput.str, tabInfo.index); // add the tabbed input to the line
	curCursorX += tabbedInput.length; // move the cursor to end of input
}

void up_arrow() {
    if (terminalMode == TerminalMode::Cmd) {
        //in terminal
        if (editingHistory == 0) return;
        editingHistory--;
        set_cur_terminal_line();
        make_rightmost();
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
            make_rightmost();
        }
    }
}

void down_arrow() {
    if (terminalMode == TerminalMode::Cmd) {
        //in terminal
        if (editingHistory == wt->history.size - 1) return;
        editingHistory++;
        set_cur_terminal_line();
        make_rightmost();
    } else {
        //in editor
        if (curCursorY == wf->size - 1) { make_rightmost(); return; }
        curCursorY++;
        curCursorX = max(curCursorX, savedCursorX);
        if (getLineLen() < curCursorX) {
            //bad, save
            savedCursorX = max(curCursorX, savedCursorX);
            make_rightmost();
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
        //printf("is rightmost x=%, size=%\n"a, curCursorX, wf->data[curCursorY].size);
        wf->data[curCursorY].push_back(c);
    } else {
        //printf("is not rightmost x=%, size=%\n"a, curCursorX, wf->data[curCursorY].size);
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
            make_rightmost();
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
    } else {
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
        wt->history.back().copy_from_skip_n(wf->back(), PROMPT_LEN);
        return interpretCommand(vecToStrA(wt->history.back()));
    } else {
        if (rightmost()) {
            //newline
            wf->insert(ArenaArrayList<char>{}, curCursorY + 1);
        } else {
            //split
            ArenaArrayList<char>& curLine = wf->data[curCursorY];
            ArenaArrayList<char> lastHalf{}; lastHalf.copy_from_skip_n(curLine, curCursorX);
            wf->insert(lastHalf, curCursorY + 1);
            curLine.copy_from_n(curLine, curCursorX);
        }
        down_arrow();
        curCursorX = 0;
        savedCursorX = 0;
        return false;
    }
}
