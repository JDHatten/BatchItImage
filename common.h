#pragma once

#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
//#include <thread>
#include <windows.h>

#include <QtWidgets/QMainWindow>
//#include <QtWidgets>
#include <QAbstractButton>
#include <QEvent>
#include <QFileDialog>
//#include <QFont>
//#include <QLabel>
#include <QMimeData>
#include <QMouseEvent>
#include <QSettings>
//#include <QString>
#include <QThread>
//#include <QTimer>

#ifdef _DEBUG
#define DEBUG(a) do { \
std::cout << a << std::endl;  \
} while (0)
#define DEBUG2(a,b) do { \
std::cout << a << b << std::endl;  \
} while (0)
#define DEBUG3(a,b,c) do { \
std::cout << a << b << c << std::endl;  \
} while (0)
#define DEBUG4(a,b,c,d) do { \
std::cout << a << b << c << d << std::endl;  \
} while (0)
#define DEBUG_ERR(x) do { \
std::cerr << x << std::endl;  \
} while (0)
#else
#define DEBUG(a)
#define DEBUG2(a,b)
#define DEBUG3(a,b,c)
#define DEBUG4(a,b,c,d)
#define DEBUG_ERR(x)
#endif

template<typename T>
void DebugPrintList(const std::vector<T>&list, std::string name = "")
{
#ifdef DEBUG
    DEBUG(name);
    for (int i = 0; i < list.size(); i++) {
        DEBUG(list.at(i).to_string());
    }
#endif // DEBUG
}

struct FileMetadata {
    int load_order;
    std::string path;
    long size;
    int width;
    int height;
    time_t date_created;
    time_t date_modified;
    bool selected;

    std::string to_string() const {
        return
            "{\n  Load Order: " + std::to_string(load_order) +
            "\n  Path:       " + path +
            "\n  Size:       " + std::to_string(size) +
            "\n  Width:      " + std::to_string(width) +
            "\n  Height:     " + std::to_string(height) +
            "\n  Created:    " + std::to_string(date_created) +
            "\n  Modified:   " + std::to_string(date_modified) +
            "\n  Selected:   " + std::to_string(selected) +
            "\n}";
    }
};

struct Preset {
    int index; // Use for sorting?
    QString description;
    int width_change_selection;
    int height_change_selection;
    int width_number;
    int height_number;
    bool keep_aspect_ratio;
    int resampling_filter;
    int rotation_angle;
    bool format_change;
    int format_extension;
    int format_subsampling;
    bool format_optimize;
    bool format_progressive;
    int format_quality;
    int format_compression;
    int format_extra1;
    int format_extra2;
    int save_file_policy_option;
    std::string save_file_name_change;
    bool relative_save_path;
    std::string save_file_path_change;

    Preset() : // Initialize Defaults
        index(0),
        description(""),
        width_change_selection(0),
        height_change_selection(0),
        width_number(0),
        height_number(0),
        keep_aspect_ratio(true),
        resampling_filter(0),
        rotation_angle(0),
        format_change(false),
        format_extension(0), // Format: JPEG
        format_subsampling(1),
        format_optimize(false),
        format_progressive(false),
        format_quality(95),
        format_compression(0),
        format_extra1(-1),
        format_extra2(-1),
        save_file_policy_option(2), // NEW_NAME
        save_file_name_change("<FILE_NAME>__new"),
        relative_save_path(true),
        save_file_path_change("") {}

    std::string to_string() const {
        return
           "{\n  Preset Index:              " + std::to_string(index) +
            "\n  Preset Description:        " + description.toStdString() +
            "\n  Name Change:               " + save_file_name_change +
            "\n  Width Change Selection:    " + std::to_string(width_change_selection) +
            "\n  Width Change:              " + std::to_string(width_number) +
            "\n  Height Change Selection:   " + std::to_string(height_change_selection) +
            "\n  Height Change:             " + std::to_string(height_number) +
            "\n  Keep Aspect Ratio:         " + (keep_aspect_ratio ? "True" : "False") +
            "\n  Rotation:                  " + std::to_string(rotation_angle) +
            "\n  Format Extension:          " + std::to_string(format_extension) +
            "\n  Save File Policy Change:   " + std::to_string(save_file_policy_option) +
            "\n  Save File Name Change:     " + save_file_name_change +
            "\n  Keep Aspect Ratio:         " + (relative_save_path ? "True" : "False") +
            "\n  Save File Path:            " + save_file_path_change +
            "\n}";
    }
};

// UI Data to be added to an object/widget
struct UIData { 
    std::variant<int, std::string> data; // A Value, Default, Type, Index, etc.
    std::string name; // An object's title, name, or other displayed text.
    std::string desc; // Description text used in a Tooltip or Statusbar, etc.
};
