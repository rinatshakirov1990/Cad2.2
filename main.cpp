#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <algorithm>

using namespace std;

struct Point {
    double x, y;
};

struct Segment {
    Point start, end;
    double length;
};

// Ввод перемещений
vector<Point> buildContour() {
    vector<Point> points;
    Point current = { 0, 0 };
    points.push_back(current);

    string input;
    cout << "Введите перемещения в формате 'x 3000' или 'y -1000'. Для завершения введите 'end'\n";

    while (true) {
        cout << "> ";
        getline(cin, input);
        if (input == "end") break;

        istringstream iss(input);
        char axis;
        int delta;
        if (iss >> axis >> delta) {
            if (axis == 'x' || axis == 'X') {
                current.x += delta;
            } else if (axis == 'y' || axis == 'Y') {
                current.y += delta;
            } else {
                cout << "Неверная ось. Используйте x или y\n";
                continue;
            }
            points.push_back(current);
        } else {
            cout << "Неверный ввод. Пример: x 3000\n";
        }
    }

    if (points.front().x != points.back().x || points.front().y != points.back().y) {
        cout << "Автоматическое замыкание контура.\n";
        points.push_back(points.front());
    }

    return points;
}

// Пересечение отрезка с прямой
Point intersectionWithLine(Point p1, Point p2, bool vertical, double lineVal, bool& valid) {
    valid = false;
    if (vertical) {
        if ((p1.x - lineVal) * (p2.x - lineVal) > 0 || p1.x == p2.x) return { 0, 0 };
        double t = (lineVal - p1.x) / (p2.x - p1.x);
        double y = p1.y + t * (p2.y - p1.y);
        valid = true;
        return { lineVal, y };
    } else {
        if ((p1.y - lineVal) * (p2.y - lineVal) > 0 || p1.y == p2.y) return { 0, 0 };
        double t = (lineVal - p1.y) / (p2.y - p1.y);
        double x = p1.x + t * (p2.x - p1.x);
        valid = true;
        return { x, lineVal };
    }
}

// Построение сетки вдоль одной линии
void processLine(double lineVal, bool vertical, const vector<Point>& contour, vector<Segment>& result) {
    vector<Point> intersections;
    for (size_t i = 0; i + 1 < contour.size(); ++i) {
        bool valid;
        Point pt = intersectionWithLine(contour[i], contour[i + 1], vertical, lineVal, valid);
        if (valid) intersections.push_back(pt);
    }

    sort(intersections.begin(), intersections.end(), [&](Point a, Point b) {
        return vertical ? a.y < b.y : a.x < b.x;
    });

    for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
        Point a = intersections[i];
        Point b = intersections[i + 1];
        double len = hypot(b.x - a.x, b.y - a.y);
        result.push_back({ a, b, len });
    }
}

int main() {
    //setlocale(LC_ALL, "rus");
    vector<Point> contour = buildContour();

    // Bounding box
    double minX = contour[0].x, maxX = contour[0].x;
    double minY = contour[0].y, maxY = contour[0].y;
    for (const Point& p : contour) {
        minX = min(minX, p.x);
        maxX = max(maxX, p.x);
        minY = min(minY, p.y);
        maxY = max(maxY, p.y);
    }

    const int step = 500;
    vector<Segment> allSegments;

    // Горизонтальные линии
    for (int y = static_cast<int>(minY); y <= static_cast<int>(maxY); y += step) {
        processLine(y, false, contour, allSegments);
    }

    // Вертикальные линии
    for (int x = static_cast<int>(minX); x <= static_cast<int>(maxX); x += step) {
        processLine(x, true, contour, allSegments);
    }

    // Разделение на горизонтальные и вертикальные
    vector<Segment> horizontalSegments;
    vector<Segment> verticalSegments;

    for (const Segment& s : allSegments) {
        if (s.start.y == s.end.y)
            horizontalSegments.push_back(s);
        else if (s.start.x == s.end.x)
            verticalSegments.push_back(s);
    }

    // Сортировка по координатам
    sort(horizontalSegments.begin(), horizontalSegments.end(), [](Segment a, Segment b) {
        return a.start.y < b.start.y;
    });

    sort(verticalSegments.begin(), verticalSegments.end(), [](Segment a, Segment b) {
        return a.start.x < b.start.x;
    });

    // Вывод
    cout << fixed << setprecision(0);

    cout << "\n🟦 Горизонтальные сегменты:\n";
    for (const Segment& s : horizontalSegments) {
        cout << "(" << s.start.x << ", " << s.start.y << ") → ("
             << s.end.x << ", " << s.end.y << ") | Длина: " << s.length << " мм\n";
    }

    cout << "\n🟥 Вертикальные сегменты:\n";
    for (const Segment& s : verticalSegments) {
        cout << "(" << s.start.x << ", " << s.start.y << ") → ("
             << s.end.x << ", " << s.end.y << ") | Длина: " << s.length << " мм\n";
    }

    return 0;
}
