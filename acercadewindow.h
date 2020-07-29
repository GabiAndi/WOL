#ifndef ACERCADEWINDOW_H
#define ACERCADEWINDOW_H

#include <QMainWindow>

#include <QCloseEvent>

namespace Ui
{
    class AcercaDeWindow;
}

class AcercaDeWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit AcercaDeWindow(QWidget *parent = nullptr);
        ~AcercaDeWindow();

    private:
        Ui::AcercaDeWindow *ui;

        void closeEvent(QCloseEvent *) override;
};

#endif // ACERCADEWINDOW_H
