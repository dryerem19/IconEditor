#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    void addItem(QWidget *widget);

private slots:
    void onOkClicked();
    void onApplyClicked();
    void onCancelClicked();
    void onGroupSelectionChanged(int currentRow);

private:
    Ui::SettingsDialog *ui;
    QMap<QString, QWidget*> m_groupWidgets;

    void createConnections();
    void translateUi();
};

#endif // SETTINGSDIALOG_H
