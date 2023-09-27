#include "EnhancedSlider.h"

/// <summary>
/// Sub class of QSlider that shows tooltip values on change. Tooltip values can be numbers by default or
/// text in-place of a range of values.
/// </summary>
/// <param name="parent"></param>
EnhancedSlider::EnhancedSlider(QWidget* parent) : QSlider(parent)
{}

/// <summary>
/// Sub class of QSlider that shows tooltip values on change. Tooltip values can be numbers by default or
/// text in-place of a range of values.
/// </summary>
/// <param name="orientation">--Horizontal or Vertical.</param>
/// <param name="parent"></param>
EnhancedSlider::EnhancedSlider(Qt::Orientation orientation, QWidget* parent) : QSlider(orientation, parent)
{}

/// <summary>
/// Add new text to tooltip in-place of a range of values.
/// </summary>
/// <param name="range_start">--Starting range value.</param>
/// <param name="range_end">--Ending range value.</param>
/// <param name="display_text">--A string that will show in place of a range of values.</param>
/// <param name="include_number">--If true the value will be include with the text string.</param>
void EnhancedSlider::addTextValue(int range_start, int range_end, QString display_text, bool include_number)
{
    show_text = true;
    number_to_text_list.push_back({ range_start, range_end, display_text, include_number });
}

/// <summary>
/// Clear all text values and go back to showing only a number in the tooltip.
/// </summary>
void EnhancedSlider::showNumbersOnly()
{
    show_text = false;
    number_to_text_list.clear();
}

/// <summary>
/// Set range of values used in slider.
/// </summary>
/// <param name="min">--Minimum starting value.</param>
/// <param name="max">--Maximum ending value.</param>
/// <param name="clear_text_values">--If true will clear any text values shown in tooltips.</param>
void EnhancedSlider::setRange(int min, int max, bool clear_text_values)
{
    QSlider::setRange(min, max);
    if (clear_text_values)
        showNumbersOnly();
}

void EnhancedSlider::sliderChange(QAbstractSlider::SliderChange change)
{
    QSlider::sliderChange(change);

    if (change == QAbstractSlider::SliderValueChange) {
        QStyleOptionSlider style_opttion;
        initStyleOption(&style_opttion);
        QRect sc_rect = style()->subControlRect(QStyle::CC_Slider, &style_opttion, QStyle::SC_SliderHandle, this);
        QPoint bottomRightCorner = sc_rect.bottomLeft();
        int number = value();
        QString tool_tip_text = "";

        if (show_text) {
            const auto it = std::find_if(number_to_text_list.begin(), number_to_text_list.end(),
                [number](const NumbersToText& nt) { return (nt.range_start <= number and nt.range_end >= number); });
            if (it != number_to_text_list.end()) {
                if (it->include_number)
                    //tool_tip_text = it->display_text + " : " + QString::number(number);
                    tool_tip_text = QString::number(number) + " : " + it->display_text;
                else
                    tool_tip_text = it->display_text;
            }
            else {
                tool_tip_text = QString::number(number);
            }
        }
        else {
            tool_tip_text = QString::number(number);
        }
        QToolTip::showText(mapToGlobal(QPoint(bottomRightCorner.x(), bottomRightCorner.y())), tool_tip_text, this);
    }
}