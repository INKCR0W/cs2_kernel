#pragma once

#include <array>
#include <vector>
#include <unordered_set>
#include <stdexcept>
#include <functional>

#include "../driver/driver.hpp"
#include "../overlay/overlay.hpp"
#include "../util/structs.hpp"

namespace cheat {
    enum bones : int {
        head = 6,
        neck = 5,
        chest = 4,
        shoulderRight = 8,
        shoulderLeft = 13,
        elbowRight = 9,
        elbowLeft = 14,
        handRight = 11,
        handLeft = 16,
        crotch = 0,
        kneeRight = 23,
        kneeLeft = 26,
        ankleRight = 24,
        ankleLeft = 27,
    };

    inline namespace bone_groups {
        inline std::vector<int> mid = { bones::head,bones::neck,bones::chest,bones::crotch };
        inline std::vector<int> left_arm = { bones::neck,bones::shoulderLeft,bones::elbowLeft,bones::handLeft };
        inline std::vector<int> right_arm = { bones::neck,bones::shoulderRight,bones::elbowRight,bones::handRight };
        inline std::vector<int> left_leg = { bones::crotch,bones::kneeLeft,bones::ankleLeft };
        inline std::vector<int> right_leg = { bones::crotch,bones::kneeRight,bones::ankleRight };
        inline std::vector<std::vector<int>> all_groups = { mid, left_arm, right_arm, left_leg, right_leg };
    };


	namespace features {
		constexpr int GLOW = 1 << 0;
		constexpr int ESP = 1 << 1;
		constexpr int AIMBOT = 1 << 2;
		constexpr int NO_FLASH = 1 << 3;
	}

    class entity {
    public:
        uintptr_t controller;
        uintptr_t pawn;
    };

	//namespace hotkeys {
	//	constexpr int MENU = 1 << 0;
	//	constexpr int PAD_0 = 1 << 1;
	//	constexpr int PAD_1 = 1 << 2;
	//	constexpr int PAD_2 = 1 << 3;
	//	constexpr int PAD_4 = 1 << 4;
	//	constexpr int PAD_5 = 1 << 5;
	//	constexpr int PAD_6 = 1 << 6;
	//	constexpr int PAD_7 = 1 << 7;
	//	constexpr int PAD_8 = 1 << 8;
	//}

	//class hotkey_data {
	//public:
	//	int vk_code;
	//	int feature_code;

	//	hotkey_data(const int _vk, const int _feature) : vk_code(_vk), feature_code(_feature) {}
	//};


    // 菜单选项类型
    enum class OptionType { Toggle, Slider };

    // 菜单选项结构体
    struct Option {
        std::string name;
        OptionType type;
        bool toggle_value = false;
        int slider_value = 0;
        int slider_min = 0;
        int slider_max = 100;
    };

    // 菜单栏结构体
    struct MenuBar {
        std::string name;
        std::vector<Option> options;
    };

    // 菜单管理类
    class MenuManager {
    public:
        MenuManager() : menu_bars({{ "Main Menu", {
                {"Toggle Option", OptionType::Toggle, true},
                {"Slider Option", OptionType::Slider, false, 50, 0, 100}
            } } }) {}

        void process_input() {
            // 检测按键状态
            check_key_state(VK_ADD, [this]() { menu_enabled = !menu_enabled; });
            check_key_state(VK_NUMPAD7, [this]() { change_menu_bar(-1); });
            check_key_state(VK_NUMPAD1, [this]() { change_menu_bar(1); });
            check_key_state(VK_NUMPAD8, [this]() { change_option(-1); });
            check_key_state(VK_NUMPAD2, [this]() { change_option(1); });
            check_key_state(VK_NUMPAD4, [this]() { change_slider_value(-1); });
            check_key_state(VK_NUMPAD6, [this]() { change_slider_value(1); });
            check_key_state(VK_NUMPAD5, [this]() { toggle_option(); });
        }

        // 获取指定菜单栏中指定选项的开关值
        const bool get_toggle_value(size_t menu_bar_index, size_t option_index) {
            return get_option(menu_bar_index, option_index).toggle_value;
        }

