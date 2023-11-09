#pragma once
#ifndef ENHANCEDPROGRESSBAR_H
#define ENHANCEDPROGRESSBAR_H

#include <QApplication>
#include <QProgressBar>
#include <QPushButton>

class EnhancedProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    /// <summary>
    /// Sub class of QProgressBar that once configured, will automatically calculate ticks in every Slot updateProgressBar() called with
    /// an option to use a multiplier (to skip/speed up progress) and an option to add a callback function once maximum progress reached.
    /// </summary>
    /// <param name="parent">--Parent QWidget</param>
    explicit EnhancedProgressBar(QWidget* parent = 0);
    /// <summary>
    /// Sub class of QProgressBar that once configured, will automatically calculate ticks in every Slot updateProgressBar() called with
    /// an option to use a multiplier (to skip/speed up progress) and an option to add a callback function once all progress completed.
    /// </summary>
    /// <param name="orientation">--Orientation::Horizontal or Orientation::Vertical.</param>
    /// <param name="parent">--Parent QWidget</param>
    explicit EnhancedProgressBar(Qt::Orientation orientation, QWidget* parent = 0);
    /// <summary>
    /// If the enhanced progress bar is placed inside a container that need it's visibility toggled, add it here.
    /// </summary>
    void setParentContainer(QWidget* parent_container);
    /// <summary>
    /// If the enhanced progress bar has a cancel button that need it's visibility toggled, add it here.
    /// </summary>
    void setCancelButton(QPushButton* button);
    /// <summary>
    /// Start (makes visible) a new progress bar process by setting up the amount of ticks to maximum progress.
    /// Note this enhanced progress bar needs to be restarted every time maximum progress previously reached.
    /// </summary>
    /// <param name="max_tick_count">--The amount of ticks to maximum progress. Must be called before starting update progress.</param>
    /// <param name="multiplier">--Set how many times updateProgressBar() must be called to complete one tick. [Default: 1.0]</param>
    /// <param name="show_cancel_button">--If set (setCancelButton), show or hide the cancel button.</param>
    /// <param name="finished_callback">--An optional function to call when maximum progress/ticks reached; example: std::bind(class::function, this).</param>
    void restartProgressBar(float max_ticks, float multiplier = 1.0f, bool show_cancel_button = false, std::function<void()> finished_callback = NULL);
    void restartProgressBar(float max_ticks, float multiplier = 1.0f, std::function<void()> finished_callback = NULL);
    void restartProgressBar(float max_ticks, float multiplier = 1.0f);
    /// <summary>
    /// Return enhanced progress bar's minimum float value.
    /// </summary>
    /// <returns></returns>
    float enhancedMinimum();
    /// <summary>
    /// Return enhanced progress bar's maximum float value.
    /// </summary>
    /// <returns></returns>
    float enhancedMaximum();

public slots:
    /// <summary>
    /// Set the enhanced progress bar's minimum float value.
    /// </summary>
    /// <param name="minimum">--Float</param>
    void setMinimum(float minimum);
    /// <summary>
    /// Set the enhanced progress bar's maximum float value.
    /// </summary>
    /// <param name="maximum">--Float</param>
    void setMaximum(float maximum);
    /// <summary>
    /// Set the enhanced progress bar's minimum and maximum float values.
    /// </summary>
    /// <param name="minimum">--Float</param>
    /// <param name="maximum">--Float</param>
    void setRange(float min, float max);
    /// <summary>
    /// Set visibility of enhanced progress bar and if set, it container and cancel button.
    /// </summary>
    void setVisible(bool visible);
    /// <summary>
    /// Returns true if enhanced progress bar is visible, or if set, it's container is visible.
    /// </summary>
    bool isVisible();
    /// <summary>
    /// Update enhanced progress bar, hiding it when maximum progress reached. Note restartProgressBar() must have been previously
    /// called before updating can proceed.
    /// </summary>
    /// <param name="multiplier">--Updates to make per one tick, effectively updating "X" amount of times. [Default: 1.0]</param>
    void updateProgressBar(float multiplier = 1.0f);

private:
    float minimum_f = 0.0f;
    float maximum_f = 100.0f;
    float current_progress = 0.0f;
    float progress_interval = 0.0f;
    const float margin_of_error = 0.05f;
    QWidget* parent_container = nullptr;
    QPushButton* cancel_button = nullptr;
    bool show_cancel_button = false;
    std::function<void()> finished_callback_function = NULL;
};

#endif // ENHANCEDPROGRESSBAR_H