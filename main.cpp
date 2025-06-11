#include <gtkmm.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

class HeaderButton : public Gtk::Button {
public:
    HeaderButton() {
        set_label("SYSTEM SETTING");
        set_size_request(679, 43);
    }
};

class MenuButton : public Gtk::Button {
public:
    MenuButton(const Glib::ustring& label)
        : label_text(label) {
        set_label(label);
        set_size_request(113, 53);
    }

private:
    Glib::ustring label_text;
};

class SideButton : public Gtk::Button {
public:
    SideButton(const Glib::ustring& label_text) {
        set_label(label_text);
        set_size_request(91, 87); // Set default size here if needed
    }
};


class PickerWheel : public Gtk::Box {
public:
    PickerWheel(const std::vector<std::string>& items, int visible_items = 3)
    : selected_index_(-1),
      item_height_(20),
      visible_items_(visible_items)
    {
        set_orientation(Gtk::ORIENTATION_VERTICAL);
        set_size_request(70, visible_items_ * item_height_);

        override_background_color(Gdk::RGBA("blue"));
        set_margin_top(8);
        set_margin_bottom(8);
        set_margin_start(8);
        set_margin_end(8);
        set_name("picker-wheel");

        scrolled_window_.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
        scrolled_window_.set_min_content_height(visible_items_ * item_height_);
        scrolled_window_.set_min_content_width(120);
        pack_start(scrolled_window_);

        listbox_.set_selection_mode(Gtk::SELECTION_NONE);

        int padding_count = (visible_items_ - 1) / 2;

        for (int i = 0; i < padding_count; ++i) {
            auto dummy_row = Gtk::manage(new Gtk::ListBoxRow());
            auto dummy_label = Gtk::manage(new Gtk::Label(""));
            dummy_row->add(*dummy_label);
            listbox_.append(*dummy_row);
            row_labels_.push_back(dummy_label);
        }

        for (const auto& item_str : items) {
            auto label = Gtk::manage(new Gtk::Label(item_str));
            label->set_halign(Gtk::ALIGN_CENTER);
            label->set_valign(Gtk::ALIGN_CENTER);
        
            label->set_margin_top(6);
            label->set_margin_bottom(6);
            row_labels_.push_back(label);

            auto row = Gtk::manage(new Gtk::ListBoxRow());
            row->add(*label);
            listbox_.append(*row);
        }

        for (int i = 0; i < padding_count; ++i) {
            auto dummy_row = Gtk::manage(new Gtk::ListBoxRow());
            auto dummy_label = Gtk::manage(new Gtk::Label(""));
            dummy_row->add(*dummy_label);
            listbox_.append(*dummy_row);
            row_labels_.push_back(dummy_label);
        }

        listbox_.show_all();
        scrolled_window_.add(listbox_);
        scrolled_window_.show();

        adjustment_ = scrolled_window_.get_vadjustment();
        adjustment_->signal_value_changed().connect(sigc::mem_fun(*this, &PickerWheel::on_scroll));

        selected_index_ = 0;
        update_selection();

        scroll_timeout_ = sigc::connection();

        scrolled_window_.signal_scroll_event().connect(sigc::mem_fun(*this, &PickerWheel::on_scroll_event), false);
    }

    Glib::ustring get_selected_text() const {
        if (selected_index_ >= 0 && selected_index_ < (int)row_labels_.size())
            return row_labels_[selected_index_]->get_text();
        return {};
    }

    int get_selected_index() const {
        return selected_index_;
    }

    sigc::signal<void, int> signal_selection_changed() {
        return selection_changed_signal_;
    }

    Glib::RefPtr<Gtk::Adjustment> get_adjustment() {
        return adjustment_;
    }

protected:
    bool on_scroll_event(GdkEventScroll* event) {
        if (scroll_timeout_.connected())
            scroll_timeout_.disconnect();

        scroll_timeout_ = Glib::signal_timeout().connect([this]() {
            snap_to_nearest();
            scroll_timeout_.disconnect();
            return false;
        }, 150);

        return false;
    }

    void on_scroll() {
        update_selection();
    }

    void snap_to_nearest() {
        double val = adjustment_->get_value();
        int nearest = static_cast<int>(std::round(val / item_height_));
        nearest = std::max(0, std::min(nearest, (int)row_labels_.size() - 1));
        double target_val = nearest * item_height_;
        adjustment_->set_value(target_val);
    }

