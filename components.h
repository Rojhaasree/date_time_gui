#pragma once
#include <gtkmm.h>
#include <vector>
#include <string>
#include <memory>
#include "interfaces.h"
#include "behaviors.h"

class HeaderButton : public Gtk::Button {
public:
    HeaderButton();
};

class MenuButton : public Gtk::Button {
public:
    MenuButton(const Glib::ustring& label);
private:
    Glib::ustring label_text;
};

class SideButton : public Gtk::Button {
public:
    SideButton(const Glib::ustring& label_text);
};

class PickerWheel : public Gtk::Box {
private:
    Gtk::ScrolledWindow scrolled_window_;
    Gtk::ListBox listbox_;
    std::vector<Gtk::Label*> row_labels_;
    std::vector<std::string> items_;
    Glib::RefPtr<Gtk::Adjustment> adjustment_;
    std::unique_ptr<PickerScrollBehavior> scroll_behavior_;
    std::unique_ptr<PickerSelectionManager> selection_manager_;
    int visible_items_;
    int item_height_;

public:
    PickerWheel(const std::vector<std::string>& items, int visible_items = 3);
    Glib::ustring get_selected_text() const;
    int get_selected_index() const;
    const std::vector<std::string>& get_items() const;
    size_t get_items_size() const;
    sigc::signal<void, int> signal_selection_changed();
    Glib::RefPtr<Gtk::Adjustment> get_adjustment();
};

class PickerComposite : public Gtk::Box, public IPickerSelectionHandler {
private:
    PickerWheel* picker1_;
    PickerWheel* picker2_;
    PickerWheel* picker3_;
    std::unique_ptr<class PickerActionHandler> action_handler_;
    
    void scroll_to_index(PickerWheel* picker, int index);

public:
    PickerComposite(const Glib::ustring& title,
                    const std::vector<std::string>& col1,
                    const std::vector<std::string>& col2,
                    const std::vector<std::string>& col3,
                    const Glib::ustring& button_label,
                    Gtk::Label* display_label = nullptr,
                    bool is_time_picker = false,
                    bool is_date_picker = false);

    void onSelectionChanged() override;
    void set_all_active(int index);
};
