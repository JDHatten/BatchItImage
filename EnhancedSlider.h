#pragma once
#ifndef ENHANCEDSLIDER_H
#define ENHANCEDSLIDER_H

#include <QSlider>
#include <QStyleOptionSlider>
#include <QToolTip>

class EnhancedSlider : public QSlider
{
    Q_OBJECT

public:
    explicit EnhancedSlider(QWidget* parent = 0);
    explicit EnhancedSlider(Qt::Orientation orientation, QWidget* parent = 0);
    void addTextValue(int range_start, int range_end, QString display_text, bool include_number);
    void showNumbersOnly();

public slots:
    void setRange(int min, int max, bool clear_text_values = true);

private:
    struct NumbersToText {
        int range_start;
        int range_end;
        QString display_text;
        bool include_number;
    };
    bool show_text = false;
    std::vector<NumbersToText> number_to_text_list;

protected:
    virtual void sliderChange(SliderChange change);
};

#endif // ENHANCEDSLIDER_H