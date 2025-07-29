#pragma once
#include <gtkmm.h>
#include <vector>

class PickerScrollBehavior {
private:
    Glib::RefPtr<Gtk::Adjustment> adjustment_;
    int item_height_;
    bool is_dragging_;
    double drag_start_y_;
    double drag_start_value_;
    sigc::connection scroll_timeout_;

    void snapToNearest();

public:
    PickerScrollBehavior(Glib::RefPtr<Gtk::Adjustment> adjustment, int item_height);
    bool handleButtonPress(GdkEventButton* event);
    bool handleButtonRelease(GdkEventButton* event);
    bool handleMotionNotify(GdkEventMotion* event);
    bool handleScrollEvent(GdkEventScroll* event);
};

class PickerSelectionManager {
private:
    std::vector<Gtk::Label*> row_labels_;
    int selected_index_;
    int item_height_;
    sigc::signal<void, int> selection_changed_signal_;

public:
    PickerSelectionManager(const std::vector<Gtk::Label*>& labels, int item_height);
    void updateSelection(double scroll_value);
    Glib::ustring getSelectedText() const;
    int getSelectedIndex() const;
    sigc::signal<void, int> signalSelectionChanged();
};
