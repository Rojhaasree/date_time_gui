#include <gtkmm.h>
#include <vector>
#include <string>
#include "layout.h"
#include "components.h"

int main(int argc, char* argv[]){
    auto app = Gtk::Application::create(argc, argv, "com.example.systemsettings");

    Gtk::Window window;
    window.set_title("Settings");
    window.set_default_size(800, 480);
    window.set_name("main-window");

    Gtk::Fixed layout;
    window.add(layout);

    auto leftBox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL);
    leftBox->set_size_request(679, 480);
    layout.put(*leftBox, 0, 0);

    auto systemLayout = Gtk::make_managed<SystemContentLayout>();
    leftBox->pack_start(*systemLayout, Gtk::PACK_EXPAND_WIDGET);

    auto menuBox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
    menuBox->set_size_request(682, 53);
    std::vector<Glib::ustring> labels = {"PRESET", "PROFILE", "APPEARANCE", "SYSTEM", "SCOPE", "RESET"};
    std::vector<Gtk::Button*> menu_buttons; // Store button references

    for (const auto& label : labels) {
        auto btn = Gtk::make_managed<MenuButton>(label);
        btn->get_style_context()->add_class("gradient-menu-button");
        menu_buttons.push_back(btn);
        
        if (label == "SYSTEM") {
            // Set SYSTEM as initially selected
            btn->get_style_context()->add_class("selected");
            
            btn->signal_clicked().connect([systemLayout, btn, &menu_buttons]() {
                // Remove selected class from all buttons
                for (auto* button : menu_buttons) {
                    button->get_style_context()->remove_class("selected");
                }
                // Add selected class to clicked button
                btn->get_style_context()->add_class("selected");
                systemLayout->show_layout();
            });
        } else {
            btn->signal_clicked().connect([systemLayout, btn, &menu_buttons]() {
                // Remove selected class from all buttons
                for (auto* button : menu_buttons) {
                    button->get_style_context()->remove_class("selected");
                }
                // Add selected class to clicked button
                btn->get_style_context()->add_class("selected");
                systemLayout->hide_layout();
            });
        }
        
        menuBox->pack_start(*btn, Gtk::PACK_EXPAND_WIDGET);
    }
    leftBox->pack_end(*menuBox, Gtk::PACK_SHRINK);

    auto rightBar = Gtk::make_managed<Gtk::Fixed>();
    rightBar->set_size_request(95, 468);
    rightBar->set_name("rightbar");
    rightBar->get_style_context()->add_class("sidebar-box");

    auto btn_patient = Gtk::make_managed<SideButton>("Patient");
    auto btn_home = Gtk::make_managed<SideButton>("Home");
    auto btn_settings = Gtk::make_managed<SideButton>("Settings");

    btn_patient->get_style_context()->add_class("nav-button");
    btn_home->get_style_context()->add_class("nav-button");
    btn_settings->get_style_context()->add_class("nav-button");

    btn_patient->set_size_request(75, 93);
    btn_home->set_size_request(61, 87);
    btn_settings->set_size_request(91, 87);

    rightBar->put(*btn_patient, 10, 32);
    rightBar->put(*btn_home, 17, 191);
    rightBar->put(*btn_settings, 1, 344);

    layout.put(*rightBar, 695, 6);

    window.show_all();
    return app->run(window);
}
