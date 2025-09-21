#include "colorbutton.h"
#include "ui_colorbutton.h"

#include <QColorDialog>
#include <QRegularExpression>

ColorButton::ColorButton(QWidget *parent)
    : QPushButton(parent)
    , ui(new Ui::ColorButton)
    , m_parent(parent)
{
    ui->setupUi(this);
    connect(this, &QPushButton::clicked, this, &ColorButton::onClicked);
}

ColorButton::~ColorButton()
{
    delete ui;
}

void ColorButton::setColor(const QColor &color)
{
    if (!color.isValid()) {
        return;
    }

    m_currentColor = color;

    ui->colorLabel->setAutoFillBackground(true);
    QPalette palette = ui->colorLabel->palette();
    palette.setColor(ui->colorLabel->backgroundRole(), color);
    ui->colorLabel->setPalette(palette);
}

QColor ColorButton::color() const
{
    return m_currentColor;
}

void ColorButton::onClicked()
{
    QColor color = QColorDialog::getColor(m_currentColor, m_parent, tr("Выбор цвета"));
    setColor(color);
}

void ColorButton::showEvent(QShowEvent *event)
{
    QPushButton::showEvent(event);

    QString hexColor = this->text();
    QColor initColor = QColor(hexColor);
    setColor(initColor);
}
