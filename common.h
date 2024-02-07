#pragma once

#include <algorithm>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#ifndef OPENCV
#define OPENCV
#include <opencv2/opencv.hpp>
#endif
#include <string>
//#include <thread>
#include <windows.h>

#include <QtWidgets/QMainWindow>
//#include <QtWidgets>
#include <QAbstractButton>
#include <QColorDialog>
#include <QEvent>
#include <QFileDialog>
//#include <QFont>
//#include <QLabel>
#include <QMimeData>
#include <QMouseEvent>
#include <QScroller>
#include <QSettings>
//#include <QString>
#include <QThread>

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
#ifdef _DEBUG
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
