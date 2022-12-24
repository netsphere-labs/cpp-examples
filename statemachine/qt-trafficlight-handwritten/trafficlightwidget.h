#ifndef TRAFFICLIGHTWIDGET_H
#define TRAFFICLIGHTWIDGET_H

#include <QWidget>

//! [0]
// 一つのライト
class LightWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
public:
    LightWidget(const QColor &color, QWidget *parent = nullptr)
        : QWidget(parent), m_color(color), m_on(false) {}

    bool isOn() const
        { return m_on; }

    void setOn(bool on) {
        if (on == m_on)
            return;
        m_on = on;
        update();
    }

public slots:
    void turnOff() { setOn(false); }
    void turnOn() { setOn(true); }

protected:
    virtual void paintEvent(QPaintEvent *) override;

private:
    QColor m_color;
    bool m_on;
};
//! [0]


// 信号機
class TrafficLightWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TrafficLightWidget(QWidget *parent = nullptr);

    LightWidget *redLight() const
        { return m_red; }
    LightWidget *yellowLight() const
        { return m_yellow; }
    LightWidget *greenLight() const
        { return m_green; }

private:
    LightWidget* m_red;
    LightWidget* m_yellow;
    LightWidget* m_green;

signals:

};

#endif // TRAFFICLIGHTWIDGET_H