    void update_selection() {
        double val = adjustment_->get_value();
        int middle_index = static_cast<int>(std::floor(val / item_height_)) + 1;
        middle_index = std::max(0, std::min(middle_index, (int)row_labels_.size() - 1));

        if (middle_index != selected_index_) {
            if (selected_index_ >= 0 && selected_index_ < (int)row_labels_.size()) {
           
                row_labels_[selected_index_]->override_color(Gdk::RGBA("#6b7280"));
            }

            selected_index_ = middle_index;
            Glib::ustring text = row_labels_[selected_index_]->get_text();
            if (!text.empty()) {
                row_labels_[selected_index_]->override_color(Gdk::RGBA("#f97316"));
                selection_changed_signal_.emit(selected_index_);
            }
        }
    }

private:
    Gtk::ScrolledWindow scrolled_window_;
    Gtk::ListBox listbox_;
    std::vector<Gtk::Label*> row_labels_;
    Glib::RefPtr<Gtk::Adjustment> adjustment_;

    int selected_index_;
    int item_height_;
    int visible_items_;
    sigc::connection scroll_timeout_;

    sigc::signal<void, int> selection_changed_signal_;
};

std::vector<std::string> generateNumbers(int start, int end) {
    std::vector<std::string> items;
    for (int i = start; i <= end; ++i)
        items.push_back(std::to_string(i));
    return items;
}

class PickerComposite : public Gtk::Box {
public:
    PickerComposite(const Glib::ustring& title,
                    const std::vector<std::string>& col1,
                    const std::vector<std::string>& col2,
                    const std::vector<std::string>& col3,
                    const Glib::ustring& button_label)
        : Gtk::Box(Gtk::ORIENTATION_VERTICAL, 4)
    {
        set_size_request(230, 212);
        override_background_color(Gdk::RGBA("white"));
        set_border_width(4);
        set_name("picker-composite");

      auto titleLabel = Gtk::manage(new Gtk::Label(title));
titleLabel->override_color(Gdk::RGBA("#111827"));
titleLabel->set_halign(Gtk::ALIGN_CENTER);
titleLabel->set_margin_bottom(2);             // <-- add this
titleLabel->set_size_request(-1, 20);         // <-- and this
pack_start(*titleLabel, Gtk::PACK_SHRINK);

        auto hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 4));
        pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET);

        picker1_ = new PickerWheel(col1);
        picker2_ = new PickerWheel(col2);
        picker3_ = new PickerWheel(col3);

        hbox->pack_start(*picker1_, Gtk::PACK_EXPAND_WIDGET);
        hbox->pack_start(*picker2_, Gtk::PACK_EXPAND_WIDGET);
        hbox->pack_start(*picker3_, Gtk::PACK_EXPAND_WIDGET);

        auto buttonBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 0));
        pack_start(*buttonBox, Gtk::PACK_SHRINK);

        auto cancelButton = Gtk::manage(new Gtk::Button("Cancel"));
        
        cancelButton->set_size_request(116.47, 35.93);
        buttonBox->pack_start(*cancelButton, Gtk::PACK_EXPAND_WIDGET);

        auto actionButton = Gtk::manage(new Gtk::Button(button_label));
   
        actionButton->set_size_request(113.53, 35.93);
        buttonBox->pack_start(*actionButton, Gtk::PACK_EXPAND_WIDGET);

        cancelButton->signal_clicked().connect([this]() {
            set_all_active(0);
        });

        actionButton->signal_clicked().connect([this, button_label]() {
            printf("%s: Selected values: %s, %s, %s\n",
                   button_label.c_str(),
                   picker1_->get_selected_text().c_str(),
                   picker2_->get_selected_text().c_str(),
                   picker3_->get_selected_text().c_str());
        });

        show_all_children();
    }

    void set_all_active(int index) {
        scroll_to_index(picker1_, index);
        scroll_to_index(picker2_, index);
        scroll_to_index(picker3_, index);
    }

