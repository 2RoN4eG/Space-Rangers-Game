#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QPainter>
#include <QMouseEvent>
#include <QTime>

#include "t_game_components.h"
#include "spaceships/t_spaceship_component.h"
#include "spaceships/t_spaceship_moving_by_trajectory_system.h"
#include "planets/t_planet_circle_moving_system.h"

#include "linear_algebra/t_2d_container_entity.h"
#include "linear_algebra/t_2d_vector_systems.h"

#include <iostream>

t_game_components game_components {};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    repaint();

    std::vector<std::runtime_error> errors;

    complex_test_2d_container(errors);

    complex_test_2d_vector_systems(errors);

    for (const std::runtime_error& error : errors) {
        std::cout << "runtime error: " << error.what() << std::endl;
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* event) {
    const QSize& size = ui->widget->size();

    QPainter painter { this };

    QPen pen(Qt::blue);
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidth(10);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(pen);

    for (const t_planet_game_component& planet : game_components.planets()) {
        painter.drawPoint(planet.get_position().x(), size.height() - planet.get_position().y());
    }

    pen = QPen(Qt::yellow);
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidth(8);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(pen);

    t_spaceship_component& spaceship = game_components.spaceship(t_spaceship_id_entity { 2 });
    for (const t_2d_position_entity enemy_position : spaceship.trajectory()) {
        painter.drawPoint(enemy_position.x(), size.height() - enemy_position.y());
    }

    for (const t_spaceship_component& spaceship : game_components.spaceships()) {
        if (spaceship.is_player()) {
            pen = QPen(Qt::green);
            pen.setCapStyle(Qt::RoundCap);
            pen.setWidth(8);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setPen(pen);
        }
        else {
            pen = QPen(Qt::red);
            pen.setCapStyle(Qt::RoundCap);
            pen.setWidth(4);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setPen(pen);
        }

        painter.drawPoint(spaceship.get_position().x(), size.height() - spaceship.get_position().y());
    }

    resizeEvent(nullptr);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    const QSize& size = ui->widget->size();

    ui->space_ship_spin_box_x->setMaximum(size.width());
    ui->space_ship_spin_box_x_maximum->setText(QString::number(size.width()));

    ui->space_ship_spin_box_y->setMaximum(size.height());
    ui->space_ship_spin_box_y_maximum->setText(QString::number(size.height()));

    ui->space_ship_spin_box_x_enemy->setMaximum(size.width());
    ui->space_ship_spin_box_x_enemy_maximum->setText(QString::number(size.width()));

    ui->space_ship_spin_box_y_enemy->setMaximum(size.height());
    ui->space_ship_spin_box_y_enemy_maximum->setText(QString::number(size.height()));
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    if (!_setting_enemy_path) {
        return;
    }

    const QPoint point = event->pos();

    t_spaceship_component& spaceship = game_components.spaceship(t_spaceship_id_entity { 2 });
    spaceship.trajectory().emplace_back(point.x(), ui->widget->size().height() - point.y());

    repaint();
}

void MainWindow::timerEvent(QTimerEvent* event) {
#define qCurrentTime \
    QTime::currentTime().toString("hh:mm:ss")

        qDebug() << qCurrentTime << "qt timerEvent(QTimerEvent* event)";

    t_spaceship_component& spaceship = game_components.spaceship(t_spaceship_id_entity { 2 });

    t_spaceship_moving_by_trajectory_system spaceship_moving { spaceship };

    spaceship_moving.update();

    t_planet_circle_moving_system planet_circle_moving { game_components };

    planet_circle_moving.update();

    repaint();
}

void MainWindow::on_pushButton_3_clicked() {
    t_spaceship_component& spaceship = game_components.spaceship(t_spaceship_id_entity { 0 });

    spaceship.position() = {
        float (ui->space_ship_spin_box_x->value()),
        float (ui->space_ship_spin_box_y->value())
    };

    std::cout << "spaceship { id: " << spaceship.id() << " } contains position { "
              << float (ui->space_ship_spin_box_x->value()) << ", " << float (ui->space_ship_spin_box_y->value())
              << " }" << std::endl;

    repaint();
}

void MainWindow::on_pushButton_4_clicked() {
    t_spaceship_component& spaceship = game_components.spaceship(t_spaceship_id_entity { 2 });

    spaceship.position() = {
        float (ui->space_ship_spin_box_x_enemy->value()),
        float (ui->space_ship_spin_box_y_enemy->value())
    };

    std::cout << "spaceship { id: " << spaceship.id() << " } contains position { "
              << float (ui->space_ship_spin_box_x_enemy->value()) << ", " << float (ui->space_ship_spin_box_y_enemy->value())
              << " }" << std::endl;

    repaint();
}

void MainWindow::on_pushButton_5_clicked() {
    t_spaceship_component& spaceship = game_components.spaceship(t_spaceship_id_entity { 2 });

    spaceship.trajectory() = {};
    _setting_enemy_path = true;

    repaint();
}

void MainWindow::on_pushButton_6_clicked() {
    _setting_enemy_path = false;

    repaint();
}

void MainWindow::on_pushButton_7_clicked() {
    if (_timer_id > -1) {
        killTimer(_timer_id);
        _timer_id = -1;
        return;
    }

    _timer_id = startTimer(500);
}
