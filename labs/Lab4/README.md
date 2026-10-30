# Лабораторна робота №4

## Дисципліна
**Об'єктно-орієнтоване програмування**

## Тема
**Вдосконалення структури коду графічного редактора об’єктів на C++**

## Студент
**Чакун Олександр Сергійович**  
Група: **ІМ-о51**  
Номер у списку: **16**

## Варіант Lab4
Номер варіанта дорівнює номеру у списку: **16**.

Оскільки номер парний, реалізовано **динамічний об'єкт `MyEditor`**:
```cpp
MyEditor* g_editor = nullptr;
```
Об'єкт створюється через `new` при `WM_CREATE` і видаляється через `delete` при `WM_DESTROY`.

### Особливості
- кольори і стилі 4 базових фігур збережені як у Lab3;
- «гумовий» слід для всіх фігур — **пунктирний**;
- додано `LineOOShape` — лінія з кружечками на кінцях;
- додано `CubeShape` — каркас куба;
- `LineOOShape` реалізовано як множинне успадкування `LineShape + EllipseShape`;
- `CubeShape` реалізовано як множинне успадкування `LineShape + RectShape`;
- для уникнення дублювання базового `Shape` використано **virtual inheritance**;
- `MyEditor::Start(Shape*)` залежить від абстракції `Shape`, а не від конкретних типів;
- динамічний масив `Shape**` виділяється та розширюється лише за потреби;
- Toolbar містить **6 кнопок** з tooltips.

## Структура
```text
Lab4/
├── Lab4.sln
├── Lab4.vcxproj
├── Lab4.vcxproj.filters
├── README.md
├── src/
│   ├── Lab4.cpp
│   ├── resource.h
│   ├── shape.h / shape.cpp
│   ├── point_shape.h / point_shape.cpp
│   ├── line_shape.h / line_shape.cpp
│   ├── rect_shape.h / rect_shape.cpp
│   ├── ellipse_shape.h / ellipse_shape.cpp
│   ├── line_oo_shape.h / line_oo_shape.cpp
│   ├── cube_shape.h / cube_shape.cpp
│   └── my_editor.h / my_editor.cpp
├── res/
│   ├── Lab4.rc
│   └── toolbar.bmp
├── screenshots/
└── report/
    ├── report.md
    └── assets/
        ├── class_diagram.dot
        └── class_diagram.png
```

## Збірка
1. Відкрити `Lab4.sln` у Visual Studio 2022.
2. Обрати `Debug` і `x64`.
3. Виконати **Build → Rebuild Solution**.
4. Переконатися, що `0 failed`.
5. Натиснути `F5`.
