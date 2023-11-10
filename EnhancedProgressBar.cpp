#include "EnhancedProgressBar.h"

EnhancedProgressBar::EnhancedProgressBar(QWidget* parent) : QProgressBar(parent)
{}

EnhancedProgressBar::EnhancedProgressBar(Qt::Orientation orientation, QWidget* parent) : QProgressBar(parent)
{
    QProgressBar::setOrientation(orientation);
}

void EnhancedProgressBar::setParentContainer(QWidget* parent_container)
{
    EnhancedProgressBar::parent_container = parent_container;
    EnhancedProgressBar::parent_container->setVisible(false);
}

void EnhancedProgressBar::setCancelButton(QPushButton* button)
{
    EnhancedProgressBar::cancel_button = button;
    EnhancedProgressBar::cancel_button->setVisible(false);
}

void EnhancedProgressBar::restartProgressBar(float max_tick_count, float multiplier, bool show_cancel_button, std::function<void()> finished_callback)
{
    EnhancedProgressBar::current_progress = 0.0f;
    EnhancedProgressBar::progress_interval = enhancedMaximum() / max_tick_count / multiplier;
    EnhancedProgressBar::show_cancel_button = show_cancel_button;
    EnhancedProgressBar::finished_callback_function = std::move(finished_callback);
    setVisible(true);
    setValue(minimum());
}
void EnhancedProgressBar::restartProgressBar(float max_tick_count, float multiplier, std::function<void()> finished_callback)
{
    restartProgressBar(max_tick_count, multiplier, false, std::move(finished_callback));
}
void EnhancedProgressBar::restartProgressBar(float max_tick_count, float multiplier)
{
    restartProgressBar(max_tick_count, multiplier, false, NULL);
}

void EnhancedProgressBar::setMinimum(float minimum)
{
    minimum_f = minimum;
}

void EnhancedProgressBar::setVisible(bool visible)
{
    if (parent_container)
        parent_container->setVisible(visible);
    if (cancel_button and show_cancel_button)
        cancel_button->setVisible(visible);
    QProgressBar::setVisible(visible);
}

bool EnhancedProgressBar::isVisible()
{
    if (parent_container)
        return parent_container->isVisible();
    else
        return QProgressBar::isVisible();
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
        qWarning() << "Enhanced Progress Bar failed to update because \"restartProgressBar()\" has to be called first.\n" \
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