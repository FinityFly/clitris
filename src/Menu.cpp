#include <ncurses.h>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include <chrono>

#include "../include/Menu.h"
#include "../include/Settings.h"

// Use std::pair<std::string, std::string> for hotkey and label
#ifdef __linux__
static std::vector<std::vector<std::pair<std::string, std::string>>> menu_options = {
    {
        {"1", "Sprint"},
        {"2", "Blitz"},
        {"3", "Zen"},
        {"4", "Cheese"},
        {"5", "Settings"},
        {"q", "Exit"}
    },
    {
        {"1", "20L Sprint"},
        {"2", "40L Sprint"},
        {"3", "100L Sprint"},
        {"q", "Back to Main Menu"}
    },
    {
        {"1", "1:00 Blitz"},
        {"2", "2:00 Blitz"},
        {"3", "4:00 Blitz"},
        {"q", "Back to Main Menu"}
    },
    {
        {"1", "10L Cheese"},
        {"2", "18L Cheese"},
        {"3", "100L Cheese"},
        {"q", "Back to Main Menu"}
    }
};

static std::vector<std::string> titles = {
    "CLITRIS",
    "SPRINT MODE",
    "BLITZ MODE",
    "CHEESE MODE",
};
#else
// macOS/Windows version with emojis
static std::vector<std::vector<std::pair<std::string, std::string>>> menu_options = {
    {
        {"1", "🚀 Sprint"},
        {"2", "🔥 Blitz"},
        {"3", "🧘 Zen"},
        {"4", "🧀 Cheese"},
        {"5", "⚙️  Settings"},
        {"q", "←  Exit"}
    },
    {
        {"1", "🚀 20L Sprint"},
        {"2", "🚀 40L Sprint"},
        {"3", "🚀 100L Sprint"},
        {"q", "←  Back to Main Menu"}
    },
    {
        {"1", "🔥 1:00 Blitz"},
        {"2", "🔥 2:00 Blitz"},
        {"3", "🔥 4:00 Blitz"},
        {"q", "←  Back to Main Menu"}
    },
    {
        {"1", "🧀 10L Cheese"},
        {"2", "🧀 18L Cheese"},
        {"3", "🧀 100L Cheese"},
        {"q", "←  Back to Main Menu"}
    }
};

static std::vector<std::string> titles = {
    "🎮 CLITRIS",
    "🚀 SPRINT MODE",
    "🔥 BLITZ MODE",
    "🧀 CHEESE MODE",
};
#endif

void Menu::display(int state) {
    const auto& options = menu_options[state];
    const std::string& title = titles[state];

    int max_width = 0;
    for (const auto& opt : options) {
        int line_width = static_cast<int>(opt.first.size()) + 4 + static_cast<int>(opt.second.size());
        max_width = std::max(max_width, line_width);
    }
    max_width = std::max(max_width, static_cast<int>(title.size()));
    max_width = std::max(max_width, 40);

    const int HORIZONTAL_PADDING = 12;
    const int VERTICAL_PADDING = 8;

    int box_width = max_width + HORIZONTAL_PADDING;
    int box_height = static_cast<int>(options.size()) + VERTICAL_PADDING;

    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols);
    int start_y = (term_rows - box_height) / 2;
    int start_x = (term_cols - box_width) / 2;

    choice = -1;
    int selected_option = 0;

    clear();
    refresh();

    // main menu window with double border effect
    WINDOW* outerwin = newwin(box_height + 2, box_width + 4, start_y - 1, start_x - 2);
    WINDOW* menuwin = newwin(box_height, box_width, start_y, start_x);

    wattron(outerwin, A_DIM);
    box(outerwin, 0, 0);
    wattroff(outerwin, A_DIM);
    wrefresh(outerwin);

    // Function to draw the menu with current selection highlighted
    auto drawMenu = [&]() {
        werase(menuwin);
        
        wattron(menuwin, A_BOLD);
        box(menuwin, 0, 0);
        wattroff(menuwin, A_BOLD);

        // title
        int title_x = (box_width - title.size()) / 2 + 1;
        mvwprintw(menuwin, 1, title_x, "%s", title.c_str());

        // separator line
        wattron(menuwin, A_DIM);
        for (int i = 2; i < box_width - 2; i++) {
            mvwprintw(menuwin, 2, i, "=");
        }
        wattroff(menuwin, A_DIM);

        // menu options
        for (size_t i = 0; i < options.size(); ++i) {
            int opt_y = static_cast<int>(i) + 4;
            
            if (static_cast<int>(i) == selected_option) {
                wattron(menuwin, A_BOLD);
            }
            
            mvwprintw(menuwin, opt_y, 4, "[%s]", options[i].first.c_str());
            mvwprintw(menuwin, opt_y, 8, "%s", options[i].second.c_str());
            
            if (static_cast<int>(i) == selected_option) {
                wattroff(menuwin, A_BOLD);
            }
        }

        // bottom instruction
        wattron(menuwin, A_DIM);
#ifdef __linux__
        std::string instruction = "[^/v]: navigate - [ENTER]: select";
#else
        std::string instruction = "[↑/↓]: navigate - [ENTER]: select";
#endif
        int instr_x = (box_width - instruction.size()) / 2 + 2;
        mvwprintw(menuwin, box_height - 2, instr_x, "%s", instruction.c_str());
        wattroff(menuwin, A_DIM);

        wrefresh(menuwin);
    };

    drawMenu();

    nodelay(stdscr, FALSE);

    while (true) {
        int ch = getch();
        bool selection_made = false;
        
        if (ch == KEY_UP) {
            selected_option = (selected_option - 1 + static_cast<int>(options.size())) % static_cast<int>(options.size());
            drawMenu();
            continue;
        } else if (ch == KEY_DOWN) {
            selected_option = (selected_option + 1) % static_cast<int>(options.size());
            drawMenu();
            continue;
        } else if (ch == '\n' || ch == KEY_ENTER) {
            choice = (options[selected_option].first == "q") ? -1 : (selected_option + 1);
            selection_made = true;
        } else {
            for (size_t i = 0; i < options.size(); ++i) {
                if (ch == options[i].first[0]) {
                    choice = (options[i].first == "q") ? -1 : (int)(i + 1);
                    selection_made = true;
                    break;
                }
            }
        }
        
        if (selection_made) {
            delwin(menuwin);
            delwin(outerwin);
            return;
        }
        
        wattron(menuwin, COLOR_PAIR(7) | A_BOLD);
#ifdef __linux__
        std::string error_msg = "Invalid choice, try again.";
#else
        std::string error_msg = "⚠️  Invalid choice, try again.";
#endif
        int error_x = (box_width - error_msg.size()) / 2 + 2;
        mvwprintw(menuwin, box_height - 4, error_x, "%s", error_msg.c_str());
        wattroff(menuwin, COLOR_PAIR(7) | A_BOLD);
        wrefresh(menuwin);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        drawMenu();
    }
}

int Menu::getChoice() {
    return choice;
}
