#ifndef GRIDSETTINGS_H
#define GRIDSETTINGS_H

#include <QWidget>

namespace Ui {
class GridSettings;
}

class GridSettings : public QWidget
{
    Q_OBJECT

public:
    explicit GridSettings(QWidget *parent = nullptr);
    ~GridSettings();

private:
    Ui::GridSettings *ui;
};

#endif // GRIDSETTINGS_H
