#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>

namespace Ui {
class ColorButton;
}

class ColorButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ColorButton(QWidget *parent = nullptr);
    ~ColorButton();

    void    setColor(const QColor &color);
    QColor  color() const;

private slots:
    void onClicked();

private:
    Ui::ColorButton *ui;
    QWidget* m_parent;
    QColor m_currentColor;

protected:
    void showEvent(QShowEvent *event);
};

#endif // COLORBUTTON_H
