#include "EnhancedProgressBar.h"

EnhancedProgressBar::EnhancedProgressBar(QWidget* parent) : QProgressBar(parent)
{}

EnhancedProgressBar::EnhancedProgressBar(Qt::Orientation orientation, QWidget* parent) : QProgressBar(parent)
{
    QProgressBar::setOrientation(orientation);
}

void EnhancedProgressBar::configure(float max_tick_count, float multiplier, std::function<void()> finished_callback)
{
    current_progress = 0.0f;
    progress_interval = enhancedMaximum() / max_tick_count / multiplier;
    finished_callback_function = std::move(finished_callback);
    setVisible(true);
    setValue(minimum());
}

void EnhancedProgressBar::setMinimum(float minimum)
{
    minimum_f = minimum;
}

void EnhancedProgressBar::setMaximum(float maximum)
{
    maximum_f = maximum;
}

void EnhancedProgressBar::setRange(float minimum, float maximum)
{
    QProgressBar::setRange(minimum, maximum);
    minimum_f = minimum;
    maximum_f = maximum;
}

float EnhancedProgressBar::enhancedMinimum()
{
    return minimum_f;
}

float EnhancedProgressBar::enhancedMaximum()
{
    return maximum_f;
}

void EnhancedProgressBar::updateProgressBar(float multiplier)
{
    if (isVisible() == false) {
        qWarning() << "Enhanced Progress Bar failed to update because \"configure()\" has to be called first.\n" \
            "Misuse of the \"multiplier\" parameter may also cause issues. Current progress (" << current_progress << ") reset.";
        current_progress = 0.0f;
        setValue(minimum());
    }
    else {
        current_progress += progress_interval * multiplier;
        //qDebug() << "Current Progress: " << current_progress;
        setValue(current_progress);
        if (current_progress > enhancedMaximum() - margin_of_error) {
            setVisible(false);
            qDebug() << "Progress Bar Finished: " << current_progress;
            if (finished_callback_function != 0) {
                finished_callback_function();
                finished_callback_function = 0;
            }
        }
    }
}