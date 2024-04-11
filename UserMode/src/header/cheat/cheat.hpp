#pragma once

#include <array>
#include <vector>
#include <unordered_set>
#include <stdexcept>
#include <functional>

#include "../driver/driver.hpp"
#include "../overlay/overlay.hpp"

namespace cheat {
	namespace features {
		constexpr int GLOW = 1 << 0;
		constexpr int ESP = 1 << 1;
		constexpr int AIMBOT = 1 << 2;
		constexpr int NO_FLASH = 1 << 3;
	}

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
        MenuManager() {
            // 初始化菜单数据
            menu_bars.push_back({ "Main Menu", {
                {"Toggle Option", OptionType::Toggle, true},
                {"Slider Option", OptionType::Slider, false, 50, 0, 100}
            } });
        }

        void ProcessInput() {
            // 检测按键状态
            CheckKeyState(VK_ADD, [this]() { menu_enabled = !menu_enabled; });
            CheckKeyState(VK_NUMPAD7, [this]() { ChangeMenuBar(-1); });
            CheckKeyState(VK_NUMPAD1, [this]() { ChangeMenuBar(1); });
            CheckKeyState(VK_NUMPAD8, [this]() { ChangeOption(-1); });
            CheckKeyState(VK_NUMPAD2, [this]() { ChangeOption(1); });
            CheckKeyState(VK_NUMPAD4, [this]() { ChangeSliderValue(-1); });
            CheckKeyState(VK_NUMPAD6, [this]() { ChangeSliderValue(1); });
            CheckKeyState(VK_NUMPAD5, [this]() { ToggleOption(); });
        }

        // 获取指定菜单栏中指定选项的开关值
        const bool GetToggleValue(size_t menu_bar_index, size_t option_index) {
            return GetOption(menu_bar_index, option_index).toggle_value;
        }

        // 设置指定菜单栏中指定选项的开关值
        void SetToggleValue(size_t menu_bar_index, size_t option_index, bool value) {
            Option& option = GetOption(menu_bar_index, option_index);
            if (option.type != OptionType::Toggle) {
                throw std::invalid_argument("Option type is not Toggle");
            }
            option.toggle_value = value;
        }

        // 获取指定菜单栏中指定选项的滑条值
        const int GetSliderValue(size_t menu_bar_index, size_t option_index) {
            return GetOption(menu_bar_index, option_index).slider_value;
        }

        // 设置指定菜单栏中指定选项的滑条值
        void SetSliderValue(size_t menu_bar_index, size_t option_index, int value) {
            Option& option = GetOption(menu_bar_index, option_index);
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

    private:
        std::vector<MenuBar> menu_bars;
        bool menu_enabled = false;
        int current_menu_bar = 0;
        int current_option = 0;
        std::unordered_set<int> key_states; // 记录按键状态

        // 获取指定菜单栏中指定选项的引用
        Option& GetOption(size_t menu_bar_index, size_t option_index) {
            if (menu_bar_index >= menu_bars.size()) {
                throw std::out_of_range("Invalid menu bar index");
            }
            MenuBar& menu_bar = menu_bars[menu_bar_index];
            if (option_index >= menu_bar.options.size()) {
                throw std::out_of_range("Invalid option index");
            }
            return menu_bars[menu_bar_index].options[option_index];
        }

        void ChangeMenuBar(int offset) {
            int new_index = (current_menu_bar + offset) % menu_bars.size();
            new_index = (new_index + menu_bars.size()) % menu_bars.size();
            current_menu_bar = new_index;
            current_option = 0;
        }

        void ChangeOption(int offset) {
            MenuBar& menu_bar = menu_bars[current_menu_bar];
            int new_index = (current_option + offset) % menu_bar.options.size();
            new_index = (new_index + menu_bar.options.size()) % menu_bar.options.size();
            current_option = new_index;
        }

        void ChangeSliderValue(int offset) {
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

        void ToggleOption() {
            Option& option = menu_bars[current_menu_bar].options[current_option];
            if (option.type == OptionType::Toggle) {
                option.toggle_value = !option.toggle_value;
            }
        }

        void CheckKeyState(int key, const std::function<void()>& action) {
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

		void menu();
		void render();


		void print_error_info(const int error_code);
	};
}