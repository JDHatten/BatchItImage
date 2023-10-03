#include "EnhancedSlider.h"

EnhancedSlider::EnhancedSlider(QWidget* parent) : QSlider(parent)
{
    setEnabled(QSlider::isEnabled(), true);
}

EnhancedSlider::EnhancedSlider(Qt::Orientation orientation, QWidget* parent) : QSlider(orientation, parent)
{
    setEnabled(QSlider::isEnabled(), true);
}

void EnhancedSlider::setEnabled(bool enable, bool reset_defaults)
{
    QSlider::setEnabled(enable);
    if (reset_defaults) {
        setRange(0, 100, true);
        QSlider::setValue(1);
        QSlider::setSingleStep(1);
        force_interval = false;
        current_value_double = QSlider::value();
        minimum_double = QSlider::minimum();
        maximum_double = QSlider::maximum();
        single_step_double = QSlider::singleStep();
        Q_ASSERT(connect(this, SIGNAL(valueChanged(int)), this, SLOT(notifyValueChanged(int))));
        if (enable) {
            setToolTip(QString::number(value()));
            setStatusTip(QString::number(value()));
        }
        else {
            setToolTip("");
            setStatusTip("");
        }
    }
}

void EnhancedSlider::setValue(int value)
{
    use_double_values = false;
    QSlider::setValue(value);
    current_value_double = value;
}

void EnhancedSlider::setValue(double value)
{
    use_double_values = true;
    current_value_double = value;
    emit doubleValueChanged(value);
}

double EnhancedSlider::valueDouble()
{
    return current_value_double;
}

void EnhancedSlider::setSingleStep(int step)
{
    use_double_values = false;
    QSlider::setSingleStep(step);
}

void EnhancedSlider::setSingleStep(double step)
{
    use_double_values = true;
    single_step_double = step;

    // Get rounding multiplier so each step is within a desirable decimal precision.
    std::string step_str = std::to_string(step);
    step_str.erase(step_str.find_last_not_of('0') + 1, std::string::npos);
    int decimal_places = step_str.length() - step_str.find(".") - 1;
    round_step_multiplier = pow(10, decimal_places);
    round_step_multiplier = round_step_multiplier ? round_step_multiplier : 1;

    matchIntegerAndDoubleSteps();
}

double EnhancedSlider::singleStepDouble()
{
    return single_step_double;
}

double EnhancedSlider::minimumDouble()
{
    return minimum_double;
}

void EnhancedSlider::setMinimum(int min)
{
    use_double_values = false;
    minimum_double = min;
    QSlider::setMinimum(min);
}

void EnhancedSlider::setMinimum(double min)
{
    use_double_values = true;
    minimum_double = min;
}

double EnhancedSlider::maximumDouble()
{
    return maximum_double;
}

void EnhancedSlider::setMaximum(int max)
{
    use_double_values = false;
    maximum_double = max;
    QSlider::setMaximum(max);
}

void EnhancedSlider::setMaximum(double max)
{
    use_double_values = true;
    maximum_double = max;
}

void EnhancedSlider::setRange(int min, int max, bool clear_text_values)
{
    QSlider::setRange(min, max);
    use_double_values = false;
    if (clear_text_values)
        showValueOnly();
}

void EnhancedSlider::setRange(double min, double max, bool clear_text_values)
{
    use_double_values = true;
    minimum_double = min;
    maximum_double = max;
    matchIntegerAndDoubleSteps();
    if (clear_text_values)
        showValueOnly();
}

void EnhancedSlider::addTextTip(int range_start, int range_end, QString display_text, bool include_value, bool text_tip_placement)
{
    show_text = true;
    number_to_text_list.push_back({ range_start, range_end, display_text, include_value, text_tip_placement });
}

void EnhancedSlider::addTextTip(double range_start, double range_end, QString display_text, bool include_value, bool text_tip_placement)
{
    show_text = true;
    number_to_text_list.push_back({ range_start, range_end, display_text, include_value, text_tip_placement });
}

void EnhancedSlider::showValueOnly()
{
    show_text = false;
    number_to_text_list.clear();
}

bool EnhancedSlider::isSingleStepIntervalEnforced()
{
    return force_interval;
}

void EnhancedSlider::forceSingleStepInterval(bool force)
{
    if (use_double_values)
        qInfo() << this << "has no need to set \"forceSingleStepInterval\" when range values are doubles, as it's automatically enforced.";
    force_interval = force;
}

void EnhancedSlider::notifyValueChanged(int value)
{
    if (use_double_values) {
        double double_value = double(value) / (double(QSlider::maximum()) / (maximumDouble() - minimumDouble())) + minimumDouble();
        double_value = round(double_value * round_step_multiplier) / round_step_multiplier;
        setValue(double_value);
    }
    auto number = use_double_values ? valueDouble() : value;
    //qDebug() << number;

    if (force_interval and not use_double_values) {
        int off_set = (int(number) - minimum()) % singleStep();
        if (off_set != 0) {
            setValue(int(number) + off_set);
            return;
        }
    }
    QStyleOptionSlider style_option;
    initStyleOption(&style_option);
    QRect sc_rect = style()->subControlRect(QStyle::CC_Slider, &style_option, QStyle::SC_SliderHandle, this);
    QPoint bottomRightCorner = sc_rect.bottomLeft();
    QString tool_tip_text = "";

    if (show_text) {
        std::vector<NumbersToText>::iterator it;
        if (use_double_values)
            it = std::find_if(number_to_text_list.begin(), number_to_text_list.end(),
                [number](const NumbersToText& nt) {
                    return (std::get<double>(nt.range_start) <= number and std::get<double>(nt.range_end) >= number);
                });
        else
            it = std::find_if(number_to_text_list.begin(), number_to_text_list.end(),
                [number](const NumbersToText& nt) {
                    return (std::get<int>(nt.range_start) <= number and std::get<int>(nt.range_end) >= number);
                });
        if (it != number_to_text_list.end())
            if (it->include_value)
                if (it->text_tip_placement == ToTheRight)
                    tool_tip_text = QString::number(number) + it->display_text;
                else
                    tool_tip_text = it->display_text + QString::number(number);
            else
                tool_tip_text = it->display_text;
        else
            tool_tip_text = QString::number(number);
    }
    else
        tool_tip_text = QString::number(number);
    
    QToolTip::showText(mapToGlobal(QPoint(bottomRightCorner.x() + 10, bottomRightCorner.y())), tool_tip_text, this);
    setToolTip(tool_tip_text);
    setStatusTip(tool_tip_text);
}

void EnhancedSlider::matchIntegerAndDoubleSteps()
{
    // TODO: Force the double step values to evenly add up to the range?
    double step_range = (maximumDouble() - minimumDouble()) / singleStepDouble();
    double upper_step_range = round(step_range) + 0.0001;
    double lower_step_range = round(step_range) - 0.0001;
    if (step_range > upper_step_range or step_range < lower_step_range) {
        qWarning() << "Warning:" << this << ") has a single step double value that does not evenly add up to the minimum and maximum double values.";
        mismatch = true;
    }
    else if (mismatch) {
        qInfo() << "-Ignore Warning for" << this << ", the mismatched values have been fixed.";
        mismatch = false;
    }
    QSlider::setRange(0, int(step_range));
    QSlider::setSingleStep(1);
}

void EnhancedSlider::sliderChange(QAbstractSlider::SliderChange change)
{
    QSlider::sliderChange(change);
}