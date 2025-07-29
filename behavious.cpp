#include "behaviors.h"
#include <cmath>
#include <algorithm>

PickerScrollBehavior::PickerScrollBehavior(Glib::RefPtr<Gtk::Adjustment> adjustment, int item_height)
    : adjustment_(adjustment), item_height_(item_height), is_dragging_(false),
      drag_start_y_(0), drag_start_value_(0) {}

bool PickerScrollBehavior::handleButtonPress(GdkEventButton* event) {
    is_dragging_ = true;
    drag_start_y_ = event->y;
    drag_start_value_ = adjustment_->get_value();
    
    if (scroll_timeout_.connected())
        scroll_timeout_.disconnect();
        
    return true;
}

bool PickerScrollBehavior::handleButtonRelease(GdkEventButton* event) {
    if (is_dragging_) {
        is_dragging_ = false;
        snapToNearest();
    }
    return true;
}

bool PickerScrollBehavior::handleMotionNotify(GdkEventMotion* event) {
    if (is_dragging_) {
        double delta_y = event->y - drag_start_y_;
        double new_value = drag_start_value_ - delta_y;
        
        adjustment_->set_value(new_value);
    }
    return true;
}

bool PickerScrollBehavior::handleScrollEvent(GdkEventScroll* event) {
    if (scroll_timeout_.connected())
        scroll_timeout_.disconnect();

    scroll_timeout_ = Glib::signal_timeout().connect([this]() {
        snapToNearest();
        scroll_timeout_.disconnect();
        return false;
    }, 150);

    return false;
}

void PickerScrollBehavior::snapToNearest() {
    double val = adjustment_->get_value();
    int nearest = static_cast<int>(std::round(val / item_height_));
    double target_val = nearest * item_height_;
    adjustment_->set_value(target_val);
}

PickerSelectionManager::PickerSelectionManager(const std::vector<Gtk::Label*>& labels, int item_height)
    : row_labels_(labels), selected_index_(-1), item_height_(item_height) {}

void PickerSelectionManager::updateSelection(double scroll_value) {
    double val = scroll_value;
    int middle_index = static_cast<int>(std::floor(val / item_height_)) + 1;
    middle_index = std::max(0, std::min(middle_index, (int)row_labels_.size() - 1));

    if (middle_index != selected_index_) {
        if (selected_index_ >= 0 && selected_index_ < (int)row_labels_.size()) {
            row_labels_[selected_index_]->override_color(Gdk::RGBA("#6b7280"));
        }

        selected_index_ = middle_index;
        Glib::ustring text = row_labels_[selected_index_]->get_text();
        if (!text.empty()) {
            row_labels_[selected_index_]->override_color(Gdk::RGBA("#0C99A3"));
            selection_changed_signal_.emit(selected_index_);
        }
    }
}

Glib::ustring PickerSelectionManager::getSelectedText() const {
    if (selected_index_ >= 0 && selected_index_ < (int)row_labels_.size())
        return row_labels_[selected_index_]->get_text();
    return {};
}

int PickerSelectionManager::getSelectedIndex() const {
    return selected_index_;
}

sigc::signal<void, int> PickerSelectionManager::signalSelectionChanged() {
    return selection_changed_signal_;
}
