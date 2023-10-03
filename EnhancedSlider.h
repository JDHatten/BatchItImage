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
    const enum TextTipPlacement { ToTheLeft, ToTheRight };
    /// <summary>
    /// Sub class of QSlider that allows double/decimal values as well as show status and tool tip values on change.
    /// Status and tool tip values can be numbers (default) or text in-place of a range of values or both.
    /// </summary>
    /// <param name="parent"></param>
    explicit EnhancedSlider(QWidget* parent = 0);
    /// <summary>
    /// Sub class of QSlider that allows double/decimal values as well as show status and tool tip values on change.
    /// Status and tool tip values can be numbers (default) or text in-place of a range of values or both.
    /// </summary>
    /// <param name="orientation">--Orientation::Horizontal or Orientation::Vertical.</param>
    /// <param name="parent"></param>
    explicit EnhancedSlider(Qt::Orientation orientation, QWidget* parent = 0);
    /// <summary>
    /// Return the double value, but only if the range of values has been set as a double, else the integer value 
    /// will be returned as a double.
    /// </summary>
    /// <returns></returns>
    double valueDouble();
    /// <summary>
    /// Return the double minimum value, but only if the range of values has been set as a double, else the integer 
    /// minimum value will be returned as a double.
    /// </summary>
    /// <param name=""></param>
    double minimumDouble();
    /// <summary>
    /// Return the double maximum value, but only if the range of values has been set as a double, else the integer 
    /// maximum value will be returned as a double.
    /// </summary>
    /// <returns></returns>
    double maximumDouble();
    /// <summary>
    /// Return the double single step value, but only if it has been set as a double, else the integer single step 
    /// value will be returned as a double.
    /// </summary>
    double singleStepDouble();
    /// <summary>
    /// Set the single step (interval) value.
    /// </summary>
    /// <param name="step">--Integer</param>
    void setSingleStep(int step);
    /// <summary>
    /// Set the single step (interval) value.
    /// </summary>
    /// <param name="step">--Double</param>
    void setSingleStep(double step);
    /// <summary>
    /// Add text to the status and tool tip, in-place of a range of values.
    /// </summary>
    /// <param name="range_start">--Starting range value.</param>
    /// <param name="range_end">--Ending range value.</param>
    /// <param name="display_text">--A string that will show in-place of a range of values.</param>
    /// <param name="include_value">--If true the value will be include with the text string.</param>
    /// <param name="text_tip_placement">--Place value TextTipPlacement::ToTheLeft or TextTipPlacement::ToTheRight of text tip.</param>
    void addTextTip(int range_start, int range_end, QString display_text, bool include_value, bool text_tip_placement = ToTheLeft);
    /// <summary>
    /// Add text to the status and tool tip, in-place of a range of values.
    /// </summary>
    /// <param name="range_start">--Starting range (double) value.</param>
    /// <param name="range_end">--Ending range (double) value.</param>
    /// <param name="display_text">--A string that will show in-place of a range of values.</param>
    /// <param name="include_value">--If true the value will be include with the text string.</param>
    /// <param name="text_tip_placement">--Place value TextTipPlacement::ToTheLeft or TextTipPlacement::ToTheRight of text tip.</param>
    void addTextTip(double range_start, double range_end, QString display_text, bool include_value, bool text_tip_placement = ToTheLeft);
    /// <summary>
    /// Clear all text tips and go back to showing only the current value in the status and tool tip.
    /// </summary>
    void showValueOnly();
    /// <summary>
    /// Return whether single step interval integer values are being enforced or not, meaning no values in-between
    /// can be selected when enforced.
    /// </summary>
    /// <returns></returns>
    bool isSingleStepIntervalEnforced();
    /// <summary>
    /// Force selectable integer values to be a multiple of the single step interval, meaning no values in-between
    /// can be selected. For example, if the minumum number is 1, and the single step is 2, then only odd numbers
    /// will be selectable. This is always enforced when double values are in use.
    /// </summary>
    /// <param name="force">--Toggle</param>
    void forceSingleStepInterval(bool force);

signals:
    void doubleValueChanged(double value);

public slots:
    /// <summary>
    /// Enable or disable EnhancedSlider with option to reset all setting to defaults.
    /// </summary>
    /// <param name="enable">--Toggle.</param>
    /// <param name="reset_defaults">--If true will reset all settings to defaults.</param>
    void setEnabled(bool enable, bool reset_defaults = false);
    /// <summary>
    /// Set the minimum selectable value.
    /// </summary>
    /// <param name="min">--Integer</param>
    void setMinimum(int min);
    /// <summary>
    /// Set the minimum selectable value.
    /// </summary>
    /// <param name="min">--Double</param>
    void setMinimum(double min);
    /// <summary>
    /// Set the maximum selectable value.
    /// </summary>
    /// <param name="max">--Integer</param>
    void setMaximum(int max);
    /// <summary>
    /// Set the maximum selectable value.
    /// </summary>
    /// <param name="max">--Double</param>
    void setMaximum(double max);
    /// <summary>
    /// Set the range of selectable values.
    /// </summary>
    /// <param name="min">--Minimum starting value.</param>
    /// <param name="max">--Maximum ending value.</param>
    /// <param name="clear_text_values">--If true will clear any text values shown in tooltips.</param>
    void setRange(int min, int max, bool clear_text_values = true);
    /// <summary>
    /// Set the range of selectable values.
    /// </summary>
    /// <param name="min">--Minimum starting (double) value.</param>
    /// <param name="max">--Maximum ending (double) value.</param>
    /// <param name="clear_text_values">--If true will clear any text values shown in tooltips.</param>
    void setRange(double min, double max, bool clear_text_values = true);
    /// <summary>
    /// Set the current value.
    /// </summary>
    /// <param name="value">--Integer</param>
    void setValue(int);
    /// <summary>
    /// Set the current value.
    /// </summary>
    /// <param name="value">--Double</param>
    void setValue(double);

private slots:
    /// <summary>
    /// On value change make calculations that "Enhance" the QSlider, which include (if enabled):
    /// -Converting the normal QSlider's integer value into a double value.
    /// -Enforcing interval values, skipping values in-between.
    /// -Adding current value and text tip (within range) to the status and tool tip.
    /// </summary>
    void notifyValueChanged(int value);

private:
    struct NumbersToText {
        std::variant<int, double> range_start;
        std::variant<int, double> range_end;
        QString display_text;
        bool include_value;
        bool text_tip_placement;
    };
    bool show_text = false;
    bool text_tip_placement = ToTheLeft;
    std::vector<NumbersToText> number_to_text_list;
    bool force_interval = false;
    bool use_double_values = false;
    double current_value_double = QSlider::value();
    double minimum_double = QSlider::minimum();
    double maximum_double = QSlider::maximum();
    double single_step_double = QSlider::singleStep();
    uint round_step_multiplier = 1;
    bool mismatch = false;
    /// <summary>
    /// Set the range of integer steps to equal the range of double steps. Necessary for double values to work.
    /// </summary>
    void matchIntegerAndDoubleSteps();

protected:
    virtual void sliderChange(SliderChange change);
};

#endif // ENHANCEDSLIDER_H