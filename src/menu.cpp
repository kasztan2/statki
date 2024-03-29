#include "menu.h"
#include <fstream>

Menu::Menu(int height, int width)
{
    _menu = newwin(height, width, LINES / 2 - height / 2, COLS / 2 - width / 2);
    _height = height;
    _width = width;
    _selected_option = 0;
    _prev_selected_option = 0;
    _curr_section = menu;
    start_color();
    init_colors();
    wybor_bota=0;
}

Menu::~Menu()
{
    delwin(_menu);
}

void Menu::start()
{
    init(_menu_options);
}

void Menu::update()
{
    refresh();
    wrefresh(_menu);
}

void Menu::clear()
{
    wclear(stdscr);
    wclear(_menu);
    update();
}

void Menu::init(const std::vector<std::string>& options)
{
    check_wind();
    
    print_menu(options);
    if (_curr_section != instructions) {
        print_file();
    }
    update();

    while (true) {
        get_move();
        if (_curr_section == end) {
            clear();
            break;
        }

        clear();
        print_menu(options);
        if (_curr_section != instructions) {
            print_file();
        }
    }
}

void Menu::print_menu(const std::vector<std::string>& options)
{
    int y = getmaxy(_menu) / 2 + options.size();
    int half_col = getmaxx(_menu) / 2;

    for (int i = 0; i < options.size(); ++i) {
        int x = half_col - options[i].length() / 2;
        if (_selected_option == i) {
            print_selected_option(options[i], y, x);
        } else {
            wattron(_menu, A_DIM);
            mvwprintw(_menu, y, x, "%s", options[i].c_str());
            wattroff(_menu, A_DIM);
        }
        y += 2;
    }
    update();
}

int Menu::get_size()
{
    if (_curr_section == menu) {
        return _menu_options.size();
    } else if (_curr_section == new_game) {
        return _new_game_options.size();
    }
    return 1;
}

void Menu::move_up()
{
    --_selected_option;
    if (_selected_option <= -1) {
        _selected_option = get_size() - 1;
    }
}

void Menu::move_down()
{
    ++_selected_option;
    if (_selected_option >= get_size()) {
        _selected_option = 0;
    }
}

void Menu::init_colors()
{
    init_pair(MAG_BLACK, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(CYAN_BLACK, COLOR_CYAN, COLOR_BLACK);
    init_pair(BLUE_BLACK, COLOR_BLUE, COLOR_BLACK);
}

void Menu::get_move()
{
    switch (wgetch(_menu)) {
    case KEY_UP:
    case 'w':
        move_up();
        break;

    case KEY_DOWN:
    case 's':
        move_down();
        break;

    case K_ENTER:
    case ' ':
        clear();
        select_option();
        break;
        
    case KEY_RESIZE:
        _menu = newwin(_height, _width, LINES / 2 - _height / 2, COLS / 2 - _width / 2);
        check_wind();
        break;
        
    default:
        break;
    }
}

// TODO
void Menu::select_option()
{
    switch (_selected_option) {
    case 0:
        if (_curr_section == menu) {
            _curr_section = new_game;
            section_new_game();
        } else if (_curr_section == new_game) {
            _curr_section = end;
            //            wywolaj bota nr 1
            wybor_bota=0;
        } else if (_curr_section == instructions) {
            section_return();
        }
        break;

    case 1:
        if (_curr_section == menu) {
            _curr_section = instructions;
            section_instructions();
        } else if (_curr_section == new_game) {
            _curr_section = end;
            //            wywolaj bota nr 2
            wybor_bota=1;
        }
        break;

    case 2:
        if (_curr_section == menu) {
            _curr_section = end;
            section_exit();
        } else if (_curr_section == new_game) {
            _curr_section = end;
            //            wywolaj bota nr 3
            wybor_bota=2;
        }
        break;

    case 3:
        if (_curr_section == new_game) {
            section_return();
        }
        break;

    default:
        break;
    }
}

void Menu::print_selected_option(const std::string& option, int y, int x)
{
    wattron(_menu, A_BLINK | COLOR_PAIR(MAG_BLACK));
    mvwprintw(_menu, y, x - 3, "->");
    wattroff(_menu, A_BLINK | COLOR_PAIR(MAG_BLACK));

    wattron(_menu, A_BOLD);
    mvwprintw(_menu, y, x, "%s", option.c_str());
    wattroff(_menu, A_BOLD);

    wattron(_menu, A_BLINK | COLOR_PAIR(MAG_BLACK));
    mvwprintw(_menu, y, getcurx(_menu) + 1, "<-");
    wattroff(_menu, A_BLINK | COLOR_PAIR(MAG_BLACK));
}

void Menu::section_return()
{
    _curr_section = menu;
    _selected_option = _prev_selected_option;
    clear();
    init(_menu_options);
}

void Menu::section_exit()
{
    _curr_section = end;
    sprzatanie_przy_wyjsciu();
    exit(0);
}

void Menu::section_new_game()
{
    _prev_selected_option = _selected_option;
    _selected_option = 0;
    init(_new_game_options);
}

void Menu::section_instructions()
{
    std::ifstream file("assets/instrukcje.txt");
    _prev_selected_option = _selected_option;
    _selected_option = 0;
    print_file();
}

void Menu::print_file()
{
    std::string line;
    std::ifstream file;
    int y = 0;
    if (_curr_section == menu) {
        file.open("assets/statki_napis.txt");
        y = getmaxy(_menu) / 8;
    } else if (_curr_section == new_game){
        file.open("assets/statek_ascii.txt");
        y = getmaxy(_menu) / 4;
    } else {
        file.open("assets/instrukcje.txt");
        y = getmaxy(_menu) / 4;
    }

    if (file.is_open()) {
        wattron(_menu, A_BOLD);
        while (getline(file, line)) {
            if (_curr_section == instructions && !line.empty() && line.back() != ':') {
                wattroff(_menu, A_BOLD);
                mvwprintw(_menu, y, getmaxx(_menu) / 2 - line.length() / 2, "%s", line.c_str());
                wattron(_menu, A_BOLD);
            } else {            
                mvwprintw(_menu, y, getmaxx(_menu) / 2 - line.length() / 2, "%s", line.c_str());
            }
            ++y;
        }
        file.close();
        wattroff(_menu, A_BOLD);
    }

    if (_curr_section == new_game) {
        std::string tmp = "Wybierz przeciwnika:";
        wattron(_menu, A_BOLD);
        mvwprintw(_menu, y + 2, getmaxx(_menu) / 2 - tmp.length() / 2, "%s", tmp.c_str());
        wattroff(_menu, A_BOLD);
    } else if (_curr_section == instructions) {
        std::string tmp = "Wróć do menu";
        print_selected_option(tmp, y + 2, getmaxx(_menu) / 2 - tmp.length() / 2);
        while (_curr_section == instructions) {
            get_move();
        }
    }
}

void Menu::print_too_small()
{
    std::string msg = "ZA MAŁY TERMINAL";
    int x = COLS / 2  - msg.length() / 2;
    int y = LINES / 2;
    mvprintw(y, x, "%s", msg.c_str());
}

void Menu::check_wind()
{
    while (_menu == nullptr) {
        wclear(stdscr);
        refresh();
        print_too_small();
        if (getch() == KEY_RESIZE) {
            _menu = newwin(_height, _width, LINES / 2 - _height / 2, COLS / 2 - _width / 2);
        }
    }
    keypad(_menu, true);
}
