#pragma once
#include <gtkmm.h>
#include <memory>
#include "managers.h"

class SystemContentLayout : public Gtk::Box {
private:
    Gtk::Stack* stackContainer_;
    std::unique_ptr<NavigationManager> navigation_manager_;
    Gtk::EventBox* selectionIndicator_;
    Gtk::Fixed* navBox_;

    void moveIndicatorTo(int target_x);
    void setup_ui();

public:
    SystemContentLayout();
    void show_layout();
    void hide_layout();
};
