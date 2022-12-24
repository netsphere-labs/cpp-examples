#ifndef TRAFFICLIGHTWIDGET_H
#define TRAFFICLIGHTWIDGET_H

#include <QWidget>
#include <QAbstractButton>

// 一つのライト
class LightWidget: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE switchLight)

public:
    LightWidget(const QString& imagePath, QWidget *parent = nullptr);

    bool isOn() const { return m_on; }
    //void setOn(bool on);

public slots:
    void switchLight(bool onoff);

protected:
    virtual void paintEvent(QPaintEvent *) override;
    virtual QSize sizeHint() const override;

private:
    QImage m_image;
    bool m_on;
};


class RunToggleButtonWidget : public QAbstractButton
{
    Q_OBJECT
public:
    RunToggleButtonWidget(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *) override;
    virtual QSize sizeHint() const override;

private:
  QImage m_playIcon; // checked
    QImage m_pauseIcon;
};


// 信号機
class TrafficLightWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TrafficLightWidget(QWidget *parent = nullptr);
    virtual ~TrafficLightWidget();

    LightWidget *redLight() const
        { return m_red; }
    LightWidget *yellowLight() const
        { return m_yellow; }
    LightWidget *greenLight() const
        { return m_green; }
    RunToggleButtonWidget* runButton() const {
        return run_button;
    }

protected:
    virtual void paintEvent(QPaintEvent *) override;
    virtual QSize sizeHint() const override;
    virtual void showEvent(QShowEvent *event) override;

private:
    QImage m_background;
    LightWidget* m_red;
    LightWidget* m_yellow;
    LightWidget* m_green;
    RunToggleButtonWidget* run_button;

signals:

};

#endif // TRAFFICLIGHTWIDGET_H
