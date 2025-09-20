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
    setColor(getButtonColorFromStylesheet());
}

ColorButton::~ColorButton()
{
    delete ui;
}

void ColorButton::setColor(const QColor &color)
{
    m_currentColor = color;
    setStyleSheet(QString("QPushButton {background-color: %1;}").arg(m_currentColor.name()));
}

QColor ColorButton::color() const
{
    return m_currentColor;
}

void ColorButton::onClicked()
{
    QColor color = QColorDialog::getColor(m_currentColor, m_parent, tr("Выбор цвета"));
    if (color.isValid()) {
        setColor(color);
    }
}

QColor ColorButton::getButtonColorFromStylesheet()
{
    QString styleSheet = this->styleSheet();

    QRegularExpression regex("background-color:\\s*(#[0-9a-fA-F]{6}|#[0-9a-fA-F]{3}|rgb\\([^)]+\\)|rgba\\([^)]+\\)|[a-zA-Z]+)");
    QRegularExpressionMatch match = regex.match(styleSheet);

    if (match.hasMatch())
    {
        QString colorStr = match.captured(1).trimmed();
        return QColor(colorStr);
    }

    return QColor();
}
