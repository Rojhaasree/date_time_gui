#include "layout.h"
#include "components.h"
#include "utils.h"

SystemContentLayout::SystemContentLayout() : Gtk::Box(Gtk::ORIENTATION_VERTICAL) {
    set_size_request(679, 427);
    setup_ui();
    set_visible(false);
}

void SystemContentLayout::show_layout() {
    set_visible(true);
}

void SystemContentLayout::hide_layout() {
    set_visible(false);
}

void SystemContentLayout::moveIndicatorTo(int target_x) {
    if (navBox_ && selectionIndicator_) {
        navBox_->move(*selectionIndicator_, target_x, 28);
    }
}

void SystemContentLayout::setup_ui() {
    auto header = Gtk::make_managed<HeaderButton>();
    pack_start(*header, Gtk::PACK_SHRINK);

    auto mainFixed = Gtk::make_managed<Gtk::Fixed>();
    mainFixed->set_size_request(679, 336);
    
    // Navigation labels
    navBox_ = Gtk::make_managed<Gtk::Fixed>();
    navBox_->set_size_request(679, 22);

    auto label_datetime = Gtk::make_managed<Gtk::Button>("Date and Time");
    label_datetime->set_size_request(112, 22);
    label_datetime->set_relief(Gtk::RELIEF_NONE);
    navBox_->put(*label_datetime, 66, 0); 

    auto label_memory = Gtk::make_managed<Gtk::Button>("Memory");
    label_memory->set_size_request(82, 22);
    label_memory->set_relief(Gtk::RELIEF_NONE);
    navBox_->put(*label_memory, 298, 0);

    auto label_network = Gtk::make_managed<Gtk::Button>("Network");
    label_network->set_size_request(92, 22);
    label_network->set_relief(Gtk::RELIEF_NONE);
    navBox_->put(*label_network, 521, 0);

    // Create selection indicator rectangle
    selectionIndicator_ = Gtk::make_managed<Gtk::EventBox>();
    selectionIndicator_->set_size_request(86, 7);
    selectionIndicator_->override_background_color(Gdk::RGBA("#0C99A3"), Gtk::STATE_FLAG_NORMAL);

    // Apply CSS styling
    auto indicatorCss = Gtk::CssProvider::create();
    indicatorCss->load_from_data(R"(
    #selection-indicator {
        background-color: #0C99A3;
        opacity: 1.0;
        border: none;
        background-image: none;
    }
    )");

    selectionIndicator_->set_name("selection-indicator");
    selectionIndicator_->get_style_context()->add_provider(indicatorCss, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Initially position under "Date and Time" tab
    navBox_->put(*selectionIndicator_, 79, 28);

    mainFixed->put(*navBox_, 0, 0);

    // Create Stack for switching between pages
    stackContainer_ = Gtk::make_managed<Gtk::Stack>();
    stackContainer_->set_size_request(679, 314);
    stackContainer_->set_transition_type(Gtk::STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    stackContainer_->set_transition_duration(300);
    mainFixed->put(*stackContainer_, 0, 22);

    // Initialize navigation manager
    navigation_manager_ = std::make_unique<NavigationManager>(stackContainer_);

    // Create pages with EventBox for background color
    auto dateTimeEventBox = Gtk::make_managed<Gtk::EventBox>();
    dateTimeEventBox->set_size_request(679, 314);
    dateTimeEventBox->override_background_color(Gdk::RGBA("#2C3435"), Gtk::STATE_FLAG_NORMAL);

    auto dateTimePage = Gtk::make_managed<Gtk::Fixed>();
    dateTimePage->set_size_request(679, 314);
    dateTimePage->set_name("picker-area");
    dateTimeEventBox->add(*dateTimePage);

    auto memoryEventBox = Gtk::make_managed<Gtk::EventBox>();
    memoryEventBox->set_size_request(679, 314);
    memoryEventBox->override_background_color(Gdk::RGBA("#2C3435"), Gtk::STATE_FLAG_NORMAL);

    auto memoryPage = Gtk::make_managed<Gtk::Fixed>();
    memoryPage->set_size_request(679, 314);
    memoryPage->set_name("memory-area");
    auto memoryLabel = Gtk::make_managed<Gtk::Label>("Memory Settings");
    memoryLabel->set_size_request(200, 50);
    memoryLabel->set_halign(Gtk::ALIGN_CENTER);
    memoryPage->put(*memoryLabel, 240, 130);
    memoryEventBox->add(*memoryPage);

    auto networkEventBox = Gtk::make_managed<Gtk::EventBox>();
    networkEventBox->set_size_request(679, 314);
    networkEventBox->override_background_color(Gdk::RGBA("#2C3435"), Gtk::STATE_FLAG_NORMAL);

    auto networkPage = Gtk::make_managed<Gtk::Fixed>();
    networkPage->set_size_request(679, 314);
    networkPage->set_name("network-area");
    auto networkLabel = Gtk::make_managed<Gtk::Label>("Network Settings");
    networkLabel->set_size_request(200, 50);
    networkLabel->set_halign(Gtk::ALIGN_CENTER);
    networkPage->put(*networkLabel, 240, 130);
    networkEventBox->add(*networkPage);

    // Add EventBoxes to stack
    stackContainer_->add(*dateTimeEventBox, "datetime", "Date and Time");
    stackContainer_->add(*memoryEventBox, "memory", "Memory");
    stackContainer_->add(*networkEventBox, "network", "Network");
    stackContainer_->set_visible_child("datetime");

    // Connect navigation buttons with indicator movement
    label_datetime->signal_clicked().connect([this]() {
        navigation_manager_->navigateTo("datetime");
        moveIndicatorTo(79);  // Position under Date and Time
    });

    label_memory->signal_clicked().connect([this]() {
        navigation_manager_->navigateTo("memory");
        moveIndicatorTo(310);  // Position under Memory
    });

    label_network->signal_clicked().connect([this]() {
        navigation_manager_->navigateTo("network");
        moveIndicatorTo(533);  // Position under Network
    });

    // Create DATE label widget
    auto customWidget = Gtk::make_managed<Gtk::Label>("DATE");
    customWidget->set_size_request(86, 34);
    customWidget->set_halign(Gtk::ALIGN_CENTER);
    customWidget->set_valign(Gtk::ALIGN_CENTER);
    customWidget->set_name("custom-widget");
    dateTimePage->put(*customWidget, 30, 64);

    // Create EventBox container for TIME
    auto timeContainer = Gtk::make_managed<Gtk::EventBox>();
    timeContainer->set_size_request(86, 34);
    timeContainer->override_background_color(Gdk::RGBA("#435051"), Gtk::STATE_FLAG_NORMAL);

    // Create label for TIME text
    auto timeLabel = Gtk::make_managed<Gtk::Label>("TIME");
    timeLabel->set_halign(Gtk::ALIGN_CENTER);
    timeLabel->set_valign(Gtk::ALIGN_CENTER);

    // Style the label text
    auto timeLabelCss = Gtk::CssProvider::create();
    timeLabelCss->load_from_data(R"(
    #time-label {
        color: white;
        font-weight: bold;
        font-size: 10px;
        background: transparent;
    }
    )");

    timeLabel->set_name("time-label");
    timeLabel->get_style_context()->add_provider(timeLabelCss, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Add label to container
    timeContainer->add(*timeLabel);

    // Apply border-radius to container
    auto timeContainerCss = Gtk::CssProvider::create();
    timeContainerCss->load_from_data(R"(
    #time-container {
        border-radius: 5px;
        border: none;
        opacity: 1.0;
    }
    )");

    timeContainer->set_name("time-container");
    timeContainer->get_style_context()->add_provider(timeContainerCss, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Position at specified coordinates
    dateTimePage->put(*timeContainer, 387, 64);

    // Main CSS provider with updated gradient for unselected menu buttons
    auto css_provider = Gtk::CssProvider::create();
    css_provider->load_from_data(R"(
    #main-window {
        background-color: #000000;
    }
    
    .display-box {
        background-color: #011415;
        color: #FDFFFF;
        border-radius: 5px;
        font-weight: bold;
        font-size: 12px;
    }

    button.action-button {
        background-image: none;
        background-color: #0C99A3;
        color: white;
        border: 0;
        border-radius: 0px;
        font-weight: bold;
    }

    button.cancel-button {
        background-image: none;
        background-color: #1F292A;
        border-radius: 0px;
        border: 0;
        color: white;
        font-weight: bold;
    }
    
    #custom-widget {
        background-color: #435051;
        border-radius: 5px;
        color: white;
        font-weight: bold;
        font-size: 10px;
        opacity: 1.0;
        border: none;
        background-image: none;
    }
    
    button {
        color: white;
        font-weight: bold;
        background: transparent;
        border: none;
    }

    button:hover {
        color: #E5E7EB;
    }
    
    .sidebar-box {
        background-color: #123235;
        border-radius: 5px;
    }

    button.nav-button {
        background-image: none;
        background-color: #123235;
        color: white;
        border-radius: 5px;
        font-weight: bold;
        font-size: 12px;
        border: none;
        margin: 1px;
    }

    button.nav-button:hover {
        background-color: #1A4A4F;
        transition: all 0.3s ease;
    }

    button.nav-button:active {
        background-color: #0F2A2D;
    }

    /* Modified gradient menu button styles */
    button.gradient-menu-button {
        /* Apply your specified gradient to unselected buttons */
        background-image: linear-gradient(270deg, #0C9BA6 0%, #274B4D 77.88%);
        background-color: transparent;
        color: white;
        border-radius: 0;
        font-weight: bold;
        font-size: 12px;
        border: none;
        margin: 0;
    }

    button.gradient-menu-button:hover {
        /* Keep the same gradient on hover, or modify as needed */
        background-image: linear-gradient(270deg, #0C9BA6 0%, #274B4D 77.88%);
        opacity: 0.8;
        transition: all 0.3s ease;
    }

    button.gradient-menu-button:active {
        background-image: linear-gradient(270deg, #0C9BA6 0%, #274B4D 77.88%);
        opacity: 0.6;
    }

    /* Add a selected state class for the active button */
    button.gradient-menu-button.selected {
        background-image: linear-gradient(to top, #6CE9F2, #136469);
        background-color: transparent;
    }

    button.gradient-menu-button.selected:hover {
        background-image: linear-gradient(to top, #136469, #6CE9F2);
    }

    .picker-composite {
        background-color: #011415;
        border-radius: 0;
        color: white;
        padding: 0;
    }

    #picker-wheel-scrolled {
        background-color: #011415;
    }

    #picker-wheel-list {
        background-color: #011415;
    }

    #picker-wheel-list row {
        background-color: #011415;
    }

    #picker-wheel-list label {
        color: white;
        font-weight: bold;
        font-size: 14px;
    }

    #picker-area, #memory-area, #network-area {
        background-color: #2C3435;
        background-image: none;
        border-radius: 8px;
        box-shadow: none;
    }

    GtkFixed#picker-area, GtkFixed#memory-area, GtkFixed#network-area {
        background-color: #2C3435;
        background-image: none;
    }

    GtkSeperator {
        background: none;
        border: none;
        margin: 0;
        padding: 0;
    }
    )");

    Gtk::StyleContext::add_provider_for_screen(
        Gdk::Screen::get_default(),
        css_provider,
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    auto dateDisplayBox = Gtk::make_managed<Gtk::Label>("01 JAN 2025");
    dateDisplayBox->set_size_request(150, 34);
    dateDisplayBox->set_halign(Gtk::ALIGN_CENTER);
    dateDisplayBox->set_valign(Gtk::ALIGN_CENTER);
    dateDisplayBox->set_justify(Gtk::JUSTIFY_CENTER);
    dateDisplayBox->get_style_context()->add_class("display-box");
    dateTimePage->put(*dateDisplayBox, 139, 64);

    auto timeDisplayBox = Gtk::make_managed<Gtk::Label>("12:00 AM");
    timeDisplayBox->set_size_request(150, 34);
    timeDisplayBox->set_halign(Gtk::ALIGN_CENTER);
    timeDisplayBox->set_valign(Gtk::ALIGN_CENTER);
    timeDisplayBox->set_justify(Gtk::JUSTIFY_CENTER);
    timeDisplayBox->get_style_context()->add_class("display-box");
    timeDisplayBox->get_style_context()->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    dateTimePage->put(*timeDisplayBox, 497, 65);

    auto datePicker = new PickerComposite(
        "Set Date",
        generateNumbers(1, 31),
        { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" },
        generateNumbers(1980, 2025),
        "Edit",
        dateDisplayBox,
        false,
        true
    );
    datePicker->set_size_request(230, 212);
    dateTimePage->put(*datePicker, 59, 111);

    auto timePicker = new PickerComposite(
        "Set Time",
        generateNumbers(1, 12),
        generateNumbers(0, 59),
        { "AM", "PM", "AM", "PM", "AM", "PM" },
        "Set",
        timeDisplayBox,
        true,
        false
    );
    timePicker->set_size_request(230, 212);
    dateTimePage->put(*timePicker, 417, 111);

    pack_start(*mainFixed, Gtk::PACK_EXPAND_WIDGET);
    show_all_children();
}
