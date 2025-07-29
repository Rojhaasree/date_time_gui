#include "components.h"
#include "handlers.h"
#include "managers.h"
#include "utils.h"

HeaderButton::HeaderButton() {
    set_label("SYSTEM SETTING");
    set_size_request(679, 43);
    auto css_provider = Gtk::CssProvider::create();
    css_provider->load_from_data(R"(
        button {
            background-image: none;
            background-color: #2C3435;
            color: white;
            border-radius: 5px;
            font-weight: bold;
            border: none;
        }
    )");
    
    get_style_context()->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

MenuButton::MenuButton(const Glib::ustring& label)
    : label_text(label) {
    set_label(label);
    set_size_request(113, 53);
}

SideButton::SideButton(const Glib::ustring& label_text) {
    set_label(label_text);
    set_size_request(91, 87);
}

PickerWheel::PickerWheel(const std::vector<std::string>& items, int visible_items)
: items_(items), visible_items_(visible_items), item_height_(20)
{
    set_orientation(Gtk::ORIENTATION_VERTICAL);
    set_size_request(70, visible_items_ * item_height_);

    set_margin_top(8);
    set_margin_bottom(8);
    set_margin_start(8);
    set_margin_end(8);
    set_name("picker-wheel");

    scrolled_window_.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    scrolled_window_.set_min_content_height(visible_items_ * item_height_);
    scrolled_window_.set_min_content_width(120);
    scrolled_window_.set_kinetic_scrolling(true);
    scrolled_window_.set_overlay_scrolling(false);
    pack_start(scrolled_window_);
    scrolled_window_.set_name("picker-wheel-scrolled");
    listbox_.set_name("picker-wheel-list");

    listbox_.set_selection_mode(Gtk::SELECTION_NONE);

    // Simple structure without infinite scrolling
    int pad = (visible_items_ - 1) / 2;

    // Add top padding
    for (int i = 0; i < pad; ++i) {
        auto dummy_row = Gtk::manage(new Gtk::ListBoxRow());
        auto dummy_label = Gtk::manage(new Gtk::Label(""));
        dummy_row->add(*dummy_label);
        listbox_.append(*dummy_row);
        row_labels_.push_back(dummy_label);
    }

    // Add actual items (no repetition)
    for (const auto& item_str : items_) {
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

    // Add bottom padding
    for (int i = 0; i < pad; ++i) {
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
    
    // Initialize separated behaviors
    scroll_behavior_ = std::make_unique<PickerScrollBehavior>(adjustment_, item_height_);
    selection_manager_ = std::make_unique<PickerSelectionManager>(row_labels_, item_height_);

    // Simple scroll handling without infinite scroll logic
    adjustment_->signal_value_changed().connect([this]() {
        selection_manager_->updateSelection(adjustment_->get_value());
    });

    scrolled_window_.signal_scroll_event().connect([this](GdkEventScroll* event) {
        return scroll_behavior_->handleScrollEvent(event);
    }, false);
    
    scrolled_window_.signal_button_press_event().connect([this](GdkEventButton* event) {
        return scroll_behavior_->handleButtonPress(event);
    }, false);
    
    scrolled_window_.signal_button_release_event().connect([this](GdkEventButton* event) {
        return scroll_behavior_->handleButtonRelease(event);
    }, false);
    
    scrolled_window_.signal_motion_notify_event().connect([this](GdkEventMotion* event) {
        return scroll_behavior_->handleMotionNotify(event);
    }, false);
    
    scrolled_window_.add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK);

    // Set initial position to first item
    int pad_offset = (visible_items_ - 1) / 2;
    adjustment_->set_value(pad_offset * item_height_);
    selection_manager_->updateSelection(adjustment_->get_value());
}

Glib::ustring PickerWheel::get_selected_text() const {
    return selection_manager_->getSelectedText();
}

int PickerWheel::get_selected_index() const {
    int idx = selection_manager_->getSelectedIndex() - (visible_items_ - 1) / 2;
    if (idx < 0 || idx >= (int)items_.size()) return -1;
    return idx;
}

const std::vector<std::string>& PickerWheel::get_items() const {
    return items_;
}

size_t PickerWheel::get_items_size() const {
    return items_.size();
}

sigc::signal<void, int> PickerWheel::signal_selection_changed() {
    return selection_manager_->signalSelectionChanged();
}

Glib::RefPtr<Gtk::Adjustment> PickerWheel::get_adjustment() {
    return adjustment_;
}

PickerComposite::PickerComposite(const Glib::ustring& title,
                const std::vector<std::string>& col1,
                const std::vector<std::string>& col2,
                const std::vector<std::string>& col3,
                const Glib::ustring& button_label,
                Gtk::Label* display_label,
                bool is_time_picker,
                bool is_date_picker)
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL, 4)
{
    set_size_request(230, 212);
    get_style_context()->add_class("picker-composite");
   
    set_border_width(4);
    set_name("picker-composite");

    // Initialize action handler with dependencies
    auto time_manager = std::make_unique<SystemTimeManager>();
    auto date_manager = std::make_unique<SystemDateManager>();
    auto display_updater = display_label ? 
        std::make_unique<LabelDisplayUpdater>(display_label) : nullptr;

    action_handler_ = std::make_unique<PickerActionHandler>(
        std::move(time_manager),
        std::move(date_manager),
        std::move(display_updater),
        is_time_picker,
        is_date_picker
    );

    auto titleLabel = Gtk::manage(new Gtk::Label(title));
    titleLabel->set_halign(Gtk::ALIGN_CENTER);
    titleLabel->set_margin_bottom(2);
    titleLabel->set_size_request(-1, 20);
    pack_start(*titleLabel, Gtk::PACK_SHRINK);

    auto hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 4));
    pack_start(*hbox, Gtk::PACK_EXPAND_WIDGET);

    picker1_ = new PickerWheel(col1);
    picker2_ = new PickerWheel(col2);
    picker3_ = new PickerWheel(col3);

    hbox->pack_start(*picker1_, Gtk::PACK_EXPAND_WIDGET);
    hbox->pack_start(*picker2_, Gtk::PACK_EXPAND_WIDGET);
    hbox->pack_start(*picker3_, Gtk::PACK_EXPAND_WIDGET);

    picker1_->signal_selection_changed().connect([this](int) { onSelectionChanged(); });
    picker2_->signal_selection_changed().connect([this](int) { onSelectionChanged(); });
    picker3_->signal_selection_changed().connect([this](int) { onSelectionChanged(); });

    auto buttonBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 0));
    pack_start(*buttonBox, Gtk::PACK_SHRINK);

    auto cancelButton = Gtk::manage(new Gtk::Button("Cancel"));
    cancelButton->set_size_request(116.47, 35.93);
    buttonBox->pack_start(*cancelButton, Gtk::PACK_EXPAND_WIDGET);
    cancelButton->get_style_context()->add_class("cancel-button");

    auto actionButton = Gtk::manage(new Gtk::Button(button_label));
    actionButton->set_size_request(113.53, 35.93);
    buttonBox->pack_start(*actionButton, Gtk::PACK_EXPAND_WIDGET);
    actionButton->get_style_context()->add_class("action-button");

    cancelButton->signal_clicked().connect([this]() {
        set_all_active(0);
    });

    actionButton->signal_clicked().connect([this, button_label]() {
        std::string text1 = picker1_->get_selected_text();
        std::string text2 = picker2_->get_selected_text();
        std::string text3 = picker3_->get_selected_text();
        
        action_handler_->handleAction(text1, text2, text3, button_label);
    });

    show_all_children();
    onSelectionChanged(); // Initial display update
}

void PickerComposite::onSelectionChanged() {
    std::string text1 = picker1_->get_selected_text();
    std::string text2 = picker2_->get_selected_text();
    std::string text3 = picker3_->get_selected_text();
    
    action_handler_->updateDisplay(text1, text2, text3);
}

void PickerComposite::set_all_active(int index) {
    scroll_to_index(picker1_, index);
    scroll_to_index(picker2_, index);
    scroll_to_index(picker3_, index);
}

void PickerComposite::scroll_to_index(PickerWheel* picker, int index) {
    if (index < 0) index = 0;
    
    // Calculate max index based on actual items using public getter method
    int pad = (3 - 1) / 2; // assuming visible_items = 3
    int max_index = static_cast<int>(picker->get_items_size()) - 1;
    index = std::min(index, max_index);
    
    double pos = (pad + index) * 20;
    picker->get_adjustment()->set_value(pos);
}
