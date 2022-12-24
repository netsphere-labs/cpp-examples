#include "trafficlightwidget.h"
#include <QPainter>
#include <QVBoxLayout>

//////////////////////////////////////////////////////////////
// LightWidget class

LightWidget::LightWidget(const QString& imagePath, QWidget *parent)
    : QWidget(parent), m_image(imagePath), m_on(false)
{
}

void LightWidget::switchLight(bool onoff)
{
    if (onoff == m_on)
        return;
    m_on = onoff;
    update();
}

// override
void LightWidget::paintEvent(QPaintEvent *)
{
    if (!m_on)
        return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(0, 0, m_image);
}

// override
QSize LightWidget::sizeHint() const
{
    return m_image.size();
}


//////////////////////////////////////////////////////////////
// RunToggleButtonWidget class

RunToggleButtonWidget::RunToggleButtonWidget(QWidget *parent) :
    QAbstractButton(parent), m_playIcon(":/play.png"),
    m_pauseIcon(":/pause.png")
{
    setCheckable(true); // トグル可能にする.
}

// override
void RunToggleButtonWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(0, 0, isChecked() ? m_playIcon : m_pauseIcon);
}

// override
QSize RunToggleButtonWidget::sizeHint() const
{
    return isChecked() ? m_playIcon.size() : m_pauseIcon.size();
}


//////////////////////////////////////////////////////////////
// TrafficLightWidget class

TrafficLightWidget::TrafficLightWidget(QWidget *parent)
    : QWidget(parent), m_background(":/background.png")
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0, 40, 0, 80);
    m_red = new LightWidget(":/red.png");
    vbox->addWidget(m_red, 0, Qt::AlignHCenter);
    m_yellow = new LightWidget(":/yellow.png");
    vbox->addWidget(m_yellow, 0, Qt::AlignHCenter);
    m_green = new LightWidget(":/green.png");
    vbox->addWidget(m_green, 0, Qt::AlignHCenter);
    setLayout(vbox);

    // 稼働中 / 故障中ボタン
    run_button = new RunToggleButtonWidget(this);
    auto setButtonGeometry = [this](){
        QSize buttonSize = run_button->size();
        run_button->setGeometry(width() - buttonSize.width() - 20,
                            height() - buttonSize.height() - 20,
                            buttonSize.width(), buttonSize.height());
    };
    connect(run_button, &QAbstractButton::toggled, this, setButtonGeometry);
    //connect(button, &QWidget::show, setButtonGeometry);
}

TrafficLightWidget::~TrafficLightWidget()
{
    delete run_button;
}

// override
void TrafficLightWidget::showEvent(QShowEvent *event)
{
    emit run_button->toggled(false);
}

// override
void TrafficLightWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(0, 0, m_background);
}

// override
QSize TrafficLightWidget::sizeHint() const
{
    return m_background.size();
}