private:
    void scroll_to_index(PickerWheel* picker, int index) {
        if (index < 0) index = 0;
        int maxIndex = static_cast<int>(picker->get_adjustment()->get_upper() / 40);
        index = std::min(index, maxIndex - 1);
        double pos = index * 40;
        picker->get_adjustment()->set_value(pos);
    }

    PickerWheel* picker1_;
    PickerWheel* picker2_;
    PickerWheel* picker3_;
};

int main(int argc, char* argv[]){
   
auto app = Gtk::Application::create(argc, argv, "com.example.systemsettings");



    // Main Window
    Gtk::Window window;
    window.set_title("Settings");
    window.set_default_size(800, 480);
    window.set_name("main-window");

    Gtk::Fixed layout;
    window.add(layout);

    // Left Panel
    auto leftBox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL);
    leftBox->set_size_request(679, 480);
    layout.put(*leftBox, 0, 0);

    auto header = Gtk::make_managed<HeaderButton>();
    leftBox->pack_start(*header, Gtk::PACK_SHRINK);

    // Feature Box
  auto featureBox = Gtk::make_managed<Gtk::Fixed>();
    featureBox->set_size_request(679, 342);
    layout.put(*featureBox, 8, 73);

    auto label_datetime = Gtk::make_managed<Gtk::Label>("Date and Time");
    label_datetime->set_size_request(112, 22);
    featureBox->put(*label_datetime, 66, 0); 
    auto label_memory = Gtk::make_managed<Gtk::Label>("Memory");
    label_memory->set_size_request(82, 22);
    featureBox->put(*label_memory, 298, 0);

    auto label_network = Gtk::make_managed<Gtk::Label>("Network");
    label_network->set_size_request(92, 22);
    featureBox->put(*label_network, 521, 0);
    // Date Picker
auto pickerContainer = Gtk::make_managed<Gtk::Fixed>();
pickerContainer->set_size_request(679, 314);
featureBox->put(*pickerContainer, 0, 28); // 28 px from top of featureBox

// Set Date
auto datePicker = new PickerComposite(
    "Set Date",
    generateNumbers(1, 31),
    { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" },
    generateNumbers(1980, 2025),
    "Edit"
);
datePicker->set_size_request(230, 212);
pickerContainer->put(*datePicker, 59, 111);

// Set Time
auto timePicker = new PickerComposite(
    "Set Time",
    generateNumbers(1, 12),
    generateNumbers(0, 59),
    { "AM", "PM" },
    "Set"
);
timePicker->set_size_request(230, 212);
pickerContainer->put(*timePicker, 417, 111);
// Display box for selected Date & Time
auto selectedInfoBox = Gtk::make_managed<Gtk::Label>(""); // Initially empty
selectedInfoBox->set_size_request(150, 34);
selectedInfoBox->override_background_color(Gdk::RGBA("#e5e7eb")); // light gray
selectedInfoBox->override_color(Gdk::RGBA("#111827")); // dark text
selectedInfoBox->set_halign(Gtk::ALIGN_CENTER);
selectedInfoBox->set_valign(Gtk::ALIGN_CENTER);
selectedInfoBox->set_margin_bottom(6);
selectedInfoBox->set_justify(Gtk::JUSTIFY_CENTER);
selectedInfoBox->set_xalign(0.5);
selectedInfoBox->set_yalign(0.5);

// Position the box inside the pickerContainer
pickerContainer->put(*selectedInfoBox, 139, 64);


    // Menu Box
    auto menuBox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
    menuBox->set_size_request(682, 53);
    std::vector<Glib::ustring> labels = {"PRESET", "PROFILE", "APPEARANCE", "SYSTEM", "SCOPE", "RESET"};
    for (const auto& label : labels) {
        auto btn = Gtk::make_managed<MenuButton>(label);
        menuBox->pack_start(*btn, Gtk::PACK_EXPAND_WIDGET);
    }
    leftBox->pack_end(*menuBox, Gtk::PACK_SHRINK);



// Sidebar box for buttons
auto rightBar = Gtk::make_managed<Gtk::Fixed>();
rightBar->set_size_request(95, 468);
rightBar->set_name("rightbar");

// Buttons
auto btn_patient = Gtk::make_managed<SideButton>("Patient");
auto btn_home = Gtk::make_managed<SideButton>("Home");
auto btn_settings = Gtk::make_managed<SideButton>("Settings");

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
   