        // 设置指定菜单栏中指定选项的开关值
        void set_toggle_value(size_t menu_bar_index, size_t option_index, bool value) {
            Option& option = get_option(menu_bar_index, option_index);
            if (option.type != OptionType::Toggle) {
                throw std::invalid_argument("Option type is not Toggle");
            }
            option.toggle_value = value;
        }

        // 获取指定菜单栏中指定选项的滑条值
        const int get_slider_value(size_t menu_bar_index, size_t option_index) {
            return get_option(menu_bar_index, option_index).slider_value;
        }

        // 设置指定菜单栏中指定选项的滑条值
        void set_slider_value(size_t menu_bar_index, size_t option_index, int value) {
            Option& option = get_option(menu_bar_index, option_index);
            if (option.type != OptionType::Slider) {
                throw std::invalid_argument("Option type is not Slider");
            }
            if (value < option.slider_min) {
                option.slider_value = option.slider_min;
            }
            else if (value > option.slider_max) {
                option.slider_value = option.slider_max;
            }
            else {
                option.slider_value = value;
            }
        }

        // 获取整个菜单信息的只可读引用
        const std::vector<MenuBar>& get_menu() {
            return menu_bars;
        }

    private:
        std::vector<MenuBar> menu_bars;
        bool menu_enabled = false;
        int current_menu_bar = 0;
        int current_option = 0;
        std::unordered_set<int> key_states; // 记录按键状态

        // 获取指定菜单栏中指定选项的引用
        Option& get_option(size_t menu_bar_index, size_t option_index) {
            if (menu_bar_index >= menu_bars.size()) {
                throw std::out_of_range("Invalid menu bar index");
            }
            MenuBar& menu_bar = menu_bars[menu_bar_index];
            if (option_index >= menu_bar.options.size()) {
                throw std::out_of_range("Invalid option index");
            }
            return menu_bars[menu_bar_index].options[option_index];
        }

        void change_menu_bar(int offset) {
            int new_index = (current_menu_bar + offset) % menu_bars.size();
            new_index = (new_index + menu_bars.size()) % menu_bars.size();
            current_menu_bar = new_index;
            current_option = 0;
        }

        void change_option(int offset) {
            MenuBar& menu_bar = menu_bars[current_menu_bar];
            int new_index = (current_option + offset) % menu_bar.options.size();
            new_index = (new_index + menu_bar.options.size()) % menu_bar.options.size();
            current_option = new_index;
        }

        void change_slider_value(int offset) {
            Option& option = menu_bars[current_menu_bar].options[current_option];
            if (option.type == OptionType::Slider) {
                int new_value = option.slider_value + offset;
                if (new_value < option.slider_min) {
                    option.slider_value = option.slider_min;
                }
                else if (new_value > option.slider_max) {
                    option.slider_value = option.slider_max;
                }
                else {
                    option.slider_value = new_value;
                }
            }
        }

        void toggle_option() {
            Option& option = menu_bars[current_menu_bar].options[current_option];
            if (option.type == OptionType::Toggle) {
                option.toggle_value = !option.toggle_value;
            }
        }

        void check_key_state(int key, const std::function<void()>& action) {
            if (GetAsyncKeyState(key) & 0x8000) {
                if (key_states.count(key) == 0) {
                    key_states.insert(key);
                    action();
                }
            }
            else {
                key_states.erase(key);
            }
        }
    };


	class Cheat : public driver::Driver {
	public:
		Cheat();
		Cheat(const Cheat& _) = delete;
		~Cheat();

		bool run();

	private:
		overlay::Overlay* overlay;
		//const std::array<const hotkey_data, 9> hotkeys;

		const float screen_size_x;
		const float screen_size_y;
		const float menu_render_y;

		int feature;
		//int feature_hotkey;
		//int hotkey_pushed;

        uintptr_t client_dll;

        bool in_match;
        uintptr_t local_player_pawn;
        uintptr_t local_player_controller;
        std::vector<entity> entity_list;


		void menu();
        bool update_entity();
        void esp();

        void render();


        Vec2 world_to_screen(view_matrix_t matrix, Vec3 position) const;
		void print_error_info(const int error_code);
	};
}